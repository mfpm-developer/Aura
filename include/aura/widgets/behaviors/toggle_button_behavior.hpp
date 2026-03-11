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

#include "aura/widgets/behaviors/button_behavior.hpp"
#include "aura/properties/string_property.hpp"
#include <map>
#include <vector>
#include <memory>
#include <algorithm>

namespace aura
{
    /**
     * @class ToggleButtonBehavior
     * @brief A ButtonBehavior subset that stays down until clicked again, or untoggles siblings in the same group.
     */
    class ToggleButtonBehavior : public ButtonBehavior
    {
    public:
        ToggleButtonBehavior();
        virtual ~ToggleButtonBehavior();

        StringProperty group{"group", ""};

        bool on_touch_down(std::shared_ptr<TouchEvent> touch) override
        {
            if (collide_point(touch->x, touch->y)) {
                touch->grab(this);
                if (state.get_value() == "normal") {
                    state.set_value("down");
                } else if (allow_no_selection || group.get_value().empty()) {
                    state.set_value("normal");
                } else {
                    // Enforce allow_no_selection: do nothing
                }
                dispatch("on_press");
                return true;
            }
            return false;
        }

        bool on_touch_up(std::shared_ptr<TouchEvent> touch) override
        {
            if (touch->grab_current == this) {
                touch->ungrab(this);
                if (collide_point(touch->x, touch->y)) {
                    dispatch("on_release");
                }
                return true;
            }
            return false;
        }

        bool set_property(const std::string& name, const std::string& value) override
        {
            if (ButtonBehavior::set_property(name, value)) return true;
            try {
                if (name == "group") { group.set(value); return true; }
            } catch (...) { return false; }
            return false;
        }

        bool allow_no_selection = true; // Kivy allows this by default

    private:
        void update_group();
        static void add_to_group(ToggleButtonBehavior* btn, const std::string& grp);
        static void remove_from_group(ToggleButtonBehavior* btn, const std::string& grp);
        static std::vector<std::shared_ptr<ToggleButtonBehavior>> get_widgets_in_group(const std::string& grp);

        std::string m_last_group;
        inline static std::map<std::string, std::vector<std::weak_ptr<Widget>>> s_groups;
    };
}
