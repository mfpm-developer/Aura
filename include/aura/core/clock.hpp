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

#include <functional>
#include <vector>
#include <mutex>
#include <string>

namespace aura
{
    /**
     * @class Clock
     * @brief Singleton class to schedule functions to be called at specific intervals.
     */
    class Clock
    {
    public:
        using Callback = std::function<bool(float)>;

        struct Event
        {
            Callback callback;
            float interval;
            float time_elapsed;
            bool active;
        };

        static Clock& get_instance()
        {
            static Clock instance;
            return instance;
        }

        /**
         * @brief Schedule a function to be called every `interval` seconds.
         * @param callback The function to call. It receives the elapsed time. Returning false cancels the schedule.
         * @param interval The interval in seconds.
         */
        void schedule_interval(Callback callback, float interval);

        /**
         * @brief Schedule a function to be called once after `timeout` seconds.
         * @param callback The function to call.
         * @param timeout The timeout in seconds.
         */
        void schedule_once(Callback callback, float timeout = 0.0f);

        /**
         * @brief Update the clock and trigger scheduled events. Should be called every frame.
         * @param dt Delta time since the last frame in seconds.
         */
        void tick(float dt);

    private:
        Clock() = default;
        ~Clock() = default;

        Clock(const Clock&) = delete;
        Clock& operator=(const Clock&) = delete;

        std::vector<Event> m_events;
        std::mutex m_mutex;
    };
}
