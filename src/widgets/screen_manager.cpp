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
#include "aura/widgets/screen_manager.hpp"
#include <iostream>

namespace aura
{
    bool Screen::set_property(const std::string& prop_name, const std::string& value)
    {
        if (FloatLayout::set_property(prop_name, value)) return true;
        try {
            if (prop_name == "name") { name.set(value); return true; }
        } catch (...) { return false; }
        return false;
    }

    ScreenManager::ScreenManager()
    {
        current.bind("on_current", [this](EventDispatcher*, const std::any& val) {
            std::string target = std::any_cast<std::string>(val);
            this->switch_to(target);
            return false;
        });
        
        // Pass layouts down to the active screen
        auto trigger_layout = [this](EventDispatcher*, const std::any&) {
            this->do_layout();
            return false;
        };

        x.bind("on_x", trigger_layout);
        y.bind("on_y", trigger_layout);
        width.bind("on_width", trigger_layout);
        height.bind("on_height", trigger_layout);
    }

    void ScreenManager::add_widget(std::shared_ptr<Widget> widget)
    {
        auto screen = std::dynamic_pointer_cast<Screen>(widget);
        if (!screen)
        {
            // If it's not a Screen, wrap it in a Screen automatically.
            screen = std::make_shared<Screen>();
            screen->add_widget(widget);
        }

        std::string s_name = screen->name.get_value();
        if (s_name.empty()) {
            std::cerr << "Screen added without a name property. Generated fallback." << std::endl;
            s_name = "screen_" + std::to_string(m_screens.size());
            screen->name.set(s_name);
        }

        m_screens[s_name] = screen;
        
        // If it's the first screen added, make it active
        if (m_screens.size() == 1 && current.get_value().empty()) {
            current.set(s_name);
        } else if (current.get_value() == s_name && m_current_screen == nullptr) {
            // Address Builder order where `current: 'home'` is parsed before 'home' is actually added.
            switch_to(s_name);
        }
    }

    void ScreenManager::remove_widget(std::shared_ptr<Widget> widget)
    {
        auto screen = std::dynamic_pointer_cast<Screen>(widget);
        if (screen)
        {
            std::string s_name = screen->name.get_value();
            m_screens.erase(s_name);
            if (m_current_screen == screen) {
                Layout::remove_widget(screen);
                m_current_screen = nullptr;
            }
        }
    }

    void ScreenManager::switch_to(const std::string& screen_name)
    {
        if (m_screens.find(screen_name) == m_screens.end()) {
            if (m_screens.empty()) return; // Suppress false-positives during initial Builder setup
            std::cerr << "ScreenManager: Screen '" << screen_name << "' not found." << std::endl;
            return;
        }

        auto target_screen = m_screens[screen_name];
        if (m_current_screen == target_screen) return;

        if (m_current_screen) {
            Layout::remove_widget(m_current_screen);
        }

        m_current_screen = target_screen;
        
        // Use base add_widget to attach to the actual render tree
        Layout::add_widget(m_current_screen);
        
        do_layout();
    }

    void ScreenManager::do_layout()
    {
        if (m_current_screen) {
            m_current_screen->x.set(x.get_value());
            m_current_screen->y.set(y.get_value());
            m_current_screen->width.set(width.get_value());
            m_current_screen->height.set(height.get_value());
        }
    }

    bool ScreenManager::set_property(const std::string& name, const std::string& value)
    {
        if (Layout::set_property(name, value)) return true;
        try {
            if (name == "current") { current.set(value); return true; }
        } catch (...) { return false; }
        return false;
    }
}
