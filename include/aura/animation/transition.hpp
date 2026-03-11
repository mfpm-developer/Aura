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

namespace aura
{
    /**
     * @class Transition
     * @brief A utility class containing easing functions for animations.
     */
    class Transition
    {
    public:
        static float linear(float t) { return t; }
        
        static float in_quad(float t) { return t * t; }
        static float out_quad(float t) { return t * (2 - t); }
        static float in_out_quad(float t) { return t < 0.5f ? 2 * t * t : -1 + (4 - 2 * t) * t; }

        static float in_cubic(float t) { return t * t * t; }
        static float out_cubic(float t) { --t; return t * t * t + 1; }
        static float in_out_cubic(float t) { return t < 0.5f ? 4 * t * t * t : (t - 1) * (2 * t - 2) * (2 * t - 2) + 1; }
    };
}
