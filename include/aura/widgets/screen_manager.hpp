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

#include "aura/widgets/layouts/float_layout.hpp"
#include "aura/properties/string_property.hpp"
#include <map>
#include <string>

namespace aura
{
    /**
     * @class Screen
     * @brief A widget that acts as a single view in a ScreenManager.
     */
    class Screen : public FloatLayout
    {
    public:
        Screen() {}
        virtual ~Screen() {}
        
        StringProperty name{"name", ""};
        
        bool set_property(const std::string& prop_name, const std::string& value) override;
    };

    /**
     * @class ScreenManager
     * @brief Manages navigation between multiple Screen widgets.
     */
    class ScreenManager : public Layout
    {
    public:
        ScreenManager();
        virtual ~ScreenManager() = default;

        StringProperty current{"current", ""};

        void add_widget(std::shared_ptr<Widget> widget) override;
        void remove_widget(std::shared_ptr<Widget> widget) override;
        
        bool set_property(const std::string& name, const std::string& value) override;
        void do_layout() override;

    private:
        void switch_to(const std::string& screen_name);

        std::map<std::string, std::shared_ptr<Screen>> m_screens;
        std::shared_ptr<Screen> m_current_screen = nullptr;
    };
}
