#pragma once

#include <vector>
#include <memory>
#include <SDL3/SDL.h>

#include "ecs/World.hpp"
#include "math/Rect.hpp"
#include "math/Transform.hpp"

#include "renderer/Renderer.hpp"
#include "renderer/Camera.hpp"
#include "renderer/RenderSystem.hpp"
#include "renderer/SpriteRenderSystem.hpp"
#include "renderer/Sprite.hpp"

#include "physics/PhysicsSystem.hpp"
#include "physics/RigidBody.hpp"
#include "physics/Collider.hpp"

#include "animation/AnimationSystem.hpp"
#include "animation/Animation.hpp"

namespace ee
{
    class Scene
    {
    protected:
        ee::ecs::World m_world;
        ee::renderer::Camera m_camera;

        std::shared_ptr<ee::physics::PhysicsSystem> m_physics;
        std::shared_ptr<ee::animation::AnimationSystem> m_animation;
        std::shared_ptr<ee::renderer::SpriteRenderSystem> m_spriteRender;
        std::vector<std::shared_ptr<ee::renderer::RenderSystem>> m_renderSystems;

        Scene(const Scene &) = delete;
        Scene &operator=(const Scene &) = delete;

    public:
        Scene(ee::math::Rect<float> _bounds)
            : m_camera(0, 0, _bounds.getSize().x, _bounds.getSize().y)
        {
            m_physics = m_world.registerSystem<ee::physics::PhysicsSystem>();
            {
                ee::ecs::Signature sig;
                sig.set(ee::ecs::getComponentID<ee::math::Transform>());
                sig.set(ee::ecs::getComponentID<ee::physics::RigidBody>());
                sig.set(ee::ecs::getComponentID<ee::physics::Collider>());
                m_world.setSystemSignature<ee::physics::PhysicsSystem>(sig);
            }
            m_physics->configure(_bounds);

            m_animation = m_world.registerSystem<ee::animation::AnimationSystem>();
            {
                ee::ecs::Signature sig;
                sig.set(ee::ecs::getComponentID<ee::animation::Animation>());
                sig.set(ee::ecs::getComponentID<ee::renderer::Sprite>());
                m_world.setSystemSignature<ee::animation::AnimationSystem>(sig);
            }

            m_spriteRender = m_world.registerSystem<ee::renderer::SpriteRenderSystem>();
            {
                ee::ecs::Signature sig;
                sig.set(ee::ecs::getComponentID<ee::math::Transform>());
                sig.set(ee::ecs::getComponentID<ee::renderer::Sprite>());
                m_world.setSystemSignature<ee::renderer::SpriteRenderSystem>(sig);
            }
            m_renderSystems.push_back(m_spriteRender);
        }

        virtual ~Scene() = default;

        virtual void onEnter(ee::renderer::Renderer &_renderer) {}
        virtual void onExit() {}
        virtual void onEvent(SDL_Event &_e) {}
        virtual void onUpdate(float _dt) {}
        virtual void onRender(ee::renderer::Renderer &_renderer) {}

        void update(float _dt)
        {
            onUpdate(_dt);
            m_world.update(_dt);
        }

        void draw(ee::renderer::Renderer &_renderer)
        {
            m_world.flush();
            for (std::shared_ptr<ee::renderer::RenderSystem> &rs : m_renderSystems)
                rs->render(m_world, _renderer, m_camera);
            onRender(_renderer);
        }
    };

}
