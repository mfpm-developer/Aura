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
#include <vector>
#include <memory>
#include <algorithm>

namespace aura
{
    /**
     * @class Instruction
     * @brief Base class for all canvas instructions.
     */
    class Instruction : public Object
    {
    public:
        Instruction() = default;
        virtual ~Instruction() = default;

        /**
         * @brief Apply this instruction to the active graphics context.
         */
        virtual void apply() = 0;

        /**
         * @brief Set a property dynamically by name.
         */
        virtual bool set_property(const std::string& name, const std::string& value)
        {
            return false;
        }
    };

    /**
     * @class InstructionGroup
     * @brief A group of graphics instructions.
     */
    class InstructionGroup : public Instruction
    {
    public:
        InstructionGroup() = default;
        virtual ~InstructionGroup() = default;

        void add(std::shared_ptr<Instruction> instruction)
        {
            if (instruction)
            {
                m_instructions.push_back(instruction);
            }
        }

        void remove(std::shared_ptr<Instruction> instruction)
        {
            auto it = std::find(m_instructions.begin(), m_instructions.end(), instruction);
            if (it != m_instructions.end())
            {
                m_instructions.erase(it);
            }
        }

        void clear()
        {
            m_instructions.clear();
        }

        void apply() override
        {
            for (auto& inst : m_instructions)
            {
                inst->apply();
            }
        }

    protected:
        std::vector<std::shared_ptr<Instruction>> m_instructions;
    };
}
