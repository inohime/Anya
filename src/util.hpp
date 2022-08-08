#pragma once

#include <SDL.h>
#include <memory>
#include <iostream>

namespace Application::Helper::Utils {
	struct Memory final {
		void operator()(SDL_Window *x) const { SDL_DestroyWindow(x); }
		void operator()(SDL_Renderer *x) const { SDL_DestroyRenderer(x); }
		void operator()(SDL_Texture *x) const { SDL_DestroyTexture(x); }
	};

	template <class T> using PTR = std::unique_ptr<T, Memory>;

	template <class T> constexpr void println(T &&var) {
		std::cout << std::forward<T>(var) << '\n';
	}

	template <class T, class... U> constexpr void println(T &&var, U &&...args) {
		std::cout << std::forward<T>(var);
		((std::cout << ", " << std::forward<U>(args)), ...);
	}
} // namespace Application::Helper::Utilities