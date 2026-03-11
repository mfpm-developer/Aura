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

#include "aura/input/touch_event.hpp"
#include "aura/core/window.hpp"
#include <SDL3/SDL.h>
#include <memory>
#include <iostream>
#include <map>

namespace aura
{
    /**
     * @class SDL3InputProvider
     * @brief Interprets SDL input events and translates them into generic Touch/Motion events.
     */
    class SDL3InputProvider
    {
    public:
        static inline std::map<int, std::shared_ptr<TouchEvent>> active_touches;

        static void process_event(const SDL_Event& e)
        {
            switch (e.type)
            {
                case SDL_EVENT_MOUSE_BUTTON_DOWN:
                {
                    auto touch = std::make_shared<TouchEvent>("mouse", e.button.which, e.button.x, e.button.y);
                    active_touches[e.button.which] = touch;
                    Window::get_instance().dispatch("on_touch_down", touch);
                    break;
                }
                case SDL_EVENT_MOUSE_BUTTON_UP:
                {
                    auto it = active_touches.find(e.button.which);
                    if (it != active_touches.end()) {
                        auto touch = it->second;
                        touch->x = e.button.x;
                        touch->y = e.button.y;
                        Window::get_instance().dispatch("on_touch_up", touch);
                        active_touches.erase(it);
                    } else {
                        auto touch = std::make_shared<TouchEvent>("mouse", e.button.which, e.button.x, e.button.y);
                        Window::get_instance().dispatch("on_touch_up", touch);
                    }
                    break;
                }
                case SDL_EVENT_MOUSE_MOTION:
                {
                    if (e.motion.state != 0) // Only dispatch on_touch_move if a button is held down
                    {
                        auto it = active_touches.find(e.motion.which);
                        if (it != active_touches.end()) {
                            auto touch = it->second;
                            touch->x = e.motion.x;
                            touch->y = e.motion.y;
                            Window::get_instance().dispatch("on_touch_move", touch);
                        } else {
                            auto touch = std::make_shared<TouchEvent>("mouse", e.motion.which, e.motion.x, e.motion.y);
                            Window::get_instance().dispatch("on_touch_move", touch);
                        }
                    }
                    break;
                }
                case SDL_EVENT_TEXT_INPUT:
                {
                    Window::get_instance().dispatch("on_text_input", std::string(e.text.text));
                    break;
                }
                case SDL_EVENT_KEY_DOWN:
                {
                    Window::get_instance().dispatch("on_key_down", e.key);
                    break;
                }
                default:
                    break;
            }
        }
    };
}
