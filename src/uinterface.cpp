#include "uinterface.hpp"

namespace Application::Helper {
	Button UInterface::createButton(MessageData &msg, IMD texture, int x, int y, unsigned int w, unsigned int h) {
		Button newButton = {};

		//SDL_assert(Utilities::isUnsigned(w) == true);

		newButton.box = {x, y, static_cast<int>(w), static_cast<int>(h)};
		if (texture != nullptr)
			newButton.texture = texture;

		newButton.md = msg;
		btnList.emplace_back(newButton);

		return newButton;
	}

	Button UInterface::createButton(std::string_view text, IMD texture, int x, int y, unsigned int w, unsigned int h) {
		Button newButton = {};

		//SDL_assert(Utilities::isUnsigned(w) == true);

		newButton.box = {x, y, static_cast<int>(w), static_cast<int>(h)};
		if (texture != nullptr)
			newButton.texture = texture;

		newButton.md.msg = text;
		btnList.emplace_back(newButton);

		return newButton;
	}

	void UInterface::setButtonTexture(Button &button, IMD &texture) {
		button.texture = texture;
	}

	void UInterface::showOutline(Button &button) {
		button.boxOutline = true;
	}

	void UInterface::update(double dt) {
		// update all buttons 
		// check to see if there is a mouse hovering or clicking
	}

	void UInterface::draw(Button &button, SDL_Renderer *ren, int x, int y, double sx, double sy, SDL_Rect *clip) {
	}
} // namespace Application::Helper