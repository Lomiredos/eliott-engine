#include "engine/Engine.hpp"
#include "engine/Timer.hpp"
#include "SDL3/SDL.h"

#include "audio/AudioManager.hpp"
#include "input/InputManager.hpp"

void ee::Engine::run()
{
    Timer T;
    float start = T.Start();

    while (m_isActive)
    {
        
        if (!ee::input::InputManager::getInstance().update())
            m_isActive = false;
        float end = T.End();
        float deltaTime = (end - start) / 1000.0f;
        start = T.Start();

        m_sceneManager.getCurrentScene().onUpdate(deltaTime);
        m_renderer->Start();
        m_sceneManager.getCurrentScene().onRender();
        m_renderer->End();

        float diff = 1.0f / m_targetFPS - deltaTime;

        if (diff > 0)
            T.Sleep(diff * 1000);
    }

    quit();
}

void ee::Engine::quit()
{
    SDL_Quit();
}

ee::SceneId ee::Engine::addScene(std::unique_ptr<ee::Scene> _scene)
{
    return m_sceneManager.addScene(std::move(_scene));
}
