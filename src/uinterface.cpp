#include "uinterface.hpp"

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

	BUTTONPTR UInterface::createButton(std::string_view text, IMD &texture, SDL_Color col, int x, int y, unsigned int w, unsigned int h) {
		BUTTONPTR newButton = std::make_shared<Button>();

		//SDL_assert(Utilities::isUnsigned(w) == true);

		newButton->box = {x, y, static_cast<int>(w), static_cast<int>(h)};

		newButton->text = text;
		if (texture != nullptr)
			newButton->texture = *texture;

		newButton->color = col;
		newButton->initializedColor = col;

		btnList.emplace_back(newButton);

		return newButton;
	}

	void UInterface::setButtonTexture(BUTTONPTR &button, IMD &texture) {
		button->texture = *texture;
	}

	void UInterface::showOutline(BUTTONPTR &button, bool show) {
		button->boxOutline = show;
	}

	void UInterface::update(SDL_Event *ev) {
		SDL_Point mousePos = {};
		for (const auto &button : btnList) {
			switch (ev->type) {
				case SDL_MOUSEBUTTONDOWN: {
					if (button->isClickable)
						button->color = {0, 0, 255, 255}; // test
				} break;

				case SDL_MOUSEBUTTONUP: {
					if (!button->isClickable)
						button->color = button->initializedColor;
				} break;

				case SDL_MOUSEMOTION: {
					mousePos.x = ev->motion.x;
					mousePos.y = ev->motion.y;
					if (mousePos.x >= button->box.x && mousePos.x <= (button->box.x + button->box.w) &&
						mousePos.y >= button->box.y && mousePos.y <= (button->box.y + button->box.h)) {

						button->isClickable = true;
						button->color = {0, 255, 0, 255};
					} else {
						button->color = button->initializedColor;
						button->isClickable = false;
					}
				} break;
			}
		}
	}

	void UInterface::draw(BUTTONPTR &button, IMD &text, SDL_Renderer *ren, double scaleX, double scaleY, SDL_Rect *clip) {
		SDL_Rect dst = {button->box.x, button->box.y, button->box.w, button->box.h};
		SDL_Rect textDst = {0, 0, button->box.w, button->box.h}; // test, fix later

		if ((scaleX && scaleY) != 0) {
			dst.w *= static_cast<int>(scaleX);
			dst.h *= static_cast<int>(scaleY);
		}

		if (button->boxOutline) {
			SDL_RenderCopy(ren, button->texture.texture.get(), nullptr, &dst);
			SDL_RenderCopy(ren, text->texture.get(), nullptr, &textDst);
			SDL_SetRenderDrawColor(ren, button->color.r, button->color.g, button->color.b, button->color.a);
			SDL_RenderDrawRect(ren, &button->box);
		} else {
			SDL_RenderCopy(ren, button->texture.texture.get(), nullptr, &dst);
			SDL_RenderCopy(ren, text->texture.get(), nullptr, &textDst);
		}
	}
} // namespace Application::Helper