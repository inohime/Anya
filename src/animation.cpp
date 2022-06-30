#include "animation.hpp"

namespace Application::Helper {
	void Animation::addAnimation(std::string_view name, int frames, int x, int y, int w, int h) {
		std::vector<SDL_Rect> frameData;

		SDL_assert(frames != 0);

		for (int i = 0; i < frames; ++i) {
			SDL_Rect newFrame = {(i + x) * w, y, w, h};
			frameData.emplace_back(newFrame);
		}

		this->frames.insert({name, frameData});
	}

	void Animation::update(double dt) {}
} // namespace Application::Helper
