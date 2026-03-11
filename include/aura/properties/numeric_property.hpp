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
#include <type_traits>
#include <stdexcept>
#include <iostream>

namespace aura
{
    /**
     * @class NumericProperty
     * @brief A property representing a numeric value (int, float, double, etc).
     */
    template <typename T>
    class NumericProperty : public Property
    {
        static_assert(std::is_arithmetic<T>::value, "NumericProperty requires an arithmetic type");

    public:
        NumericProperty(const std::string& name, T default_value = 0)
            : Property(name), m_value(default_value)
        {
            // Register an event for when the value changes
            register_event_type("on_" + name);
        }

        virtual ~NumericProperty() = default;

        std::any get() const override
        {
            return m_value;
        }

        T get_value() const
        {
            return m_value;
        }

        void set(const std::any& value) override
        {
            try {
                if (value.type() == typeid(T)) {
                    T new_value = std::any_cast<T>(value);
                    set_value(new_value);
                } else if (value.type() == typeid(int)) {
                    set_value(static_cast<T>(std::any_cast<int>(value)));
                } else if (value.type() == typeid(float)) {
                    set_value(static_cast<T>(std::any_cast<float>(value)));
                } else if (value.type() == typeid(double)) {
                    set_value(static_cast<T>(std::any_cast<double>(value)));
                } else {
                    std::cerr << "NumericProperty(" << m_name << ")::set unsupported type: " << value.type().name() << std::endl;
                }
            } catch (const std::bad_any_cast& e) {
                std::cerr << "NumericProperty(" << m_name << ")::set bad_any_cast" << std::endl;
            }
        }

        void set_value(T new_value)
        {
            if (m_value != new_value)
            {
                m_value = new_value;
                dispatch("on_" + m_name, m_value);
            }
        }

    private:
        T m_value;
    };
}
