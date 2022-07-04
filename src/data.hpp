#pragma once

#include <SDL.h>
#include <string>
#include <memory>

namespace Application::Helper {
	struct ColorData final {
		SDL_Color currentColor {0};
		SDL_Color hoverColor {0};
		SDL_Color initialColor {currentColor};

		// the outline should have a colour
		// the button background should have a colour (fill rect)
		// the text should have a colour
		// hovering should show different colours
	};

	class ColorState {
	public:
		SDL_Color &setColorTheme();
	
	private:
		SDL_Color currentColor {0};
		SDL_Color hoverColor {0};
		SDL_Color initialColor {currentColor};
		SDL_Color textColor {255, 255, 255};

	};

	struct MessageData final {
		MessageData &operator=(MessageData &) { return *this; }
		std::basic_string<char> msg {};
		std::basic_string<char> fontFile {};
		const SDL_Color &col {};
		int fontSize {0};
		int outlineThickness {1};
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