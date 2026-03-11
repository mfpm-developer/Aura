/*
 * Copyright (C) 2026 <Your Name or Organization>
 *
 * This file is part of Aura.
 *
 * Aura is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Aura is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Aura.  If not, see <https://www.gnu.org/licenses/>.
 */
#include "aura/core/app.hpp"
#include "aura/core/clock.hpp"
#include "aura/core/window.hpp"
#include "aura/core/factory.hpp"
#include "aura/input/providers/sdl3_input_provider.hpp"
#include "aura/lang/builder.hpp"
#include <iostream>

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <SDL3_image/SDL_image.h>

namespace aura
{
    App::App() : m_running(false)
    {
        register_event_type("on_start");
        register_event_type("on_stop");

        // Ensure widgets are registered with the factory
        init_factory();

        // Initialize SDL3 basics if needed here, or wait until run/window.
        // We'll initialize Video here to ensure it's ready before build().
        if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_EVENTS) < 0)
        {
            std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        }

        // Enable Hardware Anti-Aliasing (MSAA 4x) for smoother geometries
        SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
        SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);

        if (TTF_Init() < 0)
        {
            std::cerr << "SDL_ttf could not initialize! TTF_Error: " << SDL_GetError() << std::endl;
        }
    }

    App::~App()
    {
        TTF_Quit();
        SDL_Quit();
    }

    std::shared_ptr<Widget> App::load_aura(const std::string& filepath)
    {
        auto root = Builder::load_file(filepath);
        if (root)
        {
            Window::get_instance().add_widget(root);
        }
        else
        {
            std::cerr << "App: Failed to load .aura file: " << filepath << std::endl;
        }
        return root;
    }

    void App::run()
    {
        build();

        dispatch("on_start");
        m_running = true;

        auto last_time = SDL_GetTicks();

        SDL_Event e;
        while (m_running)
        {
            auto current_time = SDL_GetTicks();
            float dt = (current_time - last_time) / 1000.0f;
            last_time = current_time;

            while (SDL_PollEvent(&e))
            {
                if (e.type == SDL_EVENT_QUIT)
                {
                    stop();
                }
                else if (e.type == SDL_EVENT_WINDOW_RESIZED)
                {
                    Window::get_instance().width.set_value(e.window.data1);
                    Window::get_instance().height.set_value(e.window.data2);
                    Window::get_instance().dispatch("on_resize");
                }
                
                SDL3InputProvider::process_event(e);
            }

            Clock::get_instance().tick(dt);

            Window::get_instance().render();

            SDL_Delay(16); // ~60 FPS
        }

        dispatch("on_stop");
        Window::get_instance().clear_widgets();
        Window::get_instance().width.unbind_all("on_width");
        Window::get_instance().height.unbind_all("on_height");
    }

    void App::stop()
    {
        m_running = false;
    }
}
