#include "image.hpp"
#include "util.hpp"
#include <filesystem>

using namespace Application::Helper::Utils;

namespace Application::Helper {
    SDL_Surface *loadFile(std::string_view filePath) {
        if (filePath.data() == nullptr) {
            panicln("Failed to load file");
            return nullptr;
        }

        return IMG_Load(filePath.data());
    }

    IMD Image::createImage(std::string_view filePath, SDL_Renderer *ren, SDL_Color *key) {
        IMD newImage = std::make_shared<ImageData>();
        newImage->path = filePath;

        auto iter = images.find(filePath.data());
        if (iter != images.end())
            return iter->second; // we found the filePath

        SDL_Surface *surf = loadFile(filePath);

        if (key != nullptr)
            SDL_SetColorKey(surf, SDL_TRUE, SDL_MapRGB(surf->format, key->r, key->g, key->b));

        newImage->texture = cheesecake(SDL_CreateTextureFromSurface(ren, surf));
        if (newImage->texture == nullptr) {
            panicln("Failed to create image");
            return nullptr;
        }
        images.insert({filePath.data(), newImage});

        SDL_FreeSurface(surf);

        return newImage;
    }

    IMD Image::createRenderTarget(SDL_Renderer *ren, unsigned int width, unsigned int height) {
        IMD newImage = std::make_shared<ImageData>();

        newImage->texture =
            cheesecake(SDL_CreateTexture(ren, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_TARGET, width, height));
        if (newImage->texture == nullptr) {
            panicln("Render Target failed to be created");
            return nullptr;
        }

        return newImage;
    }

    IMD Image::createText(const MessageData &msg, SDL_Renderer *ren) {
        IMD newImage = std::make_shared<ImageData>();
        newImage->path = msg.fontFile;

        TTF_Font *font = TTF_OpenFont(msg.fontFile.data(), msg.fontSize);
        if (font == nullptr) {
            panicln("TTF_OpenFont error");
            return nullptr;
        }

        SDL_Surface *surf = TTF_RenderText_Blended(font, msg.msg.data(), msg.col.textColor);
        if (surf == nullptr) {
            TTF_CloseFont(font);
            panicln("TTF_RenderText error");
            return nullptr;
        }

        newImage->texture = cheesecake(SDL_CreateTextureFromSurface(ren, surf));
        if (newImage->texture == nullptr) {
            panicln("Failed to create text image");
            return nullptr;
        }
        images.insert({msg.fontFile, newImage});

        SDL_FreeSurface(surf);
        TTF_CloseFont(font);

        return newImage;
    }

    IMD Image::createTextA(const MessageData &msg, SDL_Renderer *ren) {
        IMD newImage = std::make_shared<ImageData>();

        TTF_Font *font = TTF_OpenFont(msg.fontFile.data(), msg.fontSize);
        if (font == nullptr) {
            panicln("TTF_OpenFont error");
            return nullptr;
        }

        TTF_Font *outlineFont = TTF_OpenFont(msg.fontFile.data(), msg.fontSize);
        if (font == nullptr) {
            panicln("TTF_OpenFont error");
            return nullptr;
        }

        TTF_SetFontOutline(outlineFont, msg.outlineThickness);

        SDL_Surface *bgSurf = TTF_RenderText_Blended(font, msg.msg.data(), msg.col.textColor);
        SDL_Surface *fgSurf = TTF_RenderText_Blended(outlineFont, msg.msg.data(), {0x00, 0x00, 0x00});

        // destination rect that gets the size of the surface (explicit x/y for those that want to understand without
        // digging) 1 is the offset from the outline
        SDL_Rect position = {position.x = 1, position.y = 1, fgSurf->w, fgSurf->h};
        SDL_BlitSurface(bgSurf, nullptr, fgSurf, &position);

        newImage->texture = cheesecake(SDL_CreateTextureFromSurface(ren, fgSurf));
        if (newImage->texture == nullptr) {
            panicln("Failed to create outline text image");
            return nullptr;
        }
        // get the size of the surface
        if (TTF_SizeUTF8(font, msg.msg.data(), &newImage->imageWidth, &newImage->imageHeight) != 0) {
            panicln("Failed to retrieve text size");
            return nullptr;
        }

        images.insert({msg.fontFile, newImage});

        SDL_FreeSurface(bgSurf);
        SDL_FreeSurface(fgSurf);
        TTF_CloseFont(outlineFont);
        TTF_CloseFont(font);

        return newImage;
    }

    void Image::draw(IMD &img, SDL_Renderer *ren, int x, int y, double sx, double sy, SDL_Rect *clip) noexcept {
        SDL_Rect dst {img->imagePos1 = x, img->imagePos2 = y, 0, 0};
        if (clip != nullptr) {
            dst.w = clip->w;
            dst.h = clip->h;
        } else {
            SDL_QueryTexture(img->texture.get(), nullptr, nullptr, &dst.w, &dst.h);
        }

        if ((sx && sy) != 0) {
            dst.w *= static_cast<int>(sx);
            dst.h *= static_cast<int>(sy);
        }

        SDL_RenderCopy(ren, img->texture.get(), clip, &dst);
    }

    void Image::drawAnimation(IMD &img, SDL_Renderer *ren, int x, int y, double scale) const noexcept {
        animPtr->draw(img, ren, x, y, scale);
    }

    int Image::add(std::string_view str, IMD &img) {
        auto iter = images.find(str.data());
        if (iter != images.end()) {
            println("Image already exists");
            return -1;
        } else {
            images.insert({str.data(), img});
            if (images.find(str.data()) != images.end())
                println("Created image");
        }

        return 0;
    }

    int Image::remove(IMD &img) {
        if (images.contains(img->path)) {
            images.erase(img->path);
            img.reset();
        } else {
            println("Failed to remove image");
            return -1;
        }

        return 0;
    }

    void Image::setTextureColor(IMD &img, SDL_Color col) const noexcept {
        SDL_SetTextureColorMod(img->texture.get(), col.r, col.g, col.b);
        SDL_SetTextureAlphaMod(img->texture.get(), col.a);
    }

    IMD Image::createPack(std::string_view packName, std::string_view dirPath, SDL_Renderer *ren) {
        std::vector<std::basic_string<char>> pathList;
        // get the directory path and append all of the files into the array
        for (const auto &pathIter : std::filesystem::directory_iterator(dirPath)) {
            auto pathString = pathIter.path().string();
            const auto fixPathString = [&]() {
                std::for_each(std::begin({pathString}), std::end({pathString}), [&](std::string_view str) {
                    if (pathString.find(str) != std::basic_string<char>::npos)
                        pathString.erase(std::remove(pathString.begin(), pathString.end(), '"'), pathString.end());
                });
            };
            pathList.emplace_back(pathString);
        }

        int imageWidth = 0;
        int imageHeight = 0;

        // store a map containing the texture (loaded with the path)
        IMD newImage = {nullptr};
        for (const auto &path : pathList) {
            newImage = createImage(path, ren);
            SDL_QueryTexture(newImage->texture.get(), nullptr, nullptr, &newImage->imageWidth, &newImage->imageHeight);
            imageWidth = newImage->imageWidth;
            imageHeight = newImage->imageHeight;
            imagePackList.insert({path, newImage});
        }

        // our 1D array is now prepped, now we need to align it on our atlas texture
        IMD canvas = {nullptr};
        // expand the width to create a large-width based canvas
        canvas = createRenderTarget(ren, imageWidth * static_cast<unsigned int>(pathList.size()), imageHeight);

        SDL_SetRenderTarget(ren, canvas->texture.get());
        int iterWidth = 0; // the image iteration width (0, 148, 296, etc..)
        bool firstElement = true; // to place the first image at origin
        // unload our list onto the canvas
        for (const auto &i : pathList) {
            // place them sequentially on the canvas
            if (firstElement) {
                draw(imagePackList[i], ren, 0, 0);
            } else {
                draw(imagePackList[i], ren, iterWidth += imageWidth, 0);
            }
            firstElement = false;
        }
        SDL_SetRenderTarget(ren, nullptr);
        // fill width and height for querying
        SDL_QueryTexture(canvas->texture.get(), nullptr, nullptr, &canvas->imageWidth, &canvas->imageHeight);
        // add canvas to Image container
        add(packName, canvas);

        return canvas;
    }

    int Image::getPackWidth(std::string_view packName) noexcept {
        auto findPack = images.find(packName.data());
        if (findPack == images.end()) {
            println("Failed to get pack");
            return -1;
        }

        return images[packName.data()]->imageWidth;
    }

    int Image::getPackHeight(std::string_view packName) noexcept {
        auto findPack = images.find(packName.data());
        if (findPack == images.end()) {
            println("Failed to get pack");
            return -1;
        }

        return images[packName.data()]->imageHeight;
    }

    std::shared_ptr<Animation> Image::getAnimPtr() noexcept {
        return animPtr;
    }

    constexpr void Image::printImageCount() const noexcept {
        println("Image Size", images.size());
    }
} // namespace Application::Helper