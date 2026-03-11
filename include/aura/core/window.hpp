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
#pragma once

#include "aura/core/event_dispatcher.hpp"
#include "aura/properties/numeric_property.hpp"
#include "aura/widgets/widget.hpp"
#include <SDL3/SDL.h>
#include <vector>
#include <memory>

namespace aura
{
    struct Transform
    {
        float tx = 0.0f;
        float ty = 0.0f;
        float sx = 1.0f;
        float sy = 1.0f;
        float angle = 0.0f; // degrees
    };

    /**
     * @class Window
     * @brief Singleton Window class managing the main application screen.
     */
    class Window : public EventDispatcher
    {
    public:
        static Window& get_instance()
        {
            static Window instance;
            return instance;
        }

        void create(const std::string& title, int width, int height);
        void destroy();

        void add_widget(std::shared_ptr<Widget> widget);
        void remove_widget(std::shared_ptr<Widget> widget);
        void clear_widgets();
        
        /**
         * @brief Invokes the main render pass.
         */
        void render();
        
        void set_root_size(int w, int h);


        SDL_Window* get_sdl_window() const { return m_sdl_window; }
        SDL_Renderer* get_sdl_renderer() const { return m_sdl_renderer; }

        void push_transform();
        void pop_transform();
        void translate(float x, float y);
        void scale(float x, float y);
        void rotate(float angle);

        Transform get_transform() const;

        NumericProperty<int> width{"width", 800};
        NumericProperty<int> height{"height", 600};

    private:
        Window();
        ~Window();

        Window(const Window&) = delete;
        Window& operator=(const Window&) = delete;

        SDL_Window* m_sdl_window;
        SDL_Renderer* m_sdl_renderer;

        std::vector<Transform> m_transform_stack;
        Transform m_current_transform;

        std::shared_ptr<Canvas> m_canvas;
        std::vector<std::shared_ptr<Widget>> m_children;
    };
}
