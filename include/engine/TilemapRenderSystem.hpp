#pragma once

#include <memory>
#include <vector>
#include <string>

#include "renderer/RenderSystem.hpp"
#include "renderer/Texture.hpp"
#include "tmx/TmxMap.hpp"

namespace ee::renderer { class Renderer; class Camera; }

namespace ee
{
    // Systeme de rendu qui dessine les calques de tuiles d'une map Tiled (.tmx).
    // Ne depend pas des entites : il dessine directement la map chargee.
    class TilemapRenderSystem : public ee::renderer::RenderSystem
    {
    private:
        ee::tmx::TmxMap m_map;
        std::vector<std::shared_ptr<ee::renderer::Texture>> m_textures; // 1 par tileset
        bool m_loaded = false;

    public:
        bool load(const std::string &_mapPath, ee::renderer::Renderer &_renderer);
        void render(ee::ecs::World &_world, ee::renderer::Renderer &_renderer, ee::renderer::Camera &_camera) override;

        // Cree une entite statique (Transform + RigidBody + Collider AABB) par
        // objet des object layers -> murs/plateformes definis dans Tiled.
        void spawnColliders(ee::ecs::World &_world);

        const ee::tmx::TmxMap &getMap() const { return m_map; }
    };
}
