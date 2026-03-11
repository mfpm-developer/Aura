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

#include "aura/widgets/layouts/box_layout.hpp"
#include "aura/md/text_input.hpp"
#include "aura/md/menu.hpp"
#include "aura/core/window.hpp"
#include <memory>
#include <vector>
#include <string>

namespace aura {
namespace md {

    class MDSelect : public MaterialWidget
    {
    public:
        MDSelect()
        {
            // MDSelect is essentially a read-only TextInput that spawns a Menu
            m_field = std::make_shared<MDTextInput>();
            m_field->hint_text.set("Select option");
            
            this->add_widget(m_field);

            m_menu = std::make_shared<MDMenu>();

            this->bind("on_pos", [this](EventDispatcher*, const std::any&) { this->update_geometry(); return false; });
            this->bind("on_size", [this](EventDispatcher*, const std::any&) { this->update_geometry(); return false; });

            // Trigger menu when the text field block is clicked
            this->bind("on_release", [this](EventDispatcher*, const std::any&) {
                // Open menu below this widget
                float sx = this->pos.get_value()[0];
                float sy = this->pos.get_value()[1] + this->height.get_value();
                m_menu->open(sx, sy);
                return false;
            });

            m_menu->bind("on_selected", [this](EventDispatcher*, const std::any& val) {
                if (val.type() == typeid(std::string)) {
                    std::string selected_text = std::any_cast<std::string>(val);
                    m_field->set_text(selected_text);
                    this->dispatch("on_selected", selected_text);
                }
                return false;
            });

            size_hint_y.set(-1.0f);
            height.set(56.0f);
            size_hint_x.set(-1.0f);
            width.set(280.0f);
        }

        void add_option(const std::string& option)
        {
            m_menu->add_item(option);
        }

    private:
        void update_geometry()
        {
            float w = width.get_value();
            float h = height.get_value();
            float x = pos.get_value()[0];
            float y = pos.get_value()[1];

            m_field->pos.set(std::vector<float>{x, y});
            m_field->width.set(w);
            m_field->height.set(h);
        }

        std::shared_ptr<MDTextInput> m_field;
        std::shared_ptr<MDMenu> m_menu;
    };

} // namespace md
} // namespace aura
