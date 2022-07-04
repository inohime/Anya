#include "uinterface.hpp"
#include <format>

namespace Application::Helper {
	/*
	BUTTONPTR UInterface::createButton(MessageData &msg, IMD texture, int x, int y, unsigned int w, unsigned int h) {
		BUTTONPTR newButton = std::make_shared<Button>();

		//SDL_assert(Utilities::isUnsigned(w) == true);

		newButton->box = {x, y, static_cast<int>(w), static_cast<int>(h)};
		if (texture != nullptr)
			newButton->texture = *texture;

		newButton->md = msg;
		btnList.emplace_back(newButton);

		return newButton;
	}
	*/

	BUTTONPTR UInterface::createButton(std::string_view text, IMD texture, ColorData col, std::string_view layerName, int x, int y, unsigned int w, unsigned int h) {
		BUTTONPTR newButton = std::make_shared<Button>();

		//SDL_assert(Utilities::isUnsigned(w) == true);

		newButton->box = {x, y, static_cast<int>(w), static_cast<int>(h)};

		newButton->text = text;
		if (texture != nullptr)
			newButton->texture = *texture;

		newButton->buttonColor.initialColor = col.initialColor;
		newButton->buttonColor.currentColor = col.currentColor;
		newButton->buttonColor.hoverColor = col.hoverColor;

		newButton->layer = layerName;

		newButton->canMinimize = false;
		newButton->canQuit = false;

		btnList.emplace_back(newButton);
		//btnList.insert({layerName, newButton});

		return newButton;
	}

	std::vector<BUTTONPTR> &UInterface::getButtonList() {
		/*
		std::vector<BUTTONPTR> currButtons;

		for (const auto &btn : btnList) {
			currButtons.push_back(btnList[btn.first]);
		}

		return currButtons;
		*/
		return btnList;
	}
	/*
	std::vector<BUTTONPTR> *UInterface::getButtonsOnLayer(std::string_view layer) {
		//find the buttons on the layer
		std::vector<BUTTONPTR> btnsOnLayer;
		auto iter = btnList.find(layer);
		if (iter != btnList.end()) {
			btnsOnLayer.push_back(btnList[layer]);
		} else {
			//std::cout << "Failed to find buttons on "
			std::format("Failed to find buttons on {}", layer);
			return nullptr;
		}

		return &btnsOnLayer;
	}
	*/

	SDL_Point &UInterface::getMousePos() {
		return mousePos;
	}

	void UInterface::setButtonTexture(BUTTONPTR &button, IMD &texture) {
		button->texture = *texture;
	}

	void UInterface::showOutline(BUTTONPTR &button, bool show) {
		button->showOutline = show;
	}

	bool UInterface::cursorInBounds(BUTTONPTR &button, SDL_Point &mousePos) {
		if (mousePos.x >= button->box.x && mousePos.x <= (button->box.x + button->box.w) &&
			mousePos.y >= button->box.y && mousePos.y <= (button->box.y + button->box.h)) {
			return true;
		}

		return false;
	}

	//bool UInterface::disableInputForLayer(BUTTONPTR &button) {
		//return false;
	//}

	void UInterface::setButtonPos(BUTTONPTR &button, int x, int y) {
		button->box.x = x;
		button->box.y = y;
	}

	void UInterface::setButtonSize(BUTTONPTR &button, unsigned int w, unsigned int h) {
		button->box.w = w;
		button->box.h = h;
	}

	void UInterface::update(SDL_Event *ev) {
		for (auto &button : getButtonList()) {
			switch (ev->type) {
				case SDL_MOUSEBUTTONDOWN: {
					if (button->isClickable)
						button->buttonColor.currentColor = {0, 0, 255};
				} break;

				case SDL_MOUSEBUTTONUP: {
					if (!button->isClickable) {
						button->buttonColor.currentColor = button->buttonColor.initialColor;
					} else {
						button->buttonColor.currentColor = button->buttonColor.hoverColor;
					}
				} break;

				case SDL_MOUSEMOTION: {
					mousePos.x = ev->motion.x;
					mousePos.y = ev->motion.y;
					// check if the cursor is hovering over the button
					if (cursorInBounds(button, mousePos)) {
						button->buttonColor.currentColor = button->buttonColor.hoverColor;
						button->isClickable = true;
					} else {
						button->buttonColor.currentColor = button->buttonColor.initialColor;
						button->isClickable = false;
					}
				} break;
			}
		}
	}

	void UInterface::draw(BUTTONPTR &button, IMD buttonText, SDL_Renderer *ren, double scaleX, double scaleY, SDL_Rect *clip) {
		SDL_Rect dst = {button->box.x, button->box.y, button->box.w, button->box.h};
		SDL_Rect textDst = {button->box.x, button->box.y, button->box.w, button->box.h}; // test, fix later

		if ((scaleX && scaleY) != 0) {
			dst.w *= static_cast<int>(scaleX);
			dst.h *= static_cast<int>(scaleY);
		}

		if (button->showOutline) {
			SDL_RenderCopy(ren, button->texture.texture.get(), nullptr, &dst);

			if (buttonText != nullptr) {
				SDL_RenderCopy(ren, buttonText->texture.get(), nullptr, &textDst);
			}

			SDL_SetRenderDrawColor(
				ren,
				button->buttonColor.currentColor.r,
				button->buttonColor.currentColor.g,
				button->buttonColor.currentColor.b,
				button->buttonColor.currentColor.a
			);
			SDL_RenderDrawRect(ren, &button->box);
		} else {
			SDL_RenderCopy(ren, button->texture.texture.get(), nullptr, &dst);

			if (buttonText != nullptr)
				SDL_RenderCopy(ren, buttonText->texture.get(), nullptr, &textDst);
		}
	}
} // namespace Application::Helper