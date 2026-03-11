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

#include "aura/widgets/widget.hpp"
#include "aura/properties/string_property.hpp"
#include "aura/graphics/image_instruction.hpp"
#include <memory>

namespace aura
{
    /**
     * @class Image
     * @brief A widget that renders a picture from a file.
     */
    class Image : public Widget
    {
    public:
        Image();
        virtual ~Image() = default;

        StringProperty source{"source", ""};

        bool set_property(const std::string& name, const std::string& value) override;

    private:
        std::shared_ptr<ImageInstruction> m_image_instruction;
    };
}
