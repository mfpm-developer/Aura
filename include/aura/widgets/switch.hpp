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
#include "aura/properties/numeric_property.hpp"
#include "aura/graphics/vertex_instructions.hpp"
#include "aura/graphics/context_instructions.hpp"

namespace aura
{
    /**
     * @class Switch
     * @brief A simple ON/OFF toggle switch.
     */
    class Switch : public Widget
    {
    public:
        Switch();
        virtual ~Switch() {}

        NumericProperty<bool> active{"active", false};

        bool on_touch_down(std::shared_ptr<TouchEvent> touch) override;
        bool set_property(const std::string& name, const std::string& value) override;

    private:
        void update_graphics();

        std::shared_ptr<SetColor> m_bg_color;
        std::shared_ptr<RoundedRectangle> m_bg_rect;
        
        std::shared_ptr<SetColor> m_thumb_color;
        std::shared_ptr<Ellipse> m_thumb_rect;
    };
}
