#include "tGameClock.h"
#include "tGraphicsController.h"
#include "tInputController.h"
#include "tObject.h"

tGameClock::tGameClock(int max_fps, int min_fps)
{
	m_curFrameEnd = SDL_GetTicks();
	m_frameLength = 1000 / max_fps;
	m_minFrames = max_fps / min_fps;

	m_done = false;
}

void tGameClock::Loop()
{
	SDL_Event event;
	while (!m_done)
	{
		int frameSkipCount = 0;
		while( SDL_GetTicks() > m_curFrameEnd && frameSkipCount < m_minFrames) 
		{
			while(SDL_PollEvent(&event))
			{
				if (event.type == SDL_QUIT)
				{
					// send stop signal to myself in another thread
					_beginthreadex(NULL, 0, m_quitStruct.m_StopFunction, m_quitStruct.m_argument, 0, NULL); // start demo in another thread
				}
				// next will be a mouse event
				else if (event.type == SDL_KEYDOWN)
				{
					if (event.key.keysym.sym == SDLK_ESCAPE)
					{
						// send stop signal to myself in another thread
						_beginthreadex(NULL, 0, m_quitStruct.m_StopFunction, m_quitStruct.m_argument, 0, NULL); // stop demo in another thread
					}
					//else
					//{
					//	// set the pushed key to true
					//	m_IC->m_pushedKeys[event.key.keysym.sym] = true;
					//}
				}
				/*else if (event.type == SDL_KEYUP)
				{
					m_IC->m_pushedKeys[event.key.keysym.sym] = false;
				}*/
			}

			m_IC->HandleInput(); // cycle through handlers
			Update();

			m_curFrameEnd += m_frameLength;
			frameSkipCount++;
		}
		m_GC->Draw(*m_geoContainer, *m_vaoProgram);
	}
}

void tGameClock::Update()
{
	for (auto i = m_dynamicObjects->begin(); i != m_dynamicObjects->end(); i++)
	{
		(*i)->Update();
	}
}