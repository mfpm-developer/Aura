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
#include "aura/core/clock.hpp"
#include "aura/widgets/widget.hpp"
#include <map>
#include <string>

namespace aura
{
    /**
     * @brief Easing functions for animations (linear, in_quad, out_quad, etc).
     */
    using TransitionFunc = std::function<float(float)>;

    struct Transitions {
        static float linear(float t) { return t; }
        static float in_quad(float t) { return t * t; }
        static float out_quad(float t) { return t * (2.0f - t); }
        static float in_out_quad(float t) { return t < 0.5f ? 2.0f * t * t : -1.0f + (4.0f - 2.0f * t) * t; }
        // More can be added as needed
    };

    /**
     * @class Animation
     * @brief A utility class to animate properties of widgets over time.
     */
    class Animation : public EventDispatcher, public std::enable_shared_from_this<Animation>
    {
    public:
        Animation(float duration = 1.0f, TransitionFunc transition = Transitions::linear) 
            : m_duration(duration), m_elapsed(0.0f), m_transition(transition)
        {
            register_event_type("on_start");
            register_event_type("on_complete");
            register_event_type("on_progress");
        }

        virtual ~Animation() = default;

        void bind_property(const std::string& prop_name, float target_value)
        {
            m_targets[prop_name] = target_value;
        }

        void start(std::shared_ptr<Widget> widget)
        {
            if (!widget) return;
            m_widget = widget;
            m_elapsed = 0.0f;
            
            // Re-read current values to animate from dynamically using the AST get_property logic
            for (auto const& [key, val] : m_targets)
            {
                std::any current_val = widget->get_property(key);
                if (current_val.has_value()) {
                    try { m_starts[key] = std::any_cast<float>(current_val); } 
                    catch (...) { 
                        try { m_starts[key] = (float)std::any_cast<int>(current_val); }
                        catch (...) { m_starts[key] = 0.0f; }
                    }
                } else {
                    m_starts[key] = 0.0f;
                }
            }

            dispatch("on_start", widget);

            // Capture self via weak_ptr to avoid memory leaks if Animation is dropped
            std::weak_ptr<Animation> weak_self = shared_from_this();
            Clock::get_instance().schedule_interval([weak_self](float dt) -> bool {
                if (auto self = weak_self.lock()) {
                    return self->update(dt);
                }
                return false; // Cancel clock if animation object is destroyed
            }, 0.0f);
        }

    private:
        bool update(float dt)
        {
            auto widget = m_widget.lock();
            if (!widget) return false;

            m_elapsed += dt;
            float raw_progress = std::min(m_elapsed / m_duration, 1.0f);
            float progress = m_transition(raw_progress);

            // Dynamically set properties
            for (auto const& [key, target] : m_targets)
            {
                float start = m_starts[key];
                float current = start + (target - start) * progress;
                widget->set_property(key, std::to_string(current));
            }

            dispatch("on_progress", raw_progress);

            if (raw_progress >= 1.0f)
            {
                dispatch("on_complete", widget);
                return false; // stop polling
            }

            return true; // keep polling
        }

        float m_duration;
        float m_elapsed;
        TransitionFunc m_transition;
        std::map<std::string, float> m_targets;
        std::map<std::string, float> m_starts;
        std::weak_ptr<Widget> m_widget;
    };
}
