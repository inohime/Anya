#pragma once

#include <SDL.h>
#include "image.hpp"

namespace Application::Helper {
	class UInterface final {
	public:
		void createButton();
		void setButtonTexture(SDL_Texture *tex);
		void update(double dt);

	private:

	};
} // namespace Application::Helper