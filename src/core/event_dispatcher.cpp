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
#include "aura/core/event_dispatcher.hpp"
#include <iostream>
#include <algorithm>

namespace aura
{
    void EventDispatcher::register_event_type(const std::string& event_name)
    {
        if (m_events.find(event_name) == m_events.end())
        {
            m_events[event_name] = EventData();
        }
    }

    bool EventDispatcher::dispatch(const std::string& event_name, const std::any& data)
    {
        auto it = m_events.find(event_name);
        if (it != m_events.end())
        {
            for (auto& callback : it->second.callbacks)
            {
                if (callback(this, data))
                {
                    return true;
                }
            }
        }
        return false;
    }

    void EventDispatcher::bind(const std::string& event_name, EventCallback callback)
    {
        auto it = m_events.find(event_name);
        if (it != m_events.end())
        {
            it->second.callbacks.push_back(callback);
        }
        else
        {
            // Auto register the event
            register_event_type(event_name);
            m_events[event_name].callbacks.push_back(callback);
        }
    }

    void EventDispatcher::unbind(const std::string& event_name, EventCallback callback)
    {
        // Simple unbind by address comparison is tricky with std::function. 
        // In a real framework, you'd use returning IDs from bind().
        // For simplistic Kivy mock, this is usually omitted or handled via IDs.
        auto it = m_events.find(event_name);
        if (it != m_events.end())
        {
            // Omitted since std::function cannot be compared for equality directly without target().
        }
    }

    void EventDispatcher::unbind_all(const std::string& event_name)
    {
        auto it = m_events.find(event_name);
        if (it != m_events.end())
        {
            it->second.callbacks.clear();
        }
    }
}
