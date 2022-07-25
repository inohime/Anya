#pragma once

#include <SDL.h>
#include "data.hpp"
#include <nfd.h>
#include <string>
#include <unordered_map>

class Scene;

namespace Application::Helper {
	struct Button {
		SDL_Rect box {0};
		ImageData texture {};
		ColorData buttonColor {};
		// 75% of 255
		float colorAlpha {191.25f};
		std::basic_string<char> text {};
		bool canMinimize {false};
		bool canQuit {false};
		bool isEnabled {false};
	};

	using BUTTONPTR = std::shared_ptr<Button>;

	class UInterface final {
	public:
		/** Create a button with a texture.
		 *
		 * \param text -> the text within the button
		 * \param texture -> texture of the button
		 * \param x -> x position of the button
		 * \param y -> y position of the button
		 * \param w -> width of the button
		 * \param h -> height of the button
		 * \return Button object filled with all of the essential details for a customized button.
		 */
		BUTTONPTR createButton(std::string_view text, IMD texture, int x, int y, uint32_t w, uint32_t h);
		/** Create a normal button.
		 *
		 * \param text -> the text within the button
		 * \param x -> x position of the button
		 * \param y -> y position of the button
		 * \param w -> width of the button
		 * \param h -> height of the button
		 * \return Button object filled with all of the essential details for a customized button.
		 */
		BUTTONPTR createButton(std::string_view text, int x, int y, uint32_t w, uint32_t h);
		std::vector<BUTTONPTR> &getButtonList();
		SDL_Point &getMousePos();
		bool cursorInBounds(BUTTONPTR &button, SDL_Point &mousePos);
		bool cursorInBounds(SDL_Rect &area, SDL_Point &mousePos);
		void setButtonTextSize(IMD &buttonText, int w, int h);
		void setButtonTheme(BUTTONPTR &button, ColorData color);
		void setButtonPos(BUTTONPTR &button, int x, int y);
		void setButtonSize(BUTTONPTR &button, uint32_t w, uint32_t h);
		void setButtonTexture(BUTTONPTR &button, IMD &texture);
		void update(SDL_Event *ev, double dt);
		void drawDivider(const SDL_Rect &rect, const SDL_Color &col, SDL_Renderer *ren);
		void draw(BUTTONPTR &button, IMD buttonText, SDL_Renderer *ren, double sx = 0.0, double sy = 0.0);

	private:
		std::vector<BUTTONPTR> btnList {};
		SDL_Point mousePos {};
	};
} // namespace Application::Helper