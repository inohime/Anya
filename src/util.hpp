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

	constexpr bool isUnsigned(std::unsigned_integral auto val) noexcept {
		return (val) ? true : false;
	}

	constexpr bool isUnsigned(std::unsigned_integral auto val1, std::unsigned_integral auto val2) noexcept {
		return ((val1) && (val2)) ? true : false;
	}
} // namespace Application::Helper::Utilities