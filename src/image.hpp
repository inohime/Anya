#pragma once

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include "animation.hpp"
#include "util.hpp"
#include <filesystem>
#include <string>
#include <memory>
#include <unordered_map>
#include <iostream>

namespace fs = std::filesystem;


/** Structure
 *
 * ImageData -> has the texture we want to actually operate on (SDL_Texture)
 * IMD -> ImageData Pointer to hold a series of ImageData objects
 * Image -> operates on ImageData (which contains an SDL_Texture and its related info)
 * Pack -> creates a texture atlas full of image objects and constructs them into a 1D array (or 2D, depending on requirements)
 */

namespace Application::Helper {
	struct MessageData final {
		MessageData &operator=(MessageData &) { return *this; }
		std::basic_string<char> msg {};
		std::basic_string<char> fontFile {};
		const SDL_Color &col {};
		int fontSize {0};
	};

	struct ImageData final {
		std::basic_string<char> path {};
		std::shared_ptr<SDL_Texture> texture {nullptr};
		int imageWidth {0};
		int imageHeight {0};
	};
	// handle
	using IMD = std::shared_ptr<ImageData>;

	class Image {
	public:
		// loads texture
		IMD create(std::string_view filePath, SDL_Renderer *ren, SDL_Color *key = nullptr);
		// loads render target
		IMD create(unsigned int width, unsigned int height, SDL_Renderer *ren);
		// how should we make Pack work? should it be like this?:
		// void create(std::string_view dirPath, SDL_Renderer *ren, int rows = 0, int cols = 0); 
		// example: ptr->create(path + "assets/gif-extract/", renderer.get());
		// if we have it this way, we can initialize Pack and create gifs, but how would we render it?
		// Pack has to return something for us to use.

		// loads text with outline (into a texture)
		IMD create(const MessageData &msg, SDL_Renderer *ren);
		// loads text (into a texture)
		IMD createA(const MessageData &msg, SDL_Renderer *ren);
		void add(std::string_view str, IMD &img);
		void remove(IMD &img);
		void draw(IMD &img, SDL_Renderer *ren, int x, int y, double sx = 0.0, double sy = 0.0, SDL_Rect *clip = nullptr) noexcept;
		void printImageCount();

	private:
		std::unordered_map<std::string_view, IMD> images {};

	private:
		friend class Animation;
	};

	// put this back inside of Image after done testing
	class Pack final {
	public:
		/** should work like this :
		 *
		 *  extracted gif images are placed sequentially on the texture atlas
		 *
		 *	-----------------------------
		 *	| 0 | 1 | 2 | 3 | 4 | 5 | 6 |
		 *	-----------------------------
		 *  1D array: images are on an x axis only (rows)
		 *
		 *	-----------------------------
		 *	| 0 | 1 | 2 | 3 | 4 | 5 | 6 |
		 *	-----------------------------
		 *	| 7 | 8 | 9 | 10| 11| 12| 13|
		 *	-----------------------------
		 *  2D array: images are on both x and y axis (rows and columns)
		 */


		 /** Packs the gif extraction into an atlas to be used as an animation
		  *
		  * \param dirPath -> the directory of the files, not the actual files!
		  * \param ren -> the renderer to use
		  * \param rows -> the number of rows the canvas should have if explicitly added
		  * \param cols -> the number of columns the canvas should have if explicitly added
		  * \return ImageData pointer or nullptr if failed
		  */
		IMD create(std::string_view dirPath, SDL_Renderer *ren, int rows = 0, int cols = 0) {
			imagePtr = std::make_unique<Image>();
			std::vector<std::basic_string<char>> pathList;
			IMD newImage = {nullptr};
			// get the directory path and append all of the files into the array
			for (const auto &pathIter : fs::directory_iterator(dirPath)) {
				auto pathString = pathIter.path().string();
				const auto findQuotesInPath = [&]() {
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

			// store a map containing the texture (loaded with the path) and an ID
			for (int i = 0; i < pathList.size(); ++i) {
				newImage = imagePtr->create(pathList[i], ren);
				SDL_QueryTexture(newImage->texture.get(), nullptr, nullptr, &newImage->imageWidth, &newImage->imageHeight);
				imageWidth = newImage->imageWidth;
				imageHeight = newImage->imageHeight;
				imageList.insert({i, newImage});
			}

			// our 1D/2D array is now prepped, now we need to align it on our atlas texture
			IMD canvas = {nullptr};
			// error somewhere after this point, const char * to _First
			
			// fill the canvas with our textures
			if ((rows && cols) != 0) {
				// change the canvas to fit [x] rows and [y (lol)] columns
				packRows = rows;
				packColumns = cols;
				unsigned int canvasWidth = imageWidth * static_cast<unsigned int>(imageList.size());
				unsigned int canvasHeight = imageHeight * static_cast<unsigned int>(imageList.size());

				canvas = imagePtr->create(imageWidth * static_cast<unsigned int>(pathList.size()), imageHeight * static_cast<unsigned int>(pathList.size()), ren);

				SDL_SetRenderTarget(ren, canvas->texture.get());
				for (int i = 0; i < imageList.size(); ++i) {
					for (unsigned int x = 0; x < canvasWidth / rows; ++x) {
						for (unsigned int y = 0; y < canvasHeight / cols; ++y) {
							// i will be the number of images we will be adding onto our canvas
							// based on the rows and columns, sort out how many images should fit on each line
							// probably a naive implementation, but optimize later.

							// unload our list onto the canvas
							imagePtr->draw(imageList[i], ren, imageWidth *= x, imageHeight *= y);
						}
					}
				}
				SDL_SetRenderTarget(ren, nullptr);
			} else {
				// expand the width to create a large-width based canvas
				canvas = imagePtr->create(imageWidth * static_cast<unsigned int>(pathList.size()), imageHeight, ren);
				// unload our list onto the canvas
				SDL_SetRenderTarget(ren, canvas->texture.get());
				for (int i = 0; i < imageList.size(); ++i) {
					// place them sequentially on the canvas
					imagePtr->draw(imageList[i], ren, imageWidth *= i, imageHeight);
				}
				SDL_SetRenderTarget(ren, nullptr);
			}
			// add canvas to Image container
			imagePtr->add("canvas", canvas);
			// fill width and height for later use (query canvas size later?)
			SDL_QueryTexture(canvas->texture.get(), nullptr, nullptr, &packWidth, &packHeight);

			return canvas;
		}

	private:
		//Image *imagePtr {nullptr}
		std::unique_ptr<Image> imagePtr {nullptr};
		// as long they have ID's, it doesn't matter since we will reorganize it
		// potentially change this to std::vector with std::lower_bound
		std::unordered_map<unsigned int, IMD> imageList {};
		int packWidth {0};
		int packHeight {0};
		int packRows {0};
		int packColumns {0};
	};
} // namespace Application::Helper