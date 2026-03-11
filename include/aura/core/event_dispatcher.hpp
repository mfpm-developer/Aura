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

#include "aura/core/object.hpp"
#include <string>
#include <map>
#include <vector>
#include <functional>
#include <any>

namespace aura
{
    /**
     * @class EventDispatcher
     * @brief The base class for objects that dispatch and handle events.
     *
     * Similar to Kivy's EventDispatcher, this class allows registering custom events
     * and binding callbacks to them.
     */
    class EventDispatcher : public Object
    {
    public:
        using EventCallback = std::function<bool(EventDispatcher*, const std::any&)>;

        EventDispatcher() = default;
        virtual ~EventDispatcher() = default;

        /**
         * @brief Register a new event type.
         * @param event_name The name of the event to register.
         */
        void register_event_type(const std::string& event_name);

        /**
         * @brief Dispatch an event.
         * @param event_name The name of the event to dispatch.
         * @param data Any optional data to pass to the callbacks.
         * @return true if the event was handled by a callback, false otherwise.
         */
        bool dispatch(const std::string& event_name, const std::any& data = {});

        /**
         * @brief Bind a callback to an event.
         * @param event_name The name of the event to bind to.
         * @param callback The function to call when the event is dispatched.
         */
        void bind(const std::string& event_name, EventCallback callback);

        /**
         * @brief Unbind a callback from an event.
         * @param event_name The name of the event to unbind from.
         * @param callback The function to remove.
         */
        void unbind(const std::string& event_name, EventCallback callback);

        /**
         * @brief Removes all callbacks bound to an event.
         * @param event_name The name of the event to clear.
         */
        void unbind_all(const std::string& event_name);

    private:
        struct EventData
        {
            std::vector<EventCallback> callbacks;
        };

        std::map<std::string, EventData> m_events;
    };
}
