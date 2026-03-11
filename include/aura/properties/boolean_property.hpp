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

namespace aura
{
    /**
     * @class BooleanProperty
     * @brief A property representing a boolean value.
     */
    class BooleanProperty : public Property
    {
    public:
        BooleanProperty(const std::string& name, bool default_value = false)
            : Property(name), m_value(default_value)
        {
            register_event_type("on_" + name);
        }

        virtual ~BooleanProperty() = default;

        std::any get() const override
        {
            return m_value;
        }

        bool get_value() const
        {
            return m_value;
        }

        void set(const std::any& value) override
        {
            if (value.type() == typeid(bool))
            {
                set_value(std::any_cast<bool>(value));
            }
            else if (value.type() == typeid(std::string))
            {
                std::string str_val = std::any_cast<std::string>(value);
                if (str_val == "true" || str_val == "True" || str_val == "1") set_value(true);
                else set_value(false);
            }
        }

        void set_value(bool new_value)
        {
            if (m_value != new_value)
            {
                m_value = new_value;
                dispatch("on_" + m_name, m_value);
            }
        }

    private:
        bool m_value;
    };
}
