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
#include <memory>

namespace aura
{
    /**
     * @class ObjectProperty
     * @brief A property representing an object reference.
     * by default using a shared pointer to a certain type.
     */
    template <typename T>
    class ObjectProperty : public Property
    {
    public:
        ObjectProperty(const std::string& name, std::shared_ptr<T> default_value = nullptr)
            : Property(name), m_value(default_value)
        {
            register_event_type("on_" + name);
        }

        virtual ~ObjectProperty() = default;

        std::any get() const override
        {
            return m_value;
        }

        std::shared_ptr<T> get_value() const
        {
            return m_value;
        }

        void set(const std::any& value) override
        {
            try {
                auto new_value = std::any_cast<std::shared_ptr<T>>(value);
                set_value(new_value);
            } catch (const std::bad_any_cast&) {
                // Ignore invalid types for now
            }
        }

        void set_value(std::shared_ptr<T> new_value)
        {
            if (m_value != new_value)
            {
                m_value = new_value;
                dispatch("on_" + m_name, m_value);
            }
        }

    private:
        std::shared_ptr<T> m_value;
    };
}
