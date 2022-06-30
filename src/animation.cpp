#include "animation.hpp"
#include <iostream>

namespace Application::Helper {
	void Animation::addAnimation(int frames, int x, int y, int w, int h) {
		SDL_assert(frames != 0);

		for (int i = 0; i < frames; ++i) {
			SDL_Rect newFrame = {(i + x) * w, y, w, h};
			this->frames.insert({i, newFrame});
		}
	}

	void Animation::update(float speed, double dt) {
		frameTime += static_cast<float>(dt);

		if (frameTime >= speed) {
			frameTime = 0.0f;
			currentFrame = (currentFrame + 1) % static_cast<int>(frames.size());
		}
	}

	void Animation::draw(IMD &img, SDL_Renderer *ren, int x, int y, double scale){
		SDL_Rect clip = frames[currentFrame];
		SDL_Rect dst {};
		dst.x = x;
		dst.y = y;
		dst.w = clip.w;
		dst.h = clip.h;
		
		if (scale != 0) {
			dst.w *= static_cast<int>(scale);
			dst.h *= static_cast<int>(scale);
		}

		SDL_RenderCopy(ren, img->texture.get(), &clip, &dst);
	}
} // namespace Application::Helper
