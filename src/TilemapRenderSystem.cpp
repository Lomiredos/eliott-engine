#include "engine/TilemapRenderSystem.hpp"

#include "renderer/Renderer.hpp"
#include "renderer/Camera.hpp"
#include "tmx/TmxParser.hpp"
#include "math/Rect.hpp"

#include <string>

bool ee::TilemapRenderSystem::load(const std::string &_mapPath, ee::renderer::Renderer &_renderer)
{
    std::optional<ee::tmx::TmxMap> map = ee::tmx::TmxParser::load(_mapPath);
    if (!map)
        return false;
    m_map = *map;

    // dossier de la map, pour resoudre les chemins d'image relatifs
    std::string baseDir;
    size_t slash = _mapPath.find_last_of("/\\");
    if (slash != std::string::npos)
        baseDir = _mapPath.substr(0, slash + 1);

    m_textures.clear();
    for (size_t i = 0; i < m_map.m_tileset.size(); i++)
    {
        std::string imgPath = baseDir + m_map.m_tileset[i].m_path;
        std::string name = "tilemap:" + _mapPath + ":" + std::to_string(i);
        m_textures.push_back(_renderer.createTexture(name, imgPath.c_str()));
    }

    m_loaded = true;
    return true;
}

void ee::TilemapRenderSystem::render(ee::ecs::World &_world, ee::renderer::Renderer &_renderer, ee::renderer::Camera &_camera)
{
    if (!m_loaded)
        return;

    for (const ee::tmx::TmxLayer &layer : m_map.m_layers)
    {
        for (int y = 0; y < layer.m_height; y++)
        {
            for (int x = 0; x < layer.m_width; x++)
            {
                int gid = layer.m_tiles[y * layer.m_width + x];
                if (gid <= 0)
                    continue;

                // tileset = celui au plus grand firstGid <= gid
                int ts = -1;
                for (size_t i = 0; i < m_map.m_tileset.size(); i++)
                    if (m_map.m_tileset[i].m_firstGid <= gid &&
                        (ts < 0 || m_map.m_tileset[i].m_firstGid > m_map.m_tileset[ts].m_firstGid))
                        ts = static_cast<int>(i);

                if (ts < 0 || m_textures[ts] == nullptr || m_map.m_tileset[ts].m_columns <= 0)
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

                _renderer.Draw(*m_textures[ts], dst, src);
            }
        }
    }
}
