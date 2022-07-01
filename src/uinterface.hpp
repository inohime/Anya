#pragma once

#include <SDL.h>
#include "image.hpp"
#include <string>

namespace Application::Helper {
	/*
	struct Button {
		SDL_Rect box {0};
		ImageData texture {};
		MessageData md {};
		bool boxOutline {false};
		int boxWidth {box.w};
		int boxHeight {box.h};
		int boxPosX {box.x};
		int boxPosY {box.y};
	};
	*/

	struct Button {
		SDL_Rect box {0};
		Helper::ImageData texture {};
		SDL_Color initializedColor {};
		SDL_Color color {};
		std::basic_string<char> text {};
		bool boxOutline {false};
		bool isClickable {false};
	};

	using BUTTONPTR = std::shared_ptr<Button>;

	class UInterface final {
	public:
		/** for when you want to explicitly state all of the details
		 *
		 * \param MessageData struct -> msg, fontFile, colour, fontSize
		 * \param texture -> texture of the button
		 * \param x -> x position of the button
		 * \param y -> y position of the button
		 * \param w -> width of the button
		 * \param h -> height of the button
		 * \return Button object filled with all of the essential details for a customized button.
		 */
		BUTTONPTR createButton(MessageData &msg, IMD &texture, int x, int y, unsigned int w, unsigned int h);
		// for simple button
		BUTTONPTR createButton(std::string_view text, IMD &texture, SDL_Color col, int x, int y, unsigned int w, unsigned int h);
		void setButtonTexture(BUTTONPTR &button, IMD &texture);
		void showOutline(BUTTONPTR &button, bool show);
		void update(SDL_Event *ev);
		void draw(BUTTONPTR &button, IMD &text, SDL_Renderer *ren, double sx = 0.0, double sy = 0.0, SDL_Rect *clip = nullptr);
		// void fadeEffect(SDL_Renderer *ren, float speed, 

	private:
		std::vector<BUTTONPTR> btnList {};
	};
} // namespace Application::Helper