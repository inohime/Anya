#pragma once

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include "animation.hpp"
//#include "data.hpp"
#include <filesystem>
#include <string>
#include <unordered_map>

namespace fs = std::filesystem;

/** Structure
 *
 * ImageData -> has the texture we want to actually operate on (SDL_Texture)
 * IMD -> ImageData Smart Pointer
 * Image -> operates on ImageData (which contains an SDL_Texture and its related info)
 * Pack -> creates a texture atlas full of image objects and constructs them into a 1D array
 */

namespace Application::Helper {
	class Image {
	public:
		/** Create an image to be used for rendering. You can add an colour to be set transparent.
		 *
		 * \param filePath -> the location of the image file
		 * \param ren -> the renderer to use
		 * \param key -> the colour to be colour keyed
		 * \return the created image or nullptr if the operation failed.
		 */
		IMD createImage(std::string_view filePath, SDL_Renderer *ren, SDL_Color *key = nullptr);
		/** Create a render target to draw on top of.
		 *
		 * \param ren -> the renderer to use
		 * \param width -> the width of the image
		 * \param height -> the height of the image
		 * \return the image to be used as a render target or nullptr if the operation failed.
		 */
		IMD createRenderTarget(SDL_Renderer *ren, unsigned int width, unsigned int height);
		/** Create a text image.
		 *
		 * \param msg -> a struct constructed with:
		 * \param - msg -> the string of text
		 * \param - fontFile -> the font file for the text
		 * \param - col -> the colour of the text
		 * \param - fontSize -> the size of the text
		 * \param - outlineThickness -> the thickness of the text outline
		 * \param ren -> the renderer to use
		 * \return the text image or nullptr if the operation failed.
		 */
		IMD createText(const MessageData &msg, SDL_Renderer *ren);
		/** Create text with an outline.
		 *
		 * \param msg -> a struct constructed with:
		 * \param - msg -> the string of text
		 * \param - fontFile -> the font file for the text
		 * \param - col -> the colour of the text
		 * \param - fontSize -> the size of the text
		 * \param - outlineThickness -> the thickness of the text outline
		 * \param ren -> the renderer to use
		 * \return the text image with an outline or nullptr if the operation failed.
		 */
		IMD createTextA(const MessageData &msg, SDL_Renderer *ren);
		/** Create an Image Pack (texture atlas). 
		 *
		 *  extracted gif images are placed sequentially on the texture atlas
		 *
		 *	-----------------------------
		 *	| 0 | 1 | 2 | 3 | 4 | 5 | 6 |
		 *	-----------------------------
		 *  1D array: images are on an x axis only (rows)
		 */

		/** Packs the gif extraction into an atlas to be used as an animation
		 *
		 * \param packName -> the name of the image pack canvas that will be added to the map.
		 * \param dirPath -> the directory of the files, not the actual files!
		 * \param ren -> the renderer to use
		 * \return the image (canvas) or nullptr if the operation failed.
		 */
		IMD createPack(std::string_view packName, std::string_view dirPath, SDL_Renderer *ren);
		/** Gets the animation pointer for adding & drawing animations.
		 * 
		 * \return the pointer associated with the image animation.
		 */
		std::shared_ptr<Animation> getAnimPtr() noexcept;
		/** Gets the Image Pack width.
		 *
		 * \param packName -> the name of the image that was packed
		 * \return the width of the image pack or -1 if the image pack was not found.
		 */
		int getPackWidth(std::string_view packName) noexcept;
		/** Gets the Image Pack height. 
		 * 
		 * \param packName -> the name of the image that was packed
		 * \return the height of the image pack or -1 if the image pack was not found.
		 */
		int getPackHeight(std::string_view packName) noexcept;
		/** Emplace an image with a nametag into the map.
		 * 
		 * \param str -> the nametag of the image
		 * \param img -> the image to be inserted into the map
		 * \return 0 if the operation succeeded, otherwise -1 if it failed.
		 */
		int add(std::string_view str, IMD &img);
		/** Remove an image out of the map.
		 * 
		 * \param img -> the image to be removed from the map & safely freed.
		 * \return 0 if the operation succeeded, otherwise -1 if it failed.
		 */
		int remove(IMD &img);
		/** Renders an image to the screen.
		 * 
		 * \param img -> the image to draw
		 * \param ren -> the renderer to use
		 * \param x -> x position of the image
		 * \param y -> y position of the image
		 * \param scale -> scale up or down the image width and height (0 if default)
		 * \param clip -> the portion of the image to render (nullptr if default)
		 */
		void draw(IMD &img, SDL_Renderer *ren, int x, int y, double sx = 0.0, double sy = 0.0, SDL_Rect *clip = nullptr) noexcept;
		/** Renders an animation (or GIF from Image Pack) to the screen.
		 * 
		 * \param img -> the image (animation) to draw
		 * \param ren -> the renderer to use
		 * \param x -> x position of the image
		 * \param y -> y position of the image
		 * \param scale -> scale up or down the image width and height (0 if default)
		 */
		void drawAnimation(IMD &img, SDL_Renderer *ren, int x, int y, double scale = 0) const noexcept;
		/** Modifies the colour of the image.
		 * 
		 * \param img -> the image to modify
		 * \param col -> the colour to set the image to
		 */
		void setTextureColor(IMD &img, SDL_Color col);
		// prints the number of images in the map
		void printImageCount() const noexcept;

	private:
		std::unordered_map<std::basic_string<char>, IMD> images {};
		std::unordered_map<std::basic_string<char>, IMD> imagePackList {};
		std::shared_ptr<Animation> animPtr {std::make_shared<Animation>()};
	};
} // namespace Application::Helper