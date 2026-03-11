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
#include "aura/widgets/image.hpp"

namespace aura
{
    Image::Image()
    {
        m_image_instruction = std::make_shared<ImageInstruction>(
            source.get_value(),
            x.get_value(),
            y.get_value(),
            width.get_value(),
            height.get_value()
        );

        canvas->add(m_image_instruction);

        source.bind("on_source", [this](EventDispatcher*, const std::any& val) {
            this->m_image_instruction->set_source(std::any_cast<std::string>(val));
            return false;
        });

        auto update_bounds = [this](EventDispatcher*, const std::any&) {
            this->m_image_instruction->set_bounds(
                this->x.get_value(),
                this->y.get_value(),
                this->width.get_value(),
                this->height.get_value()
            );
            return false;
        };

        x.bind("on_x", update_bounds);
        y.bind("on_y", update_bounds);
        width.bind("on_width", update_bounds);
        height.bind("on_height", update_bounds);
    }

    bool Image::set_property(const std::string& name, const std::string& value)
    {
        if (Widget::set_property(name, value)) return true;
        try {
            if (name == "source") { source.set(value); return true; }
        } catch (...) { return false; }
        return false;
    }
}
