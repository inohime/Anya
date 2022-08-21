#pragma once

#include <SDL.h>
#include <string>
#include <memory>

namespace Application::Helper {
    struct ColorData final {
        SDL_Color outlineColor {55, 55, 55};
        SDL_Color bgColor {255, 255, 255};
        SDL_Color textColor {0, 0, 0};
    };

    struct MessageData final {
        std::basic_string<char> msg;
        std::basic_string<char> fontFile;
        ColorData col;
        int fontSize;
        int outlineThickness {1};
    };

    struct ImageData final {
        // The file path of the image
        std::basic_string<char> path;
        // The image itself, self-managed memory
        std::shared_ptr<SDL_Texture> texture {nullptr};
        // The width of the image (horizontal)
        int imageWidth;
        // The height of the image (vertical)
        int imageHeight;
        // The image's X position
        int imagePos1;
        // The image's Y position
        int imagePos2;
    };
    // handle
    using IMD = std::shared_ptr<ImageData>;
} // namespace Application::Helper