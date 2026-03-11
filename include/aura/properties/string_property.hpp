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

#include "aura/properties/property.hpp"
#include <string>

namespace aura
{
    /**
     * @class StringProperty
     * @brief A property representing a string value.
     */
    class StringProperty : public Property
    {
    public:
        StringProperty(const std::string& name, const std::string& default_value = "")
            : Property(name), m_value(default_value)
        {
            register_event_type("on_" + name);
        }

        virtual ~StringProperty() = default;

        std::any get() const override
        {
            return m_value;
        }

        const std::string& get_value() const
        {
            return m_value;
        }

        void set(const std::any& value) override
        {
            try {
                if (value.type() == typeid(std::string)) {
                    std::string new_value = std::any_cast<std::string>(value);
                    set_value(new_value);
                } else if (value.type() == typeid(const char*)) {
                    set_value(std::string(std::any_cast<const char*>(value)));
                } 
                // Advanced RTTI checks for char const arrays could go here
            } catch (const std::bad_any_cast&) {
                // Ignore invalid types for now
            }
        }

        // By-pass std::any entirely for string literal setters
        void set(const char* new_value)
        {
            set_value(std::string(new_value));
        }
        
        void set(const std::string& new_value)
        {
            set_value(new_value);
        }

        void set_value(const std::string& new_value)
        {
            if (m_value != new_value)
            {
                m_value = new_value;
                dispatch("on_" + m_name, m_value);
            }
        }

    private:
        std::string m_value;
    };
}
