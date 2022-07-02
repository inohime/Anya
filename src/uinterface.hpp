#pragma once

#include <SDL.h>
#include "image.hpp"
#include <string>
#include <iostream>

namespace Application::Helper {
	struct Button {
		SDL_Rect box {0};
		ImageData texture {};
		SDL_Color initialColor {};
		SDL_Color color {};
		std::basic_string<char> text {};
		bool showOutline {false};
		bool isClickable {false};
		bool canMinimize {false};
		bool canQuit {false};
		// add button color state (for button themes)
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
		//BUTTONPTR createButton(MessageData &msg, IMD &texture, int x, int y, unsigned int w, unsigned int h);
		// for simple button
		// width and height is the size of the gif extracted images
		BUTTONPTR createButton(std::string_view text, IMD texture, SDL_Color col, int x, int y, unsigned int w, unsigned int h);
		std::vector<BUTTONPTR> &getButtonList();
		SDL_Point &getMousePos();
		bool cursorInBounds(BUTTONPTR &button, SDL_Point &mousePos);
		void setButtonTexture(BUTTONPTR &button, IMD &texture);
		void showOutline(BUTTONPTR &button, bool show);
		void update(SDL_Event *ev);
		void draw(BUTTONPTR &button, IMD buttonText, SDL_Renderer *ren, double sx = 0.0, double sy = 0.0, SDL_Rect *clip = nullptr);
		// void fadeEffect(SDL_Renderer *ren, float speed, 

	private:
		std::vector<BUTTONPTR> btnList {};
		SDL_Point mousePos {};
	};
} // namespace Application::Helper