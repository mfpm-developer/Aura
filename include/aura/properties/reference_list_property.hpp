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
#include <algorithm>

namespace aura
{
    /**
     * @class ReferenceListProperty
     * @brief A composite property that acts as a unified list view over multiple individual properties.
     *        For example, `pos` is a ReferenceListProperty covering `x` and `y`.
     *        Changes to `pos` update `x` and `y`, and changes to `x` or `y` update `pos`.
     */
    template <typename T>
    class ReferenceListProperty : public Property
    {
    public:
        /**
         * @brief Construct a ReferenceListProperty.
         * @param name Name of the composite property (e.g., "pos").
         * @param properties A list of pointers to the individual properties it covers.
         */
        ReferenceListProperty(const std::string& name, const std::vector<Property*>& properties)
            : Property(name), m_properties(properties)
        {
            register_event_type("on_" + name);

            // Bind to the underlying properties so we dispatch when they change individually
            for (auto* prop : m_properties)
            {
                prop->bind("on_" + prop->get_name(), [this](EventDispatcher*, const std::any&) {
                    dispatch("on_" + m_name, get_value());
                    return false;
                });
            }
        }

        virtual ~ReferenceListProperty() = default;

        std::any get() const override
        {
            return get_value();
        }

        std::vector<T> get_value() const
        {
            std::vector<T> values;
            values.reserve(m_properties.size());
            for (const auto* prop : m_properties)
            {
                try {
                    values.push_back(std::any_cast<T>(prop->get()));
                } catch (const std::bad_any_cast&) {
                    // Fallback if casting fails
                }
            }
            return values;
        }

        void set(const std::any& value) override
        {
            if (value.type() == typeid(std::vector<T>))
            {
                set_value(std::any_cast<std::vector<T>>(value));
            }
        }

        void set_value(const std::vector<T>& new_values)
        {
            std::cout << "ReferenceListProperty set_value: " << m_name << std::endl;
            bool changed = false;
            size_t limit = std::min(new_values.size(), m_properties.size());
            
            for (size_t i = 0; i < limit; ++i)
            {
                try {
                    T current_val = std::any_cast<T>(m_properties[i]->get());
                    if (current_val != new_values[i])
                    {
                        m_properties[i]->set(new_values[i]);
                        changed = true;
                    }
                } catch (const std::bad_any_cast&) {
                    // Skip if types are mismatched
                }
            }

            if (changed)
            {
                dispatch("on_" + m_name, get_value());
            }
        }
        void set_value_old(const std::vector<T>& new_values)
        {
            bool changed = false;
            size_t limit = std::min(new_values.size(), m_properties.size());
            
            for (size_t i = 0; i < limit; ++i)
            {
                try {
                    T current_val = std::any_cast<T>(m_properties[i]->get());
                    if (current_val != new_values[i])
                    {
                        m_properties[i]->set(new_values[i]);
                        changed = true;
                    }
                } catch (const std::bad_any_cast&) {
                    // Skip if types are mismatched
                }
            }

            if (changed)
            {
                dispatch("on_" + m_name, get_value());
            }
        }

    private:
        std::vector<Property*> m_properties;
    };
}
