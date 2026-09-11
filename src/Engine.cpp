#include "engine/Engine.hpp"
#include "engine/Timer.hpp"
#include "SDL3/SDL.h"

#include "audio/AudioManager.hpp"
#include "input/InputManager.hpp"

void ee::Engine::run()
{
    if (!m_sceneManager.hasCurrentScene())
        m_sceneManager.setCurrentScene(0, *m_renderer);
    if (!m_sceneManager.hasCurrentScene())
        return;

    Timer timer;
    const float fixedDt = 1.0f / static_cast<float>(m_targetFPS);
    float previous = timer.GetCurrentTime();
    float accumulator = 0.0f;

    while (m_isActive)
    {
        if (!ee::input::InputManager::getInstance().update())
            m_isActive = false;

        float current = timer.GetCurrentTime();
        float frameTime = (current - previous) / 1000.0f;
        previous = current;

        if (frameTime > 0.25f)
            frameTime = 0.25f;
        accumulator += frameTime;

        while (accumulator >= fixedDt)
        {
            m_sceneManager.getCurrentScene().update(fixedDt);
            accumulator -= fixedDt;
        }

        m_renderer->Start();
        m_sceneManager.getCurrentScene().draw(*m_renderer);
        m_renderer->End();

        float remaining = (fixedDt - accumulator) * 1000.0f;
        if (remaining > 0.0f)
            timer.Sleep(static_cast<int>(remaining));
    }

    quit();
}

void ee::Engine::quit()
{
    SDL_Quit();
}

ee::SceneManager& ee::Engine::getSceneManager(){
    return m_sceneManager;
}

ee::renderer::Renderer &ee::Engine::getRenderer()
{
    return *m_renderer;
}
ee::SceneId ee::Engine::addScene(std::unique_ptr<ee::Scene> _scene)
{
    return m_sceneManager.addScene(std::move(_scene));
}

void ee::Engine::setCurrentScene(ee::SceneId _id)
{
    m_sceneManager.setCurrentScene(_id, *m_renderer);
}
