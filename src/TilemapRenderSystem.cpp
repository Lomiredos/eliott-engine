#include "engine/TilemapRenderSystem.hpp"

#include "ecs/World.hpp"
#include "math/Rect.hpp"
#include "math/Transform.hpp"
#include "physics/Collider.hpp"
#include "physics/RigidBody.hpp"
#include "renderer/Camera.hpp"
#include "renderer/Renderer.hpp"
#include "tmx/TmxParser.hpp"

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <string>

namespace {
// Bits de flip Tiled dans le GID.
constexpr std::uint32_t FLIP_H = 0x80000000u;
constexpr std::uint32_t FLIP_V = 0x40000000u;
constexpr std::uint32_t GID_MASK = 0x1FFFFFFFu;
} // namespace

bool ee::TilemapRenderSystem::load(const std::string &_mapPath,
                                   ee::renderer::Renderer &_renderer) {
  std::optional<ee::tmx::TmxMap> map = ee::tmx::TmxParser::load(_mapPath);
  if (!map)
    return false;
  m_map = *map;

  // TmxParser resout deja m_path par rapport au dossier de la map
  // (TmxParser::loadTileSet joint basePath/imageSource) -> pas besoin de
  // reprefixer ici, sinon le dossier est duplique.
  m_textures.clear();
  for (size_t i = 0; i < m_map.m_tileset.size(); i++) {
    std::string imgPath = m_map.m_tileset[i].m_path;
    std::string name = "tilemap:" + _mapPath + ":" + std::to_string(i);
    m_textures.push_back(_renderer.createTexture(name, imgPath.c_str()));
  }

  m_loaded = true;
  return true;
}

void ee::TilemapRenderSystem::render(ee::ecs::World &_world,
                                     ee::renderer::Renderer &_renderer,
                                     ee::renderer::Camera &_camera) {
  if (!m_loaded || m_map.m_tileWidth <= 0 || m_map.m_tileHeight <= 0)
    return;

  for (const ee::tmx::TmxLayer &layer : m_map.m_layers) {
    // Culling : on ne parcourt que les tuiles visibles dans la vue camera.
    int startX =
        std::max(0, static_cast<int>(_camera.getX() / m_map.m_tileWidth));
    int startY =
        std::max(0, static_cast<int>(_camera.getY() / m_map.m_tileHeight));
    int endX = std::min(layer.m_width,
                        static_cast<int>((_camera.getX() + _camera.getWidth()) /
                                         m_map.m_tileWidth) +
                            1);
    int endY =
        std::min(layer.m_height,
                 static_cast<int>((_camera.getY() + _camera.getHeight()) /
                                  m_map.m_tileHeight) +
                     1);

    for (int y = startY; y < endY; y++) {
      for (int x = startX; x < endX; x++) {
        std::uint32_t raw =
            static_cast<std::uint32_t>(layer.m_tiles[y * layer.m_width + x]);
        if ((raw & GID_MASK) == 0)
          continue;

        bool flipX = (raw & FLIP_H) != 0;
        bool flipY = (raw & FLIP_V) != 0;
        int gid = static_cast<int>(raw & GID_MASK);

        int ts = -1;
        for (size_t i = 0; i < m_map.m_tileset.size(); i++)
          if (m_map.m_tileset[i].m_firstGid <= gid &&
              (ts < 0 ||
               m_map.m_tileset[i].m_firstGid > m_map.m_tileset[ts].m_firstGid))
            ts = static_cast<int>(i);

        if (ts < 0 || m_textures[ts] == nullptr ||
            m_map.m_tileset[ts].m_columns <= 0)
          continue;

        const ee::tmx::TmxTileset &tileset = m_map.m_tileset[ts];
        int localId = gid - tileset.m_firstGid;
        int col = localId % tileset.m_columns;
        int row = localId / tileset.m_columns;

        ee::math::Rect<float> src(
            static_cast<float>(col * tileset.m_tileWidth),
            static_cast<float>(row * tileset.m_tileHeight),
            static_cast<float>(tileset.m_tileWidth),
            static_cast<float>(tileset.m_tileHeight));

        ee::math::Rect<float> dst(
            _camera.getScreenX(static_cast<float>(x * m_map.m_tileWidth)),
            _camera.getScreenY(static_cast<float>(y * m_map.m_tileHeight)),
            static_cast<float>(tileset.m_tileWidth),
            static_cast<float>(tileset.m_tileHeight));

        _renderer.Draw(*m_textures[ts], dst, src, 0.0f, 255, {}, flipX, flipY);
      }
    }
  }
}

void ee::TilemapRenderSystem::spawnColliders(ee::ecs::World &_world) {
  for (const ee::tmx::TmxObjectGroup &group : m_map.m_objectGroup) {
    // Seul le calque nomme "Collision" genere des colliders.
    if (group.m_name != "Collision")
      continue;

    for (const ee::tmx::TmxObject &obj : group.m_objects) {
      if (obj.m_width <= 0.0f || obj.m_height <= 0.0f)
        continue;

      ee::ecs::EntityID e = _world.createEntity();

      // Tiled : (x,y) = coin haut-gauche, et la rotation pivote autour de ce
      // coin (pas du centre). Notre Transform/SAT tournent la box autour de
      // tr.position -> on doit donc calculer ou finit le centre une fois la
      // rotation appliquee autour du coin, sinon la box est bien tournee mais
      // decalee par rapport a ce qu'affiche Tiled.
      float rad = obj.m_rotation * 3.14159265f / 180.0f;
      float c = std::cos(rad);
      float s = std::sin(rad);
      ee::math::Vector2<float> half(obj.m_width * 0.5f, obj.m_height * 0.5f);
      ee::math::Vector2<float> rotatedHalf(half.x * c - half.y * s,
                                          half.x * s + half.y * c);

      ee::math::Transform tr;
      tr.position = ee::math::Vector2<float>(obj.m_x, obj.m_y) + rotatedHalf;
      tr.rotation = obj.m_rotation;
      _world.addComponent(e, tr);

      ee::physics::RigidBody body;
      body.isStatic = true;
      _world.addComponent(e, body);

      ee::physics::Collider col;
      col.shape = ee::physics::AABB{obj.m_width, obj.m_height};
      _world.addComponent(e, col);
    }
  }
}
