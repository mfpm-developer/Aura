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
#include "aura/md/theme.hpp"
#include "aura/input/touch_event.hpp"

namespace aura {
namespace md {

    class MaterialWidget : public Widget
    {
    public:
        MaterialWidget() : Widget()
        {
            register_event_type("on_press");
            register_event_type("on_release");

            // Listen for theme changes to trigger a redraw/restyle
            Theme::get().bind("on_theme_changed", [this](EventDispatcher*, const std::any&) {
                this->on_theme_changed();
                return false;
            });
            
            // Interaction tracking bindings
            this->bind("on_touch_down", [this](EventDispatcher*, const std::any& ev) {
                try {
                    auto touch = std::any_cast<std::shared_ptr<aura::TouchEvent>>(ev);
                    if (touch && this->collide_point(touch->x, touch->y)) {
                        if (m_state != State::Disabled) {
                            touch->grab(this);
                            set_state(State::Pressed);
                            dispatch("on_press");
                        }
                    }
                } catch(...) {}
                return false;
            });

            this->bind("on_touch_up", [this](EventDispatcher*, const std::any& ev) {
                try {
                    auto touch = std::any_cast<std::shared_ptr<aura::TouchEvent>>(ev);
                    if (touch && touch->grab_current == this) {
                        touch->ungrab(this);
                        if (m_state != State::Disabled) {
                            set_state(State::Normal);
                            
                            bool hit = this->collide_point(touch->x, touch->y);
                            std::cout << "MaterialWidget on_touch_up."
                                      << " touchXY=" << touch->x << "," << touch->y
                                      << " widgetPos=" << this->x.get_value() << "," << this->y.get_value() 
                                      << " widgetSize=" << this->width.get_value() << "x" << this->height.get_value()
                                      << " COLLIDE=" << (hit ? "true" : "false") << std::endl;

                            if (hit) {
                                dispatch("on_release");
                            }
                        }
                    }
                } catch(...) {}
                return false;
            });

            // Hover states would require a custom mouse motion listener checking bounds
            // but for foundation we have the base set.
        }

        virtual ~MaterialWidget() = default;

        State get_state() const { return m_state; }
        void set_state(State state)
        {
            if (m_state != state)
            {
                m_state = state;
                on_state_changed();
                dispatch("on_state_changed", m_state);
            }
        }

    protected:
        // Lifecycle hooks for subclasses
        virtual void on_theme_changed() 
        {
            // Subclasses will update their canvas instructions here based on new colors
            /* update_canvas() removed */
        }

        virtual void on_state_changed()
        {
            // Subclasses will update state layer overlays (hover/press) here
            /* update_canvas() removed */
        }

        // Utility method to convert aura::Color to std::array<float, 4> used by Base Aura Canvas Instructions right now
        std::array<float, 4> to_array(const Color& c) const
        {
            return {c.r, c.g, c.b, c.a};
        }

    private:
        State m_state{State::Normal};
    };

} // namespace md
} // namespace aura
