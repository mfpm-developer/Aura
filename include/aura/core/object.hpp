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

#include <string>
#include <atomic>

namespace aura
{
    /**
     * @class Object
     * @brief The base class for all Aura objects.
     * 
     * Provides a unique identifier for instance tracking.
     */
    class Object
    {
    public:
        Object() : m_uid(++s_next_uid) {}
        virtual ~Object() = default;

        Object(const Object&) = delete;
        Object& operator=(const Object&) = delete;

        Object(Object&&) = default;
        Object& operator=(Object&&) = default;

        /**
         * @brief Get the unique identifier of this object.
         * @return The unique identifier.
         */
        unsigned long long get_uid() const
        {
            return m_uid;
        }

    private:
        unsigned long long m_uid;
        static std::atomic<unsigned long long> s_next_uid;
    };
}
