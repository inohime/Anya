#pragma once

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include "animation.hpp"
#include "data.hpp"
#include <filesystem>
#include <string>
#include <unordered_map>

namespace fs = std::filesystem;

/** Structure
 *
 * ImageData -> has the texture we want to actually operate on (SDL_Texture)
 * IMD -> ImageData Pointer to hold a series of ImageData objects
 * Image -> operates on ImageData (which contains an SDL_Texture and its related info)
 * Pack -> creates a texture atlas full of image objects and constructs them into a 1D array (or 2D, depending on requirements)
 */

namespace Application::Helper {
	class Image {
	public:
		// todo: add param/return 
		IMD createImage(std::string_view filePath, SDL_Renderer *ren, SDL_Color *key = nullptr);
		IMD createRenderTarget(unsigned int width, unsigned int height, SDL_Renderer *ren);
		IMD createText(const MessageData &msg, SDL_Renderer *ren);
		IMD createTextA(const MessageData &msg, SDL_Renderer *ren);
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
		  * \return ImageData pointer (canvas) or nullptr if failed.
		  */
		IMD createPack(std::string_view dirPath, SDL_Renderer *ren, int rows = 0, int cols = 0);
		std::shared_ptr<PackData> getPackData() noexcept;
		std::shared_ptr<Animation> getAnimPtr() noexcept;
		void add(std::string_view str, IMD &img);
		void remove(IMD &img);
		void draw(IMD &img, SDL_Renderer *ren, int x, int y, double sx = 0.0, double sy = 0.0, SDL_Rect *clip = nullptr) noexcept;
		void drawAnimation(IMD &img, SDL_Renderer *ren, int x, int y, double scale = 0) const noexcept;
		void printImageCount() const noexcept;

	private:
		std::unordered_map<std::string_view, IMD> images {};
		std::unordered_map<std::string_view, IMD> imagePackList {};
		std::shared_ptr<PackData> packPtr {std::make_shared<PackData>()};
		std::shared_ptr<Animation> animPtr {std::make_shared<Animation>()};
	};
} // namespace Application::Helper