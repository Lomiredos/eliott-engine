#include "engine/Engine.hpp"
#include "engine/Timer.hpp"
#include "SDL3/SDL.h"

void ee::Engine::run()
{
    Timer T;
	float start = T.Start();

    SDL_Event event;
    while (m_isActive){
        SDL_PollEvent(&event);
        m_sceneManager.getCurrentScene().onEvent(event);

        float end = T.End();
		float deltaTime = (end - start) / 1000.0f;
		start = T.Start();


        m_sceneManager.getCurrentScene().onUpdate(deltaTime);
        m_renderer->Start();
        m_sceneManager.getCurrentScene().onRender();
        m_renderer->End();

        		float diff = 1.0f / m_targetFPS - deltaTime;

		if (diff > 0)
			T.Sleep(diff *1000);
    }

    quit();



}

void ee::Engine::quit()
{
}
