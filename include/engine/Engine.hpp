#pragma once

#include <memory>
#include "renderer/Renderer.hpp"
#include "engine/SceneManager.hpp"

namespace ee
{

    class Engine
    {
    private:
        std::unique_ptr<ee::renderer::Renderer> m_renderer;
        ee::SceneManager m_sceneManager;
        bool m_isActive = true;
        int m_targetFPS;

    public:
        Engine(const char *_name, float _width, float _height, int _targetFPS = 60) : m_renderer(std::make_unique<ee::renderer::Renderer>(_width, _height, _name)) 
        {
            m_targetFPS = _targetFPS;
        }

        void run();
        void quit();

        ee::renderer::Renderer& getRenderer();

        ee::SceneId addScene(std::unique_ptr<ee::Scene> _scene);
    };
}