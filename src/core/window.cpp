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
#include "aura/core/window.hpp"
#include "aura/input/touch_event.hpp"
#include <iostream>
#include <algorithm>

namespace aura
{
    Window::Window() : m_sdl_window(nullptr), m_sdl_renderer(nullptr)
    {
        m_canvas = std::make_shared<Canvas>();

        m_current_transform = Transform();
        m_transform_stack.clear();
        register_event_type("on_resize");
        register_event_type("on_close");
        register_event_type("on_text_input");
        register_event_type("on_key_down");
        register_event_type("on_post_render");

        register_event_type("on_touch_down");
        bind("on_touch_down", [this](EventDispatcher*, const std::any& data) {
            try {
                auto touch = std::any_cast<std::shared_ptr<TouchEvent>>(data);
                for (auto it = m_children.rbegin(); it != m_children.rend(); ++it)
                    if ((*it)->on_touch_down(touch)) return true;
            } catch (const std::bad_any_cast&) {}
            return false;
        });

        register_event_type("on_touch_move");
        bind("on_touch_move", [this](EventDispatcher*, const std::any& data) {
            try {
                auto touch = std::any_cast<std::shared_ptr<TouchEvent>>(data);
                
                // Dispatch to grabbers first
                bool grabbed_handled = false;
                auto grabbers_copy = touch->get_grabbers(); // Copy in case of modification during dispatch
                for (auto* grabber : grabbers_copy) {
                    touch->grab_current = grabber;
                    if (grabber->on_touch_move(touch)) grabbed_handled = true;
                    touch->grab_current = nullptr;
                }
                if (grabbed_handled) return true;

                for (auto it = m_children.rbegin(); it != m_children.rend(); ++it)
                    if ((*it)->on_touch_move(touch)) return true;
            } catch (const std::bad_any_cast&) {}
            return false;
        });

        register_event_type("on_touch_up");
        bind("on_touch_up", [this](EventDispatcher*, const std::any& data) {
            try {
                auto touch = std::any_cast<std::shared_ptr<TouchEvent>>(data);

                // Dispatch to grabbers first
                bool grabbed_handled = false;
                auto grabbers_copy = touch->get_grabbers();
                for (auto* grabber : grabbers_copy) {
                    touch->grab_current = grabber;
                    if (grabber->on_touch_up(touch)) grabbed_handled = true;
                    touch->grab_current = nullptr;
                }
                // Typically you auto-ungrab on touch_up, but Kivy lets widgets ungrab themselves.
                if (grabbed_handled) return true;

                for (auto it = m_children.rbegin(); it != m_children.rend(); ++it)
                    if ((*it)->on_touch_up(touch)) return true;
            } catch (const std::bad_any_cast&) {}
            return false;
        });
    }

    Window::~Window()
    {
        destroy();
    }

    void Window::create(const std::string& title, int width, int height)
    {
        if (m_sdl_window) return;

        this->width.set_value(width);
        this->height.set_value(height);

        m_sdl_window = SDL_CreateWindow(title.c_str(), width, height, SDL_WINDOW_RESIZABLE);
        if (!m_sdl_window)
        {
            std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
            return;
        }

        m_sdl_renderer = SDL_CreateRenderer(m_sdl_window, nullptr);
        if (!m_sdl_renderer)
        {
            std::cerr << "Renderer could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        } else {
            SDL_SetRenderDrawBlendMode(m_sdl_renderer, SDL_BLENDMODE_BLEND);
        }
    }

    void Window::add_widget(std::shared_ptr<Widget> widget)
    {
        if (widget)
        {
            m_children.push_back(widget);
            m_canvas->add(widget->canvas);
        }
    }

    void Window::clear_widgets()
    {
        m_canvas->clear();
        m_canvas->before->clear();
        m_canvas->after->clear();
        m_children.clear();
        std::cout << "Window widgets cleared." << std::endl;
    }

    void Window::remove_widget(std::shared_ptr<Widget> widget)
    {
        auto it = std::find(m_children.begin(), m_children.end(), widget);
        if (it != m_children.end())
        {
            m_canvas->remove(widget->canvas);
            m_children.erase(it);
        }
    }

    void Window::render()
    {
        if (!m_sdl_renderer) return;

        // Reset transform state for the new frame
        m_current_transform = Transform();
        m_transform_stack.clear();

        // Clear the screen with a default black color
        SDL_SetRenderDrawColorFloat(m_sdl_renderer, 0.0f, 0.0f, 0.0f, 1.0f);
        SDL_RenderClear(m_sdl_renderer);

        // Apply all graphics instructions in the root canvas
        m_canvas->draw();

        // Dispatch on_post_render before present
        dispatch("on_post_render", nullptr);

        // Present the rendered image to the screen
        SDL_RenderPresent(m_sdl_renderer);
    }

    void Window::set_root_size(int w, int h)
    {
        if (!m_children.empty())
        {
            auto root = m_children.front();
            root->width.set(w);
            root->height.set(h);
        }
    }

    void Window::push_transform()
    {
        m_transform_stack.push_back(m_current_transform);
    }

    void Window::pop_transform()
    {
        if (!m_transform_stack.empty())
        {
            m_current_transform = m_transform_stack.back();
            m_transform_stack.pop_back();
        }
    }

    void Window::translate(float x, float y)
    {
        m_current_transform.tx += x;
        m_current_transform.ty += y;
    }

    void Window::scale(float x, float y)
    {
        m_current_transform.sx *= x;
        m_current_transform.sy *= y;
    }

    void Window::rotate(float angle)
    {
        m_current_transform.angle += angle;
    }

    Transform Window::get_transform() const
    {
        return m_current_transform;
    }

    void Window::destroy()
    {
        if (m_sdl_renderer)
        {
            SDL_DestroyRenderer(m_sdl_renderer);
            m_sdl_renderer = nullptr;
        }

        if (m_sdl_window)
        {
            SDL_DestroyWindow(m_sdl_window);
            m_sdl_window = nullptr;
        }
    }
}
