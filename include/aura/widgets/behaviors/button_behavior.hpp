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

#include "aura/widgets/widget.hpp"
#include "aura/properties/string_property.hpp"
#include "aura/input/touch_event.hpp"

namespace aura
{
    /**
     * @class ButtonBehavior
     * @brief Mixin class that provides button behavior (handling touch down/up, states).
     */
    class ButtonBehavior : public virtual Widget
    {
    public:
        ButtonBehavior()
        {
            register_event_type("on_press");
            register_event_type("on_release");

            // We listen to the window for raw touches or the parent widget hierarchy
            // For simplicity, we just bind to our own events that will be dispatched by layout
        }

        virtual ~ButtonBehavior() = default;

        StringProperty state{"state", "normal"}; // "normal" or "down"


        virtual bool on_touch_down(std::shared_ptr<TouchEvent> touch)
        {
            if (collide_point(touch->x, touch->y))
            {
                std::cout << "ButtonBehavior: Collided! state=" << state.get_value() << std::endl;
                if (state.get_value() == "normal")
                {
                    touch->grab(this);
                    state.set_value("down");
                    dispatch("on_press");
                    return true;
                }
            }
            return false;
        }

        virtual bool on_touch_up(std::shared_ptr<TouchEvent> touch)
        {
            if (touch->grab_current == this)
            {
                std::cout << "ButtonBehavior: on_touch_up grab matched!" << std::endl;
                touch->ungrab(this);
                if (state.get_value() == "down")
                {
                    state.set_value("normal");
                    dispatch("on_release");
                }
                return true;
            }
            return false;
        }
    };
}
