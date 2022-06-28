#pragma once

#include <SDL.h>
#include <map>
#include <string>
#include <vector>

// add a sprite sheet and iterate over it (assuming the spritesheet is exactly the same width and height
namespace Application::Helper {
	class Animation {
	public:
		//constexpr uint32_t getCurrentFrame();
		void addAnimation(std::string_view name, int frames, int x, int y, int w, int h);
		//void setCurrentFrame(std::string_view frameName);
		//void stopAnimation();
		void update(double dt);

	private:
		SDL_Texture *ptr {nullptr};
		float frameIndex {0.0f};
		int currentFrame {0};
		// make this an unordered_map?
		std::map<std::basic_string<char>, std::vector<SDL_Rect>> frames;
	};
} // namespace Application::Helper