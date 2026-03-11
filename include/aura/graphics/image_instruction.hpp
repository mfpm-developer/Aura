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
#include <SDL3_image/SDL_image.h>
#include <string>
#include <iostream>

namespace aura
{
    /**
     * @class ImageInstruction
     * @brief A graphics instruction that renders an image from a file using SDL_image.
     */
    class ImageInstruction : public Instruction
    {
    public:
        ImageInstruction(const std::string& source, float x, float y, float width = 100.0f, float height = 100.0f);
        virtual ~ImageInstruction();

        void set_source(const std::string& source);
        void set_bounds(float x, float y, float width, float height);

        float x, y, width, height;

        void apply() override;

    private:
        void load_texture();

        std::string m_source;
        SDL_Texture* m_texture;
        
        // original image size
        float m_img_width = 0.0f;
        float m_img_height = 0.0f;
    };
}
