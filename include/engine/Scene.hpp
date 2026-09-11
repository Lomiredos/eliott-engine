#pragma once

#include <SDL3/SDL.h>
#include "ecs/World.hpp"
#include "physics/PhysicsWorld.hpp"
#include "math/Rect.hpp"
#include "renderer/Renderer.hpp"
#include "renderer/Camera.hpp"
#include "renderer/SpriteRenderer.hpp"

namespace ee
{
    class Scene
    {

    protected:
        ee::ecs::World m_world;
        ee::physics::PhysicsWorld m_physicsWorld;
        ee::renderer::Camera m_camera;
        ee::renderer::SpriteRenderer m_spriteRenderer;

        Scene(const Scene &) = delete;
        Scene &operator=(const Scene &) = delete;

    public:
        Scene(ee::math::Rect<float> _bounds)
            : m_physicsWorld(m_world, _bounds),
              m_camera(0, 0, _bounds.getSize().x, _bounds.getSize().y),
              m_spriteRenderer(m_world)
        {
        }

        virtual ~Scene() = default;

        virtual void onEnter() {}
        virtual void onExit() {}
        virtual void onEvent(SDL_Event &_e) {}
        virtual void onUpdate(float _dt) {}
        virtual void onRender(ee::renderer::Renderer &_renderer) {}

        // Appele par l'Engine : dessine les sprites de l'ECS, puis le rendu custom.
        void draw(ee::renderer::Renderer &_renderer)
        {
            m_spriteRenderer.render(_renderer, m_camera);
            onRender(_renderer);
        }
    };

}
