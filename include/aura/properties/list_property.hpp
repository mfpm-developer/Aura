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
#include <vector>
#include <any>

namespace aura
{
    /**
     * @class ListProperty
     * @brief A property holding a vector of basic types (typically floats or ints).
     */
    template <typename T>
    class ListProperty : public Property
    {
    public:
        ListProperty(const std::string& name, const std::vector<T>& default_value = {})
            : Property(name), m_value(default_value)
        {
            register_event_type("on_" + name);
        }

        virtual ~ListProperty() = default;

        std::any get() const override
        {
            return m_value;
        }

        std::vector<T> get_value() const
        {
            return m_value;
        }

        void set(const std::any& value) override
        {
            if (value.type() == typeid(std::vector<T>))
            {
                set_value(std::any_cast<std::vector<T>>(value));
            }
        }

        void set_value(const std::vector<T>& new_value)
        {
            std::cout << "ListProperty set_value: " << m_name << std::endl;
            if (m_value != new_value)
            {
                m_value = new_value;
                dispatch("on_" + m_name, m_value);
            }
        }
        void set_value_old(const std::vector<T>& new_value)
        {
            if (m_value != new_value)
            {
                m_value = new_value;
                dispatch("on_" + m_name, m_value);
            }
        }

        /**
         * @brief Modifies a single element in the list and triggers the event dispatch.
         * Useful for modifying ListProperty colors like `color[0] = r`.
         */
        void set_element(size_t index, const T& value)
        {
            if (index < m_value.size() && m_value[index] != value)
            {
                m_value[index] = value;
                dispatch("on_" + m_name, m_value);
            }
        }

    private:
        std::vector<T> m_value;
    };
}
