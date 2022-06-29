#pragma once

#include <SDL.h>
#include <memory>
#include <concepts>

namespace Application::Helper::Utilities {
	struct Memory final {
		void operator()(SDL_Window *x) const {SDL_DestroyWindow(x);}
		void operator()(SDL_Renderer *x) const {SDL_DestroyRenderer(x);}
		void operator()(SDL_Texture *x) const {SDL_DestroyTexture(x);}
	};

	template <typename T> using PTR = std::unique_ptr<T, Memory>;
	/*
	template <typename T> constexpr bool isUnsigned(T val) {
		return std::unsigned_integral<T>(val);
	}
	*/
} // namespace Application::Helper::Utilities