#pragma once

#include <SDL.h>
#include <string>
#include <memory>

namespace Application::Helper {
	struct ColorState final {
		SDL_Color initialColor {};
		SDL_Color currentColor {};
		// the outline should have a colour
		// the button background should have a colour (fill rect)
		// the text should have a colour
		// hovering should show different colours
	};

	struct MessageData final {
		MessageData &operator=(MessageData &) { return *this; }
		std::basic_string<char> msg {};
		std::basic_string<char> fontFile {};
		const SDL_Color &col {};
		int fontSize {0};
	};

	struct ImageData final {
		std::basic_string<char> path;
		std::shared_ptr<SDL_Texture> texture {nullptr};
		int imageWidth {0};
		int imageHeight {0};
	};
	// handle
	using IMD = std::shared_ptr<ImageData>;
} // namespace Application::Helper