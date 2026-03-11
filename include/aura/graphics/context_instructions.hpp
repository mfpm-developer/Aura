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

#include "aura/graphics/instruction.hpp"
#include "aura/core/window.hpp"
#include <SDL3/SDL.h>

namespace aura
{
    /**
     * @class Color
     * @brief Context instruction to change the drawing color.
     */
    class SetColor : public Instruction
    {
    public:
        SetColor(float r = 1.0f, float g = 1.0f, float b = 1.0f, float a = 1.0f)
            : r(r), g(g), b(b), a(a) {}

        float r, g, b, a;

        void apply() override
        {
            SDL_Renderer* renderer = Window::get_instance().get_sdl_renderer();
            if (renderer)
            {
                SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
                SDL_SetRenderDrawColorFloat(renderer, r, g, b, a);
            }
        }

        bool set_property(const std::string& name, const std::string& value) override
        {
            if (name == "r") r = std::stof(value);
            else if (name == "g") g = std::stof(value);
            else if (name == "b") b = std::stof(value);
            else if (name == "a") a = std::stof(value);
            else return false;
            return true;
        }
    };
    
    // Context Instructions for drawing transformations
    class PushMatrix : public Instruction
    {
    public:
        void apply() override { Window::get_instance().push_transform(); }
    };

    class PopMatrix : public Instruction
    {
    public:
        void apply() override { Window::get_instance().pop_transform(); }
    };

    class Translate : public Instruction
    {
    public:
        float x, y;
        Translate(float x = 0, float y = 0) : x(x), y(y) {}
        void apply() override { Window::get_instance().translate(x, y); }

        bool set_property(const std::string& name, const std::string& value) override
        {
            if (name == "x") x = std::stof(value);
            else if (name == "y") y = std::stof(value);
            else return false;
            return true;
        }
    };

    class Scale : public Instruction
    {
    public:
        float x, y;
        Scale(float x = 1.0f, float y = 1.0f) : x(x), y(y) {}
        void apply() override { Window::get_instance().scale(x, y); }

        bool set_property(const std::string& name, const std::string& value) override
        {
            if (name == "x") x = std::stof(value);
            else if (name == "y") y = std::stof(value);
            else return false;
            return true;
        }
    };

    class Rotate : public Instruction
    {
    public:
        float angle;
        Rotate(float angle = 0.0f) : angle(angle) {}
        void apply() override { Window::get_instance().rotate(angle); }

        bool set_property(const std::string& name, const std::string& value) override
        {
            if (name == "angle") angle = std::stof(value);
            else return false;
            return true;
        }
    };
}
