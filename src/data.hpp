#pragma once

#include <SDL.h>
#include <string>
#include <memory>

namespace Application::Helper {
	struct PackData final {
		int width {0};
		int height {0};
		int rows {0};
		int columns {0};
	};

	struct MessageData final {
		MessageData &operator=(MessageData &) { return *this; }
		std::basic_string<char> msg {};
		std::basic_string<char> fontFile {};
		const SDL_Color &col {};
		int fontSize {0};
	};

	struct ImageData final {
		std::basic_string<char> path {};
		std::shared_ptr<SDL_Texture> texture {nullptr};
		int imageWidth {0};
		int imageHeight {0};
	};
	// handle
	using IMD = std::shared_ptr<ImageData>;
} // namespace Application::Helper