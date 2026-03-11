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
#include "aura/graphics/image_instruction.hpp"

namespace aura
{
    ImageInstruction::ImageInstruction(const std::string& source, float x, float y, float width, float height)
        : m_source(source), x(x), y(y), width(width), height(height), m_texture(nullptr)
    {
        load_texture();
    }

    ImageInstruction::~ImageInstruction()
    {
        if (m_texture)
        {
            SDL_DestroyTexture(m_texture);
        }
    }

    void ImageInstruction::set_source(const std::string& source)
    {
        if (m_source != source)
        {
            m_source = source;
            load_texture();
        }
    }

    void ImageInstruction::set_bounds(float x_val, float y_val, float width_val, float height_val)
    {
        x = x_val;
        y = y_val;
        width = width_val;
        height = height_val;
    }

    void ImageInstruction::load_texture()
    {
        SDL_Renderer* renderer = Window::get_instance().get_sdl_renderer();
        if (!renderer) return;

        if (m_texture)
        {
            SDL_DestroyTexture(m_texture);
            m_texture = nullptr;
        }

        if (m_source.empty()) return;

        m_texture = IMG_LoadTexture(renderer, m_source.c_str());
        if (!m_texture)
        {
            std::cerr << "Failed to load image texture from " << m_source << ": " << SDL_GetError() << std::endl;
        }
        else
        {
            float w, h;
            SDL_GetTextureSize(m_texture, &w, &h);
            m_img_width = w;
            m_img_height = h;
            SDL_SetTextureBlendMode(m_texture, SDL_BLENDMODE_BLEND);
        }
    }

    void ImageInstruction::apply()
    {
        if (m_texture)
        {
            SDL_Renderer* renderer = Window::get_instance().get_sdl_renderer();
            if (renderer)
            {
                auto t = Window::get_instance().get_transform();
                SDL_FRect dst_rect = { x * t.sx + t.tx, y * t.sy + t.ty, width * t.sx, height * t.sy };
                if (t.angle != 0.0f) {
                    SDL_FPoint center = { dst_rect.w / 2.0f, dst_rect.h / 2.0f };
                    SDL_RenderTextureRotated(renderer, m_texture, nullptr, &dst_rect, t.angle, &center, SDL_FLIP_NONE);
                } else {
                    SDL_RenderTexture(renderer, m_texture, nullptr, &dst_rect);
                }
            }
        }
    }
}
