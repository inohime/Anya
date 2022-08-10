#pragma once

#include <SDL.h>
#include <memory>
#include <iostream>

namespace Application::Helper::Utils {
	// SDL memory deleter
	struct Memory final {
		void operator()(SDL_Window *x) const { SDL_DestroyWindow(x); }
		void operator()(SDL_Renderer *x) const { SDL_DestroyRenderer(x); }
		void operator()(SDL_Texture *x) const { SDL_DestroyTexture(x); }
	};
	/** Memory deleter handle. Used to free SDL window, renderer, and/or textures without the overhead of a shared pointer.
	 * \param T -> the SDL type to manage
	 */
	template <class T> using PTR = std::unique_ptr<T, Memory>;
	/** Print lines to the console (works in constexpr functions).
	* \param msg -> the msg/variable to print
	*/
	template <class T> constexpr void println(T &&msg) {
		std::cout << std::forward<T>(msg) << '\n';
	}
	/** Print lines to the console (works in constexpr functions).
	* \param msg -> the message/variable to print
	* \param optMsg -> (optional) additional messages/variables to print
	*/
	template <class T, class... U> constexpr void println(T &&msg, U &&...optMsg) {
		std::cout << std::forward<T>(msg);
		((std::cout << ", " << std::forward<U>(optMsg)), ...);
	}
} // namespace Application::Helper::Utils