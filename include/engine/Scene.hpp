#pragma once

#include <SDL3/SDL.h>
#include "ecs/World.hpp"
#include "physics/PhysicsWorld.hpp"
#include "math/Rect.hpp"

namespace ee
{
    class Scene
    {

    protected:
        ee::ecs::World m_world;
        ee::physics::PhysicsWorld m_physicsWorld;

        Scene(const Scene &) = delete;
        Scene &operator=(const Scene &) = delete;
    public:
        Scene(ee::math::Rect<float> _bounds): m_physicsWorld(m_world, _bounds) {}

        virtual ~Scene() = default;
        virtual void onEnter() {}
        virtual void onExit() {}
        virtual void onEvent(SDL_Event &_e) {}
        virtual void onUpdate(float _dt) {}
        virtual void onRender() {}
    };

}