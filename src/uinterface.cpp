#include "uinterface.hpp"
#include "util.hpp"

using namespace Application::Helper::Utils;

namespace Application::Helper {
    BUTTONPTR UInterface::createButton(std::string_view text, std::string_view layerName, IMD texture, int x, int y,
                                       uint32_t w, uint32_t h) {
        BUTTONPTR newButton = std::make_shared<Button>();

        newButton->box = {x, y, static_cast<int>(w), static_cast<int>(h)};
        newButton->text = text;

        if (texture != nullptr)
            newButton->texture = *texture;

        newButton->layer = layerName;

        btnList.emplace_back(newButton);

        return newButton;
    }

    BUTTONPTR UInterface::createButton(std::string_view text, std::string_view layerName, int x, int y, uint32_t w,
                                       uint32_t h) {
        BUTTONPTR newButton = std::make_shared<Button>();

        newButton->box = {x, y, static_cast<int>(w), static_cast<int>(h)};
        newButton->text = text;
        newButton->layer = layerName;

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

    bool UInterface::cursorInBounds(BUTTONPTR &button, SDL_Point &mousePos) {
        if (mousePos.x >= button->box.x && mousePos.x <= (button->box.x + button->box.w) &&
            mousePos.y >= button->box.y && mousePos.y <= (button->box.y + button->box.h)) {
            return true;
        }

        return false;
    }

    bool UInterface::cursorInBounds(SDL_Rect &area, SDL_Point &mousePos) {
        if (mousePos.x >= area.x && mousePos.x <= (area.x + area.w) && mousePos.y >= area.y &&
            mousePos.y <= (area.y + area.h)) {
            return true;
        }

        return false;
    }

    // make the text in the button independent
    void UInterface::setButtonTextSize(IMD &buttonText, int w, int h) {
        if (buttonText != nullptr) {
            buttonText->imageWidth = w;
            buttonText->imageHeight = h;
        }
    }

    void UInterface::setButtonTheme(BUTTONPTR &button, ColorData color) {
        button->buttonColor = color;
    }

    void UInterface::setButtonPos(BUTTONPTR &button, int x, int y) {
        button->box.x = x;
        button->box.y = y;
    }

    void UInterface::setButtonSize(BUTTONPTR &button, uint32_t w, uint32_t h) {
        button->box.w = w;
        button->box.h = h;
    }

    void UInterface::update(SDL_Event *ev, double dt) {
        switch (ev->type) {
            case SDL_MOUSEMOTION: {
                mousePos.x = ev->motion.x;
                mousePos.y = ev->motion.y;
            } break;
        }

        for (auto &button : getButtonList()) {
            if (cursorInBounds(button, getMousePos())) {
                button->colorAlpha += 0.35f * static_cast<float>(dt);
                if (button->colorAlpha >= SDL_ALPHA_OPAQUE)
                    button->colorAlpha = SDL_ALPHA_OPAQUE;
            } else {
                button->colorAlpha -= 0.35f * static_cast<float>(dt);
                if (button->colorAlpha <= 191.25f)
                    button->colorAlpha = 191.25f;
            }
        }
    }

    void UInterface::drawDivider(const SDL_Rect &rect, const SDL_Color &col, SDL_Renderer *ren) {
        SDL_SetRenderDrawColor(ren, col.r, col.g, col.b, col.a);
        SDL_RenderDrawRect(ren, &rect);
    }

    void UInterface::drawGradientEx(float x1, float y1, float x2, float y2, const SDL_Color &initial, const SDL_Color &end, SDL_Renderer *ren) {
        SDL_Vertex vert[4] = {
            {x1, y1, initial}, // top left
            {x2, y1, initial}, // top right
            {x1, y2, end}, // bottom left
            {x2, y2, end}, // bottom right
        };

        const int indices[] = {0, 1, 3, 0, 2, 3};

        SDL_RenderGeometry(ren, nullptr, vert, 4, indices, 6);
    }

    void UInterface::drawGradient(const SDL_FRect &rect, SDL_Color &initial, SDL_Color &end, SDL_Renderer *ren) {
        drawGradientEx(rect.x, rect.y, rect.w, rect.h, initial, end, ren);
    }

    void UInterface::draw(BUTTONPTR &button, IMD buttonText, SDL_Renderer *ren, double scaleX, double scaleY) {
        SDL_Rect dst = {button->box.x, button->box.y, button->box.w, button->box.h};
        SDL_Rect textDst = {};
        if (buttonText != nullptr) {
            textDst = {button->box.x - (buttonText->imageWidth / 2), button->box.y - (buttonText->imageHeight / 2),
                       button->box.w + buttonText->imageWidth,
                       button->box.h + buttonText->imageHeight}; // modify the text dimensions here
        }

        if ((scaleX && scaleY) != 0) {
            dst.w *= static_cast<int>(scaleX);
            dst.h *= static_cast<int>(scaleY);
        }

        // button background colour
        SDL_SetRenderDrawColor(ren, button->buttonColor.bgColor.r, button->buttonColor.bgColor.g,
                               button->buttonColor.bgColor.b, (uint8_t)button->colorAlpha);
        SDL_RenderFillRect(ren, &dst);

        SDL_Rect innerOutline = {button->box.x - 1, button->box.y - 1, button->box.w + 2, button->box.h + 2};
        SDL_SetRenderDrawColor(ren, button->buttonColor.outlineColor.r, button->buttonColor.outlineColor.g,
                               button->buttonColor.outlineColor.b, (uint8_t)button->colorAlpha);
        SDL_RenderDrawRect(ren, &innerOutline);

        SDL_Rect outerOutline = {button->box.x - 2, button->box.y - 2, button->box.w + 4, button->box.h + 4};
        SDL_SetRenderDrawColor(ren, button->buttonColor.outlineColor.r, button->buttonColor.outlineColor.g,
                               button->buttonColor.outlineColor.b, (uint8_t)button->colorAlpha);
        SDL_RenderDrawRect(ren, &outerOutline);

        SDL_RenderCopy(ren, button->texture.texture.get(), nullptr, &dst);

        if (buttonText != nullptr) {
            SDL_RenderCopy(ren, buttonText->texture.get(), nullptr, &textDst);
        }
    }
} // namespace Application::Helper