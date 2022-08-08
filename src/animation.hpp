#pragma once

#include <SDL.h>
#include "data.hpp"
#include <map>
#include <string>

// add a spritesheet and iterate over it (assuming the spritesheet is exactly the same width and height)
// TODO: redo this

namespace Application::Helper {
	class Animation {
	public:
		/** Quick animation creation
		 *
		 * \param frames -> number of frames in the gif extraction
		 * \param x -> x position of the image
		 * \param y -> y position of the image
		 * \param w -> width of the image (pixel width)
		 * \param h -> height of the image (pixel height)
		 * \return void -> no return available.
		 */
		void addAnimation(int frames, int x, int y, int w, int h);
		/** Updates the animation frames
		 *
		 * \param speed -> how fast the animation should play
		 * \param dt -> deltaTime from the main loop
		 */
		void update(float speed, double dt);
		/** Renders the animation to the screen
		 * 
		 * \param img -> the button to draw
		 * \param ren -> the renderer to use
		 * \param x -> x position of the animation
		 * \param y -> y position of the animation
		 * \param scale -> scale the animation width and height up or down (0 is the lowest it can go)
		 */
		void draw(IMD &img, SDL_Renderer *ren, int x, int y, double scale = 0.0);

	private:
		float frameTime {0.0f};
		int currentFrame {0};
		std::basic_string<char> animStr {};
		std::map<unsigned int, SDL_Rect> frames {};
	};
} // namespace Application::Helper