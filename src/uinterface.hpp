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
		 * \return button object filled with all of the essential details for a customized button.
		 */
		BUTTONPTR createButton(std::string_view text, IMD texture, int x, int y, uint32_t w, uint32_t h);
		/** Create a normal button.
		 *
		 * \param text -> the text within the button
		 * \param x -> x position of the button
		 * \param y -> y position of the button
		 * \param w -> width of the button
		 * \param h -> height of the button
		 * \return button object filled with all of the essential details for a customized button.
		 */
		BUTTONPTR createButton(std::string_view text, int x, int y, uint32_t w, uint32_t h);
		/** Gets all of the buttons created
		 *
		 * \return An std::vector of all of the buttons in the application.
		 */
		std::vector<BUTTONPTR> &getButtonList();
		/** Gets the mouse's position in the application
		 *
		 * \return The position of the mouse.
		 */
		SDL_Point &getMousePos();
		/** Checks if the cursor is inside of a button
		 *
		 * \param button -> button to check
		 * \param mousePos -> cursor position
		 * \return true if the cursor is inside of the button, otherwise false.
		 */
		bool cursorInBounds(BUTTONPTR &button, SDL_Point &mousePos);
		/** Checks if the cursor is inside of a rectangle
		 *
		 * \param area -> the rectangle to check
		 * \param mousePos -> cursor position
		 * \return true if the cursor is inside of the rectangle, otherwise false.
		 */
		bool cursorInBounds(SDL_Rect &area, SDL_Point &mousePos);
		/** Changes the text size in a button
		 *
		 * \param buttonText -> the text to modify
		 * \param w -> new width of the text
		 * \param h -> new height of the text
		 */
		void setButtonTextSize(IMD &buttonText, int w, int h);
		/** Changes the button colours (theme)
		 *
		 * \param button ->
		 * \param color -> a struct that consists of:
		 * \param - outlineColor -> the outline colour of the button
		 * \param - bgColor -> the background colour of the button
		 * \param - textColor -> the text colour of the button
		 */
		void setButtonTheme(BUTTONPTR &button, ColorData color);
		/** Retrieves all of the buttons created
		 *
		 * \return An std::vector of all of the buttons in the application
		 */
		void setButtonPos(BUTTONPTR &button, int x, int y);
		/** Retrieves all of the buttons created
		 *
		 * \return An std::vector of all of the buttons in the application
		 */
		void setButtonSize(BUTTONPTR &button, uint32_t w, uint32_t h);
		/** Changes the texture of a button
		 *
		 * \param button -> the button to modify
		 * \param texture -> the new texture of the button
		 */
		void setButtonTexture(BUTTONPTR &button, IMD &texture);
		/** Updates the mouse position and 
		 *
		 * \param ev -> the event to poll
		 * \param dt -> the update of the app
		 */
		void update(SDL_Event *ev, double dt);
		void drawDivider(const SDL_Rect &rect, const SDL_Color &col, SDL_Renderer *ren);
		/** Renders a button to the screen
		 * 
		 * \param button -> the button to draw
		 * \param buttonText -> text to draw on the button (can be nullptr)
		 * \param ren -> the renderer to use
		 * \param sx -> scale the image's width up (0 by default)
		 * \param sy -> scale the image's height up (0 by default)
		 */
		void draw(BUTTONPTR &button, IMD buttonText, SDL_Renderer *ren, double sx = 0.0, double sy = 0.0);

	private:
		std::vector<BUTTONPTR> btnList {};
		SDL_Point mousePos {};
	};
} // namespace Application::Helper