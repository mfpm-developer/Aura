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

namespace aura
{
    /**
     * @class ClipboardBase
     * @brief Abstract base class for clipboard operations.
     */
    class ClipboardBase : public Object
    {
    public:
        ClipboardBase() = default;
        virtual ~ClipboardBase() = default;

        virtual void put(const std::string& text) = 0;
        virtual std::string get() const = 0;
    };
}
