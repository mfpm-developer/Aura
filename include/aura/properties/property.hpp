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
#include <string>
#include <any>

namespace aura
{
    /**
     * @class Property
     * @brief The base class for all Aura properties.
     *
     * Properties allow automatic type checking, validation, and event dispatching
     * when their values change.
     */
    class Property : public EventDispatcher
    {
    public:
        Property(const std::string& name) : m_name(name) {}
        virtual ~Property() = default;

        const std::string& get_name() const { return m_name; }

        /**
         * @brief Get the current value of the property.
         * @return The value as std::any.
         */
        virtual std::any get() const = 0;

        /**
         * @brief Set the new value of the property.
         * @param value The value to set.
         */
        virtual void set(const std::any& value) = 0;

    protected:
        std::string m_name;
    };
}
