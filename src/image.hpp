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

namespace fs = std::filesystem;

// structure
/**
 * ImageData -> has the texture we want to actually operate on (SDL_Texture)
 * IMD -> ImageData Pointer to hold a series of ImageData objects
 * Image -> operates on ImageData (which operates on SDL_Texture)
 * Pack -> creates a texture atlas full of image objects and constructs them into a 1D array (or 2D, depending on requirements)
*/

namespace Application::Helper {
	struct MessageData final {
		MessageData &operator=(MessageData &) {return *this;}
		std::basic_string<char> msg {};
		std::basic_string<char> fontFile {};
		const SDL_Color &col {};
		int fontSize {0};
	};

	struct ImageData final {
		std::basic_string<char> path {};
		std::shared_ptr<SDL_Texture> ptr {nullptr};
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
		// creates a 1D texture atlas unless rows & cols is specified
		// worry about Atlas Packer later
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
			 *	| 7 | 8 | 9 | 10 | 11 | 12  |
			 *	-----------------------------
			 *  2D array: images are on both x and y axis (rows and columns)
			 */

			 /// <summary>
			 /// Packs {x amount} of images of the same width and height into an atlas
			 /// gif extraction (individual frames) must be in a folder in /assets/ with nothing else in it.
			 /// </summary>
			 /// \param @rows: the number of rows the canvas should have if explicitly added
			 /// \param @cols: the number of columns the canvas should have if explicitly added
			Pack(std::string_view dirPath, SDL_Renderer *ren, int rows = 0, int cols = 0) {
				std::vector<std::string_view> pathList;
				IMD newImage = {nullptr};
				// we want the directory path, not an actual file
				for (const auto &pathIter : fs::directory_iterator(dirPath))
					pathList.emplace_back(pathIter.path().string()); // store the paths (files listed inside the cwd)

				// store a map containing the texture (loaded with the path) and an ID
				for (int i = 0; i < pathList.size(); ++i) {
					newImage = ptr->create(pathList[i], ren);
					SDL_QueryTexture(newImage->ptr.get(), nullptr, nullptr, &newImage->imageWidth, &newImage->imageHeight);
					imageList.insert({newImage, i});
				}
				// our 1D/2D array is now prepped, now we need to align it on our atlas texture
				IMD canvas = {nullptr};
				// fill the canvas with our textures
				if ((rows && cols) != 0) {
					// change the canvas to fit [x] rows and [y (lol)] columns
					packRows = rows;
					packColumns = cols;

					for (int x = 0; x < rows; ++x) {
						for (int y = 0; y < cols; ++y) {
							canvas = ptr->create(newImage->imageWidth * static_cast<unsigned int>(pathList.size()), newImage->imageHeight * static_cast<unsigned int>(pathList.size()), ren);
							// unload our list onto the canvas
						}
					}
				} else {
					// expand the width to create a large-width based canvas
					canvas = ptr->create(newImage->imageWidth * static_cast<unsigned int>(pathList.size()), newImage->imageHeight, ren);
					// unload our list onto the canvas
				}
				// add canvas to Image container
				ptr->add("canvas", canvas);
				// fill width and height for later use (query canvas size later?)
				SDL_QueryTexture(canvas->ptr.get(), nullptr, nullptr, &packWidth, &packHeight);
			}
			Pack(const Pack &) = delete; // copy
			Pack &operator=(const Pack &) noexcept = delete;
			Pack(Pack &&) = delete; // move
			Pack &operator=(Pack &&) noexcept = delete;

		private:
			Image *ptr {nullptr};
			// as long they have ID's, it doesn't matter since we will reorganize it
			// potentially change this to std::vector with std::lower_bound
			std::unordered_map<IMD, unsigned int> imageList {};
			//std::map<IMD, unsigned int> imageList {};
			//std::map<std::unique_ptr<SDL_Texture, Utilities::Memory>, uint32_t> imageList;
			int packWidth {0};
			int packHeight {0};
			int packRows {0};
			int packColumns {0};
		};
	};
} // namespace Application::Helper
