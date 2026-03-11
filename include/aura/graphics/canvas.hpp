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

#include "aura/graphics/instruction.hpp"

namespace aura
{
    /**
     * @class Canvas
     * @brief Represents the drawing area for a Widget.
     * Contains graphics instructions that define the rendering of the widget.
     */
    class Canvas : public InstructionGroup
    {
    public:
        Canvas() = default;
        virtual ~Canvas() = default;

        /**
         * @brief Defines the before-canvas group (instructions executed before main canvas).
         */
        std::shared_ptr<InstructionGroup> before = std::make_shared<InstructionGroup>();

        /**
         * @brief Defines the after-canvas group (instructions executed after main canvas).
         */
        std::shared_ptr<InstructionGroup> after = std::make_shared<InstructionGroup>();

        /**
         * @brief Applies all instructions: before, this canvas (main), and after.
         */
        void apply() override
        {
            if (before) before->apply();
            InstructionGroup::apply(); // Applies m_instructions
            if (after) after->apply();
        }
        
        // Backward compatibility
        void draw() { apply(); }
    };
}
