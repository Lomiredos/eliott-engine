#pragma once
#include <unordered_map>
#include "engine/Scene.hpp"

namespace ee
{

    using SceneId = size_t;
    
    class SceneManager
    {

    private:
        std::unordered_map<SceneId, std::unique_ptr<ee::Scene>> m_scenes;
        SceneId m_currentSceneID = 0;
        bool m_hasCurrent = false;

    public:
        SceneId addScene(std::unique_ptr<Scene> _scene)
        {
            SceneId id = m_scenes.size();
            m_scenes[id] = std::move(_scene);
            return id;
        }

        void setCurrentScene(SceneId _id, ee::renderer::Renderer &_renderer)
        {
            if (_id >= m_scenes.size())
                return;

            if (m_hasCurrent)
                m_scenes[m_currentSceneID]->onExit();

            m_currentSceneID = _id;
            m_hasCurrent = true;
            m_scenes[_id]->onEnter(_renderer);
        }

        bool hasCurrentScene() const { return m_hasCurrent; }

        Scene& getCurrentScene()
        {
            return *m_scenes[m_currentSceneID];
        }

        ~SceneManager()
        {
            m_scenes.clear();
        }
    };
}