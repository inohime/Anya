#include "uinterface.hpp"
#include <format>

namespace Application::Helper {
	
	BUTTONPTR UInterface::createButton(std::string_view text, IMD texture, ColorData col, std::string_view layerName, int x, int y, unsigned int w, unsigned int h) {
		BUTTONPTR newButton = std::make_shared<Button>();

		//SDL_assert(Utilities::isUnsigned(w) == true);

		newButton->box = {x, y, static_cast<int>(w), static_cast<int>(h)};

		newButton->text = text;
		if (texture != nullptr)
			newButton->texture = *texture;

		//newButton->buttonColor.initialColor = col.initialColor;
		//newButton->buttonColor.currentColor = col.currentColor;
		//newButton->buttonColor.hoverColor = col.hoverColor;

		newButton->layer = layerName;

		btnList.emplace_back(newButton);

		return newButton;
	}
	

	BUTTONPTR UInterface::createButton(std::string_view text, int x, int y, uint32_t w, uint32_t h) {
		BUTTONPTR newButton = std::make_shared<Button>();

		//SDL_assert(Utilities::isUnsigned(w) == true);

		newButton->box = {x, y, static_cast<int>(w), static_cast<int>(h)};
		newButton->text = text;

		btnList.emplace_back(newButton);

		return newButton;
	}

	std::vector<BUTTONPTR> &UInterface::getButtonList() {
		return btnList;
	}

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

	// make the text in the button independent
	void UInterface::setButtonTextSize(BUTTONPTR &button, uint32_t w, uint32_t h) {
	}

	void UInterface::setButtonTheme(BUTTONPTR &button, ColorData color) {
		button->buttonColor = color;
	}

	void UInterface::setButtonPos(BUTTONPTR &button, int x, int y) {
		button->box.x = x;
		button->box.y = y;
	}

	void UInterface::setButtonSize(BUTTONPTR &button, unsigned int w, unsigned int h) {
		button->box.w = w;
		button->box.h = h;
	}

	void UInterface::update(SDL_Event *ev, double dt) {
		for (auto &button : getButtonList()) {
			switch (ev->type) {
				case SDL_MOUSEMOTION: {
					mousePos.x = ev->motion.x;
					mousePos.y = ev->motion.y;
					// check if the cursor is hovering over the button
					if (cursorInBounds(button, mousePos)) {
						button->isClickable = true;
					} else {
						button->isClickable = false;
					}
				} break;
			}
		}

		for (auto &button : getButtonList()) {
			if (cursorInBounds(button, getMousePos())) {
				button->colorAlpha += 0.35f * dt;
				if (button->colorAlpha >= SDL_ALPHA_OPAQUE)
					button->colorAlpha = SDL_ALPHA_OPAQUE;
			} else {
				button->colorAlpha -= 0.35f * dt;
				if (button->colorAlpha <= 191.25f)
					button->colorAlpha = 191.25f;
			}
		}
	}

	void UInterface::draw(BUTTONPTR &button, IMD buttonText, SDL_Renderer *ren, double scaleX, double scaleY, SDL_Rect *clip) {
		SDL_Rect dst = {button->box.x, button->box.y, button->box.w, button->box.h};
		SDL_Rect textDst = {button->box.x, button->box.y, button->box.w, button->box.h};

		if ((scaleX && scaleY) != 0) {
			dst.w *= static_cast<int>(scaleX);
			dst.h *= static_cast<int>(scaleY);
		}

		// get the button colour and the size of the button
		// draw the button with styles
		
		// test style (button position should be set to button pos
		//SDL_Rect bg = {5, 5, 25, 25}; // x, y -> 50 (last position)
		SDL_SetRenderDrawColor(ren, button->buttonColor.bgColor.r, button->buttonColor.bgColor.g, button->buttonColor.bgColor.b, button->colorAlpha);
		SDL_RenderFillRect(ren, &dst);

		// outline
		SDL_Rect innerOutline = {button->box.x - 1, button->box.y - 1, button->box.w + 2, button->box.h + 2};
		SDL_SetRenderDrawColor(ren, button->buttonColor.outlineColor.r, button->buttonColor.outlineColor.g, button->buttonColor.outlineColor.b, button->colorAlpha);
		SDL_RenderDrawRect(ren, &innerOutline);

		SDL_Rect outerOutline = {button->box.x - 2, button->box.y - 2, button->box.w + 4, button->box.h + 4};
		SDL_SetRenderDrawColor(ren, button->buttonColor.outlineColor.r, button->buttonColor.outlineColor.g, button->buttonColor.outlineColor.b, button->colorAlpha);
		SDL_RenderDrawRect(ren, &outerOutline);

		SDL_RenderCopy(ren, button->texture.texture.get(), nullptr, &dst);

		if (buttonText != nullptr) {
			SDL_RenderCopy(ren, buttonText->texture.get(), nullptr, &textDst);
		}
	}
} // namespace Application::Helper