#pragma once

#include <SDL.h>
#include <memory>

namespace Application::Helper::Utilities {
	struct Memory final {
		void operator()(SDL_Window *x) const {SDL_DestroyWindow(x);}
		void operator()(SDL_Renderer *x) const {SDL_DestroyRenderer(x);}
		void operator()(SDL_Texture *x) const {SDL_DestroyTexture(x);}
	};

	template <typename T> using PTR = std::unique_ptr<T, Memory>;
}
	/*
	template <class ... T> struct PTR { 
		PTR(T*...) {}  
		std::unique_ptr<T *> ret() {return std::make_unique<T *>(val);}
		T *val;
	};
	*/
	//template <typename T> std::unique_ptr<T, Memory> WPTR = {T, Memory};
	//sing PTR = decltype([] {return std::unique_ptr<T, Memory>});
	/*
	template <typename T> struct Video {};

	template <typename T> Video<T> WPTR(const T &ptr) {
		return Video<T>(ptr);
	}
	*/