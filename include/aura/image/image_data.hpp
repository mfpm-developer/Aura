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

#include "aura/core/object.hpp"
#include <string>

namespace aura
{
    /**
     * @class ImageData
     * @brief Container for pixel data loaded from an image file.
     */
    class ImageData : public Object
    {
    public:
        ImageData(int width, int height, const std::string& fmt, void* data, size_t data_len)
            : width(width), height(height), format(fmt), 
              data(new unsigned char[data_len]), data_length(data_len)
        {
            std::copy(static_cast<unsigned char*>(data), 
                      static_cast<unsigned char*>(data) + data_len, 
                      this->data);
        }

        virtual ~ImageData()
        {
            delete[] data;
        }

        int width;
        int height;
        std::string format;
        unsigned char* data;
        size_t data_length;
    };
}
