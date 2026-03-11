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

#include "aura/md/color_scheme.hpp"
#include "aura/md/typography.hpp"
#include "aura/core/event_dispatcher.hpp"

namespace aura {
namespace md {

    // Material 3 interaction states
    enum class State
    {
        Normal,
        Hovered,
        Pressed,
        Focused,
        Disabled
    };

    // Singleton to hold the entire active M3 Theme
    class Theme : public EventDispatcher
    {
    public:
        static Theme& get()
        {
            static Theme instance;
            return instance;
        }

        Theme(const Theme&) = delete;
        Theme& operator=(const Theme&) = delete;

        bool is_dark_mode() const { return m_is_dark; }
        void set_dark_mode(bool dark)
        {
            if (m_is_dark != dark)
            {
                m_is_dark = dark;
                auto new_colors = dark ? ColorScheme::dark() : ColorScheme::light();
                update_colors(new_colors);
                dispatch("on_theme_changed", m_is_dark);
            }
        }

        const ColorScheme& colors() const { return m_colors; }
        const Typography& typography() const { return m_typography; }

        // Helper to get State Layer alpha mapping
        float get_state_layer_alpha(State state) const
        {
            switch (state)
            {
                case State::Hovered: return 0.08f;
                case State::Focused: return 0.12f;
                case State::Pressed: return 0.12f;
                default: return 0.0f;
            }
        }

        void update_colors(const ColorScheme& c)
        {
            m_colors.primary.r = c.primary.r; m_colors.primary.g = c.primary.g; m_colors.primary.b = c.primary.b; m_colors.primary.a = c.primary.a;
            m_colors.on_primary.r = c.on_primary.r; m_colors.on_primary.g = c.on_primary.g; m_colors.on_primary.b = c.on_primary.b; m_colors.on_primary.a = c.on_primary.a;
            
            m_colors.primary_container.r = c.primary_container.r; m_colors.primary_container.g = c.primary_container.g; m_colors.primary_container.b = c.primary_container.b; m_colors.primary_container.a = c.primary_container.a;
            m_colors.on_primary_container.r = c.on_primary_container.r; m_colors.on_primary_container.g = c.on_primary_container.g; m_colors.on_primary_container.b = c.on_primary_container.b; m_colors.on_primary_container.a = c.on_primary_container.a;

            m_colors.secondary.r = c.secondary.r; m_colors.secondary.g = c.secondary.g; m_colors.secondary.b = c.secondary.b; m_colors.secondary.a = c.secondary.a;
            m_colors.on_secondary.r = c.on_secondary.r; m_colors.on_secondary.g = c.on_secondary.g; m_colors.on_secondary.b = c.on_secondary.b; m_colors.on_secondary.a = c.on_secondary.a;
            
            m_colors.secondary_container.r = c.secondary_container.r; m_colors.secondary_container.g = c.secondary_container.g; m_colors.secondary_container.b = c.secondary_container.b; m_colors.secondary_container.a = c.secondary_container.a;
            m_colors.on_secondary_container.r = c.on_secondary_container.r; m_colors.on_secondary_container.g = c.on_secondary_container.g; m_colors.on_secondary_container.b = c.on_secondary_container.b; m_colors.on_secondary_container.a = c.on_secondary_container.a;
            
            m_colors.error.r = c.error.r; m_colors.error.g = c.error.g; m_colors.error.b = c.error.b; m_colors.error.a = c.error.a;
            m_colors.on_error.r = c.on_error.r; m_colors.on_error.g = c.on_error.g; m_colors.on_error.b = c.on_error.b; m_colors.on_error.a = c.on_error.a;
            
            m_colors.surface.r = c.surface.r; m_colors.surface.g = c.surface.g; m_colors.surface.b = c.surface.b; m_colors.surface.a = c.surface.a;
            m_colors.on_surface.r = c.on_surface.r; m_colors.on_surface.g = c.on_surface.g; m_colors.on_surface.b = c.on_surface.b; m_colors.on_surface.a = c.on_surface.a;
            
            m_colors.surface_variant.r = c.surface_variant.r; m_colors.surface_variant.g = c.surface_variant.g; m_colors.surface_variant.b = c.surface_variant.b; m_colors.surface_variant.a = c.surface_variant.a;
            m_colors.on_surface_variant.r = c.on_surface_variant.r; m_colors.on_surface_variant.g = c.on_surface_variant.g; m_colors.on_surface_variant.b = c.on_surface_variant.b; m_colors.on_surface_variant.a = c.on_surface_variant.a;
            
            m_colors.outline.r = c.outline.r; m_colors.outline.g = c.outline.g; m_colors.outline.b = c.outline.b; m_colors.outline.a = c.outline.a;
            m_colors.shadow.r = c.shadow.r; m_colors.shadow.g = c.shadow.g; m_colors.shadow.b = c.shadow.b; m_colors.shadow.a = c.shadow.a;
            m_colors.scrim.r = c.scrim.r; m_colors.scrim.g = c.scrim.g; m_colors.scrim.b = c.scrim.b; m_colors.scrim.a = c.scrim.a;
        }

    private:
        Theme() {
            update_colors(ColorScheme::light());
        }

        bool m_is_dark{false};
        ColorScheme m_colors;
        Typography m_typography;
    };

} // namespace md
} // namespace aura
