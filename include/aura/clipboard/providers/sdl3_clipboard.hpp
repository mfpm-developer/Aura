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

#include "aura/clipboard/clipboard_base.hpp"
#include <SDL3/SDL.h>

namespace aura
{
    /**
     * @class SDL3Clipboard
     * @brief SDL3 implementation for clipboard operations.
     */
    class SDL3Clipboard : public ClipboardBase
    {
    public:
        SDL3Clipboard() = default;
        virtual ~SDL3Clipboard() = default;

        void put(const std::string& text) override
        {
            SDL_SetClipboardText(text.c_str());
        }

        std::string get() const override
        {
            if (SDL_HasClipboardText())
            {
                char* text = SDL_GetClipboardText();
                if (text)
                {
                    std::string result(text);
                    SDL_free(text);
                    return result;
                }
            }
            return "";
        }
    };
}
