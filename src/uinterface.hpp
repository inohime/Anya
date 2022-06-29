#pragma once

#include <SDL.h>
#include "image.hpp"
#include <string>

namespace Application::Helper {
	using IMDW = std::weak_ptr<ImageData>;

	struct Button {
		SDL_Rect box {0};
		IMDW texture;
		MessageData md {};
		bool boxOutline {false};
		int boxWidth {box.w};
		int boxHeight {box.h};
		int boxPosX {box.x};
		int boxPosY {box.y};
	};

	class UInterface final {
	public:

		/** for when you want to explicitly state all of the details
		 *
		 * \param MessageData struct -> msg, fontFile, colour, fontSize
		 * \param texture -> texture of the button
		 * \param x -> x position of the button
		 * \param y -> y position of the button
		 * \param w -> width of the button
		 * \param y -> height of the button
		 * \return Button object filled with all of the essential details for a customized button
		 */
		Button createButton(MessageData &msg, IMD texture, int x, int y, unsigned int w, unsigned int h);
		// for simple button
		Button createButton(std::string_view text, IMD texture, int x, int y, unsigned int w, unsigned int h);
		void setButtonTexture(Button &button, IMD &texture);
		void showOutline(Button &button);
		void update(double dt);
		void draw(Button &button, SDL_Renderer *ren, int x, int y, double sx = 0.0, double sy = 0.0, SDL_Rect *clip = nullptr);

	private:
		Image *ptr {nullptr};
		// change this to Element that holds UI components?
		std::vector<Button> btnList;
	};
} // namespace Application::Helper