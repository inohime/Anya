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
		MessageData &operator=(MessageData &) {return *this;}
		std::basic_string<char> msg {};
		std::basic_string<char> fontFile {};
		ColorData col {};
		int fontSize {0};
		int outlineThickness {1};
	};

	struct ImageData final {
		std::basic_string<char> path;
		std::shared_ptr<SDL_Texture> texture {nullptr};
		int imageWidth {0};
		int imageHeight {0};
		int imagePos1 {0}; // x
		int imagePos2 {0}; // y
	};
	// handle
	using IMD = std::shared_ptr<ImageData>;
} // namespace Application::Helper