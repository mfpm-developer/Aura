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
#include "aura/core/clock.hpp"
#include <algorithm>

namespace aura
{
    void Clock::schedule_interval(Callback callback, float interval)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_events.push_back({callback, interval, 0.0f, true});
    }

    void Clock::schedule_once(Callback callback, float timeout)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_events.push_back({
            [callback](float dt) -> bool {
                callback(dt);
                return false; // Run once and cancel
            },
            timeout,
            0.0f,
            true
        });
    }

    void Clock::tick(float dt)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        for (auto& event : m_events)
        {
            if (!event.active) continue;

            event.time_elapsed += dt;
            if (event.time_elapsed >= event.interval)
            {
                // Trigger callback
                bool keep_alive = event.callback(dt);
                
                if (keep_alive)
                {
                    event.time_elapsed = 0.0f; // Reset for next interval
                }
                else
                {
                    event.active = false;      // Mark for removal
                }
            }
        }

        // Cleanup inactive events
        m_events.erase(
            std::remove_if(m_events.begin(), m_events.end(),
                [](const Event& e) { return !e.active; }),
            m_events.end()
        );
    }
}
