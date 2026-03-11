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
#include "aura/widgets/behaviors/toggle_button_behavior.hpp"
#include <iostream>

namespace aura
{
    ToggleButtonBehavior::ToggleButtonBehavior()
    {
        group.bind("on_group", [this](EventDispatcher*, const std::any&) {
            this->update_group();
            return false;
        });

        state.bind("on_state", [this](EventDispatcher* d, const std::any& val) {
            try {
                std::string new_state = std::any_cast<std::string>(val);
                std::cout << "ToggleButtonBehavior state changed to: " << new_state << std::endl;
                if (new_state == "down") {
                    for (auto& btn : get_widgets_in_group(this->group.get_value())) {
                        std::cout << "Checking sibling... " << btn.get() << std::endl;
                        if (btn.get() != this && btn->state.get_value() == "down") {
                            // Prevent infinite loop by unbinding or just setting quietly if we supported it
                            // Since set_value triggers bind, we just call set_value.
                            btn->state.set_value("normal");
                        }
                    }
                } else if (new_state == "normal") {
                    if (!this->allow_no_selection && !this->group.get_value().empty()) {
                        // Are there any other buttons down in the group?
                        bool any_down = false;
                        for (auto& btn : get_widgets_in_group(this->group.get_value())) {
                            if (btn.get() != this && btn->state.get_value() == "down") {
                                any_down = true;
                                break;
                            }
                        }
                        if (!any_down) {
                            // Reverting back to down
                            std::cerr << "ToggleButtonBehavior: Cannot deactivate last active button in group" << std::endl;
                            // Queue this reverting to avoid nested dispatch issues, or just set it:
                            // Calling state.set("down") inside the on_state bind for "normal" is tricky
                            // but Aura's EventDispatcher handles re-entrancy usually.
                            this->state.set_value("down");
                        }
                    }
                }
            } catch (const std::bad_any_cast& e) {
                std::cerr << "bad_any_cast caught in ToggleButtonBehavior on_state bind!" << std::endl;
            }
            return false;
        });
    }

    ToggleButtonBehavior::~ToggleButtonBehavior()
    {
        remove_from_group(this, group.get_value());
    }

    void ToggleButtonBehavior::update_group()
    {
        remove_from_group(this, m_last_group);
        m_last_group = group.get_value();
        add_to_group(this, m_last_group);
    }

    void ToggleButtonBehavior::add_to_group(ToggleButtonBehavior* btn, const std::string& grp)
    {
        if (grp.empty()) return;
        s_groups[grp].push_back(std::weak_ptr<Widget>(btn->weak_from_this()));
    }

    void ToggleButtonBehavior::remove_from_group(ToggleButtonBehavior* btn, const std::string& grp)
    {
        if (grp.empty() || s_groups.find(grp) == s_groups.end()) return;
        auto& vec = s_groups[grp];
        vec.erase(std::remove_if(vec.begin(), vec.end(), [btn](const std::weak_ptr<Widget>& wp) {
            auto sp = wp.lock();
            return !sp || sp.get() == btn; // Also cleanup dead weak_ptrs
        }), vec.end());
    }

    std::vector<std::shared_ptr<ToggleButtonBehavior>> ToggleButtonBehavior::get_widgets_in_group(const std::string& grp)
    {
        std::vector<std::shared_ptr<ToggleButtonBehavior>> result;
        if (grp.empty() || s_groups.find(grp) == s_groups.end()) return result;
        
        auto& vec = s_groups[grp];
        for (auto it = vec.begin(); it != vec.end();) {
            if (auto sp = it->lock()) {
                if (auto casted = std::dynamic_pointer_cast<ToggleButtonBehavior>(sp)) {
                    result.push_back(casted);
                }
                ++it;
            } else {
                it = vec.erase(it);
            }
        }
        return result;
    }
}
