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

#include "aura/widgets/layouts/layout.hpp"
#include "aura/properties/numeric_property.hpp"
#include "aura/properties/string_property.hpp"

namespace aura
{
    /**
     * @class FloatLayout
     * @brief Honors size_hint and pos_hint properties of its children.
     */
    class FloatLayout : public Layout
    {
    public:
        FloatLayout() = default;
        virtual ~FloatLayout() = default;

        void do_layout() override;
        bool set_property(const std::string& name, const std::string& value) override;
    };
}
