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
#include <SDL3_ttf/SDL_ttf.h>
#include <string>
#include <iostream>

namespace aura
{
    /**
     * @class TextInstruction
     * @brief A graphics instruction that renders text using SDL_ttf.
     */
    class TextInstruction : public Instruction
    {
    public:
        TextInstruction(const std::string& text, float x, float y, const std::string& font_path = "Roboto-Regular.ttf", int font_size = 24)
            : m_text(text), x(x), y(y), m_font_path(font_path), m_font_size(font_size), m_texture(nullptr), m_font(nullptr)
        {
            update_texture();
        }

        virtual ~TextInstruction()
        {
            if (m_texture) SDL_DestroyTexture(m_texture);
            if (m_font) TTF_CloseFont(m_font);
        }

        void set_text(const std::string& text)
        {
            if (m_text != text)
            {
                m_text = text;
                update_texture();
            }
        }

        void set_font_size(int size)
        {
            if (m_font_size != size)
            {
                m_font_size = size;
                update_texture();
            }
        }

        std::string get_text() const { return m_text; }

        float x;
        float y;

        float get_width() const { return m_width; }
        float get_height() const { return m_height; }

        void apply() override
        {
            if (m_texture)
            {
                SDL_Renderer* renderer = Window::get_instance().get_sdl_renderer();
                if (renderer)
                {
                    float r, g, b, a;
                    if (SDL_GetRenderDrawColorFloat(renderer, &r, &g, &b, &a)) {
                        SDL_SetTextureColorModFloat(m_texture, r, g, b);
                        SDL_SetTextureAlphaModFloat(m_texture, a);
                    }

                    auto t = Window::get_instance().get_transform();
                    SDL_FRect dst_rect = { x * t.sx + t.tx, y * t.sy + t.ty, m_width * t.sx, m_height * t.sy };
                    if (t.angle != 0.0f) {
                        SDL_FPoint center = { dst_rect.w / 2.0f, dst_rect.h / 2.0f };
                        SDL_RenderTextureRotated(renderer, m_texture, nullptr, &dst_rect, t.angle, &center, SDL_FLIP_NONE);
                    } else {
                        SDL_RenderTexture(renderer, m_texture, nullptr, &dst_rect);
                    }
                }
            }
        }

    private:
        void update_texture()
        {
            if (m_font)
            {
                TTF_CloseFont(m_font);
                m_font = nullptr;
            }

            if (m_text.empty()) {
                m_width = 0.0f;
                m_height = 0.0f;
                return;
            }

            SDL_IOStream* io = SDL_IOFromFile(m_font_path.c_str(), "r");
            if (io) {
                m_font = TTF_OpenFontIO(io, true, m_font_size);
            }

            if (!m_font)
            {
                // Try fallback fonts if the specified one fails (useful for local Linux vs Android)
                std::vector<std::string> fallbacks = {
                    "Roboto-Regular.ttf",
                    "/usr/share/fonts/truetype/freefont/FreeSans.ttf",
                    "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
                    "/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf"
                };
                for (const auto& fallback : fallbacks) {
                    SDL_IOStream* fallback_io = SDL_IOFromFile(fallback.c_str(), "r");
                    if (fallback_io) {
                        m_font = TTF_OpenFontIO(fallback_io, true, m_font_size);
                        if (m_font) {
                            m_font_path = fallback; // update path so we don't spam fallbacks next time
                            break;
                        }
                    }
                }
            }

            if (!m_font)
            {
                SDL_Log("Failed to load font %s: %s", m_font_path.c_str(), SDL_GetError());
                return;
            }

            SDL_Color color = { 255, 255, 255, 255 }; 
            SDL_Surface* surface = TTF_RenderText_Blended(m_font, m_text.c_str(), m_text.length(), color);
            if (!surface)
            {
                SDL_Log("Failed to render text surface: %s", SDL_GetError());
                return;
            }

            // Always update width and height based on the surface measurement regardless of renderer state
            m_width = static_cast<float>(surface->w);
            m_height = static_cast<float>(surface->h);

            SDL_Renderer* renderer = Window::get_instance().get_sdl_renderer();
            if (renderer)
            {
                if (m_texture)
                {
                    SDL_DestroyTexture(m_texture);
                    m_texture = nullptr;
                }

                m_texture = SDL_CreateTextureFromSurface(renderer, surface);
                if (!m_texture) {
                    SDL_Log("Failed to create texture: %s", SDL_GetError());
                } else {
                    SDL_SetTextureBlendMode(m_texture, SDL_BLENDMODE_BLEND);
                }
            }

            SDL_DestroySurface(surface);
        }

        std::string m_text;
        std::string m_font_path;
        int m_font_size;
        
        float m_width = 0.0f;
        float m_height = 0.0f;

        SDL_Texture* m_texture;
        TTF_Font* m_font;
    };
}
