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

#include "aura/input/motion_event.hpp"
#include <vector>
#include <chrono>

namespace aura
{
    /**
     * @class TouchEvent
     * @brief A motion event representing a touch interaction.
     * Contains properties for touch actions such as double-tap, continuous motion history, etc.
     */
    class TouchEvent : public MotionEvent
    {
    public:
        TouchEvent(const std::string& device, int id, float x, float y)
            : MotionEvent(device, id, x, y)
        {
            is_touch = true;
            m_start_time = std::chrono::steady_clock::now();
        }

        virtual ~TouchEvent() = default;

        bool is_double_tap = false;
        bool is_triple_tap = false;

        float time_start() const
        {
            // Time duration since start
            return 0.0f; // placeholder for real steady_clock diff
        }

        // Touch grabbing mechanics
        void grab(class Widget* widget) {
            if (std::find(m_grabbers.begin(), m_grabbers.end(), widget) == m_grabbers.end()) {
                m_grabbers.push_back(widget);
            }
        }

        void ungrab(class Widget* widget) {
            auto it = std::find(m_grabbers.begin(), m_grabbers.end(), widget);
            if (it != m_grabbers.end()) {
                m_grabbers.erase(it);
            }
        }
        
        const std::vector<class Widget*>& get_grabbers() const {
            return m_grabbers;
        }

        // State used by the Window event dispatcher to denote if a grabber is processing it
        class Widget* grab_current = nullptr;

    private:
        std::chrono::steady_clock::time_point m_start_time;
        std::vector<class Widget*> m_grabbers;
    };
}
