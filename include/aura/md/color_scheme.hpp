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

#include "aura/graphics/color.hpp"

namespace aura {
namespace md {

    // Represents the Material Design 3 Color Roles
    struct ColorScheme
    {
        Color primary;
        Color on_primary;
        Color primary_container;
        Color on_primary_container;

        Color secondary;
        Color on_secondary;
        Color secondary_container;
        Color on_secondary_container;

        Color tertiary;
        Color on_tertiary;
        Color tertiary_container;
        Color on_tertiary_container;

        Color error;
        Color on_error;
        Color error_container;
        Color on_error_container;

        Color background;
        Color on_background;
        Color surface;
        Color on_surface;
        Color surface_variant;
        Color on_surface_variant;
        Color outline;
        Color outline_variant;

        Color inverse_surface;
        Color inverse_on_surface;
        Color inverse_primary;

        Color scrim;
        Color shadow;

        static aura::Color hex_to_color(const std::string& hex)
        {
            return aura::Color::from_hex(hex);
        }

        // Default Baseline Light Theme matching M3 specs
        static ColorScheme light()
        {
            return {
                .primary = hex_to_color("#6750A4"),
                .on_primary = hex_to_color("#FFFFFF"),
                .primary_container = hex_to_color("#EADDFF"),
                .on_primary_container = hex_to_color("#21005D"),
                .secondary = hex_to_color("#625B71"),
                .on_secondary = hex_to_color("#FFFFFF"),
                .secondary_container = hex_to_color("#E8DEF8"),
                .on_secondary_container = hex_to_color("#1D192B"),
                .tertiary = hex_to_color("#7D5260"),
                .on_tertiary = hex_to_color("#FFFFFF"),
                .tertiary_container = hex_to_color("#FFD8E4"),
                .on_tertiary_container = hex_to_color("#31111D"),
                .error = hex_to_color("#B3261E"),
                .on_error = hex_to_color("#FFFFFF"),
                .error_container = hex_to_color("#F9DEDC"),
                .on_error_container = hex_to_color("#410E0B"),
                .background = hex_to_color("#FEF7FF"),
                .on_background = hex_to_color("#1D1B20"),
                .surface = hex_to_color("#FEF7FF"),
                .on_surface = hex_to_color("#1D1B20"),
                .surface_variant = hex_to_color("#E7E0EC"),
                .on_surface_variant = hex_to_color("#49454F"),
                .outline = hex_to_color("#79747E"),
                .outline_variant = hex_to_color("#CAC4D0"),
                .inverse_surface = hex_to_color("#322F35"),
                .inverse_on_surface = hex_to_color("#F5EFF7"),
                .inverse_primary = hex_to_color("#D0BCFF"),
                .scrim = hex_to_color("#000000"),
                .shadow = hex_to_color("#000000")
            };
        }

        // Default Baseline Dark Theme matching M3 specs
        static ColorScheme dark()
        {
            return {
                .primary = hex_to_color("#D0BCFF"),
                .on_primary = hex_to_color("#381E72"),
                .primary_container = hex_to_color("#4F378B"),
                .on_primary_container = hex_to_color("#EADDFF"),
                .secondary = hex_to_color("#CCC2DC"),
                .on_secondary = hex_to_color("#332D41"),
                .secondary_container = hex_to_color("#4A4458"),
                .on_secondary_container = hex_to_color("#E8DEF8"),
                .tertiary = hex_to_color("#EFB8C8"),
                .on_tertiary = hex_to_color("#492532"),
                .tertiary_container = hex_to_color("#633B48"),
                .on_tertiary_container = hex_to_color("#FFD8E4"),
                .error = hex_to_color("#F2B8B5"),
                .on_error = hex_to_color("#601410"),
                .error_container = hex_to_color("#8C1D18"),
                .on_error_container = hex_to_color("#F9DEDC"),
                .background = hex_to_color("#141218"),
                .on_background = hex_to_color("#E6E0E9"),
                .surface = hex_to_color("#141218"),
                .on_surface = hex_to_color("#E6E0E9"),
                .surface_variant = hex_to_color("#49454F"),
                .on_surface_variant = hex_to_color("#CAC4D0"),
                .outline = hex_to_color("#938F99"),
                .outline_variant = hex_to_color("#49454F"),
                .inverse_surface = hex_to_color("#E6E0E9"),
                .inverse_on_surface = hex_to_color("#322F35"),
                .inverse_primary = hex_to_color("#6750A4"),
                .scrim = hex_to_color("#000000"),
                .shadow = hex_to_color("#000000")
            };
        }
    };

} // namespace md
} // namespace aura
