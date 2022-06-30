#include "image.hpp"
#include <iostream>

namespace Application::Helper {
	SDL_Surface *loadFile(std::string_view filePath) {
		if (filePath.data() == nullptr) {
			std::cout << "Failed to load file " << SDL_GetError() << '\n';
			return nullptr;
		}
		return IMG_Load(filePath.data());
	}

	// load textures
	IMD Image::createImage(std::string_view filePath, SDL_Renderer *ren, SDL_Color *key) {
		IMD newImage = std::make_shared<ImageData>();
		newImage->path = filePath;

		auto iter = images.find(filePath);
		if (iter != images.end())
			return iter->second; // we found the filePath

		// start texture creation process
		SDL_Surface *surf = loadFile(filePath);

		if (key != nullptr)
			SDL_SetColorKey(surf, SDL_TRUE, SDL_MapRGB(surf->format, key->r, key->g, key->b));

		newImage->texture = std::shared_ptr<SDL_Texture>(SDL_CreateTextureFromSurface(ren, surf), SDL_DestroyTexture);
		if (newImage->texture == nullptr) {
			std::cout << "Text texture failed to be created\n";
			return nullptr;
		}
		images.insert({filePath, newImage});

		SDL_FreeSurface(surf);

		return newImage;
	}

	// load render target
	IMD Image::createRenderTarget(uint32_t width, uint32_t height, SDL_Renderer *ren) {
		IMD newImage = std::make_shared<ImageData>();

		newImage->texture = std::shared_ptr<SDL_Texture>(SDL_CreateTexture(ren, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_TARGET, width, height), SDL_DestroyTexture);
		if (newImage->texture == nullptr) {
			std::cout << "Texture failed to be created\n";
			return nullptr;
		}
		//images.insert({targetName, newImage});
		// use add instead (wherever)

		return newImage;
	}

	// load text
	IMD Image::createText(const MessageData &msg, SDL_Renderer *ren) {
		IMD newImage = std::make_shared<ImageData>();
		newImage->path = msg.fontFile;

		TTF_Font *font = TTF_OpenFont(msg.fontFile.data(), msg.fontSize);
		if (font == nullptr) {
			std::cout << "TTF_OpenFont error " << TTF_GetError() << "\n";
			return nullptr;
		}

		SDL_Surface *surf = TTF_RenderText_Blended(font, msg.msg.data(), msg.col);
		if (surf == nullptr) {
			TTF_CloseFont(font);
			std::cout << "TTF_RenderText error " << TTF_GetError() << "\n";
			return nullptr;
		}

		newImage->texture = std::shared_ptr<SDL_Texture>(SDL_CreateTextureFromSurface(ren, surf), SDL_DestroyTexture);
		if (newImage->texture == nullptr) {
			std::cout << "Text texture failed to be created\n";
			return nullptr;
		}
		images.insert({msg.fontFile, newImage});

		SDL_FreeSurface(surf);
		TTF_CloseFont(font);

		return newImage;
	}

	// load text with outline
	IMD Image::createTextA(const MessageData &msg, SDL_Renderer *ren) {
		IMD newImage = std::make_shared<ImageData>();

		TTF_Font *font = TTF_OpenFont(msg.fontFile.data(), msg.fontSize);
		if (font == nullptr) {
			std::cout << "TTF_OpenFont error " << TTF_GetError() << "\n";
			return nullptr;
		}

		TTF_Font *ofont = TTF_OpenFont(msg.fontFile.data(), msg.fontSize);
		if (font == nullptr) {
			std::cout << "TTF_OpenFont error " << TTF_GetError() << "\n";
			return nullptr;
		}

		TTF_SetFontOutline(ofont, 1);

		SDL_Surface *bg_surf = TTF_RenderText_Blended(font, msg.msg.data(), msg.col);
		SDL_Surface *fg_surf = TTF_RenderText_Blended(ofont, msg.msg.data(), {0x00, 0x00, 0x00});

		// destination rect that gets the size of the surface (explicit x/y for those that want to understand without digging)
		SDL_Rect position = {position.x = 1, position.y = 1, fg_surf->w, fg_surf->h};
		SDL_BlitSurface(bg_surf, nullptr, fg_surf, &position);

		newImage->texture = std::shared_ptr<SDL_Texture>(SDL_CreateTextureFromSurface(ren, fg_surf), SDL_DestroyTexture);
		if (newImage->texture == nullptr) {
			std::cout << "Text texture failed to be created\n";
			return nullptr;
		}
		images.insert({msg.fontFile, newImage});

		SDL_FreeSurface(bg_surf);
		SDL_FreeSurface(fg_surf);
		TTF_CloseFont(ofont);
		TTF_CloseFont(font);

		return newImage;
	}

	void Image::draw(IMD &img, SDL_Renderer *ren, int x, int y, double sx, double sy, SDL_Rect *clip) noexcept {
		SDL_Rect dst {};
		dst.x = x;
		dst.y = y;
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

	void Image::add(std::string_view str, IMD &img) {
		auto iter = images.find(str);
		if (iter != images.end()) {
			std::cout << "image already exists\n";
			return; // canvas exists
		} else {
			images.insert({str, img});
			if (images.find(str) != images.end())
				std::cout << "created image\n";
		}
	}

	void Image::remove(IMD &img) {
		if (images.contains(img->path)) {
			images.erase(img->path);
			img.reset();
		}
	}

	void Image::printImageCount() {
		for (const auto &i : images) {
			std::cout << "Image Size: " << images.size() << '\n';
		}
	}

	IMD Image::createPack(std::string_view dirPath, SDL_Renderer *ren, int rows, int cols) {
		std::vector<std::basic_string<char>> pathList;
		// get the directory path and append all of the files into the array
		for (const auto &pathIter : fs::directory_iterator(dirPath)) {
			auto pathString = pathIter.path().string();
			const auto fixPathString = [&]() {
				std::for_each(std::begin({pathString}), std::end({pathString}), [&](std::string_view str) {
					if (pathString.find(str) != std::basic_string<char>::npos)
						pathString.erase(std::remove(pathString.begin(), pathString.end(), '"'), pathString.end());
				});
			};
			pathList.emplace_back(pathString);
		}

		for (const auto &i : pathList)
			std::cout << i << '\n';

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

		// our 1D/2D array is now prepped, now we need to align it on our atlas texture
		PackData pack = {0};
		IMD canvas = {nullptr};
		// fill the canvas with our textures
		if ((rows && cols) != 0) {
			// change the canvas to fit [x] rows and [y (lol)] columns
			pack.rows = rows;
			pack.columns = cols;
			unsigned int canvasWidth = imageWidth * static_cast<unsigned int>(imagePackList.size());
			unsigned int canvasHeight = imageHeight * static_cast<unsigned int>(imagePackList.size());

			canvas = createRenderTarget(imageWidth * static_cast<unsigned int>(pathList.size()), imageHeight * static_cast<unsigned int>(pathList.size()), ren);

			SDL_SetRenderTarget(ren, canvas->texture.get());
			for (const auto &i : pathList) {
				for (unsigned int x = 0; x < canvasWidth / rows; ++x) {
					for (unsigned int y = 0; y < canvasHeight / cols; ++y) {
						// [i] will be the number of images we will be adding onto our canvas
						// based on the rows and columns, sort out how many images should fit on each line
						// probably a naive implementation, but optimize later.

						// unload our list onto the canvas
						draw(imagePackList[i], ren, imageWidth *= x, imageHeight *= y); // fix this later, not that important at the moment
					}
				}
			}
			SDL_SetRenderTarget(ren, nullptr);
		} else {
			// expand the width to create a large-width based canvas
			canvas = createRenderTarget(imageWidth * static_cast<unsigned int>(pathList.size()), imageHeight, ren);
			// unload our list onto the canvas
			SDL_SetRenderTarget(ren, canvas->texture.get());
			int iterWidth = 0; // the image iteration width (0, 148, 296, etc..)
			bool firstElement = true; // to place the first image at origin
			for (const auto &i : pathList) {
				// place them sequentially on the canvas
				if (firstElement)
					draw(imagePackList[i], ren, 0, 0);
				else
					draw(imagePackList[i], ren, iterWidth += imageWidth, 0);

				firstElement = false;
			}
			SDL_SetRenderTarget(ren, nullptr);
		}
		// add canvas to Image container
		add("canvas", canvas);
		// fill width and height for later use (query canvas size later?)
		SDL_QueryTexture(canvas->texture.get(), nullptr, nullptr, &pack.width, &pack.height);

		return canvas;
	}

	std::weak_ptr<PackData> Image::getPackData() {
		return packPtr;
	}
} // namespace Application::Helper