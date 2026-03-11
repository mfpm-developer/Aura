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
#include "aura/properties/numeric_property.hpp"
#include "aura/graphics/text_instruction.hpp"
#include "aura/graphics/context_instructions.hpp"
#include <memory>

namespace aura
{
    /**
     * @class Label
     * @brief A widget that renders text.
     */
    class Label : public Widget
    {
    public:
        Label();
        virtual ~Label() = default;

        StringProperty text{"text", ""};
        NumericProperty<int> font_size{"font_size", 24};
        StringProperty font_name{"font_name", "Roboto-Regular.ttf"};
        StringProperty halign{"halign", "center"}; 
        StringProperty valign{"valign", "center"};

        bool set_property(const std::string& name, const std::string& value) override;

    private:
        void update_text_instruction();
        void update_alignment();

        std::shared_ptr<SetColor> m_text_color;
        std::shared_ptr<TextInstruction> m_text_instruction;
    };
}
