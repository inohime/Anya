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
	/** Memory handler. Used to manage an SDL window, renderer, and/or textures without the overhead of a shared pointer.
	 * 
	 * \param T -> SDL type to manage
	 */
	template <class T> using SMD = std::unique_ptr<T, Memory>;
	/** Memory handler function. Used to create & free an SDL window, renderer, and/or textures.
	 * 
	 * \param type -> SDL type to manage
	 * \return the type in a managed memory wrapper.
	 */
	template <class T> inline constexpr SMD<T> cheesecake(T *type) {
		return SMD<T>(type);
	}
	/** Print lines to the console (works in constexpr functions).
	 * 
	 * \param msg -> message/variable to print
	 */
	template <class T> inline constexpr void println(T &&msg) noexcept {
		std::cout << std::forward<T>(msg) << '\n';
	}
	/** Print multiple lines to the console (works in constexpr functions).
	 * 
	 * \param msg -> message/variable to print
	 * \param optMsg -> (optional) additional messages/variables to print
	 */
	template <class T, class... U> inline constexpr void println(T &&msg, U &&...optMsg) noexcept {
		std::cout << std::forward<T>(msg);
		((std::cout << ", " << std::forward<U>(optMsg)), ...);
		std::cout << '\n';
	}
	/** Print lines and SDL_GetError to the console (works in constexpr functions).
	 * 
	 * \param errMsg -> error message to print
	 */
	template <class T> inline constexpr void panicln(T &&errMsg) noexcept {
		std::cout << std::forward<T>(errMsg) << ": " << SDL_GetError() << '\n';
	}
} // namespace Application::Helper::Utils