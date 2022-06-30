#pragma once

#include <SDL.h>
#include "image.hpp"
#include "uinterface.hpp"
#include "util.hpp"
#include <chrono>
#include <format>
#include <sstream>

// low memory | low cpu utilization app (not the lowest since added features and no optimizations)
// at its current state, it takes 3.6mb on debug, too high. (19mb with gif alloc)

namespace Application {
	using namespace Helper::Utilities;

	class Anya final {
	public:
		Anya();
#ifdef _DEBUG
		Anya(const std::chrono::system_clock::time_point &time);
#endif

		std::basic_string<char> timeToStr(const std::chrono::system_clock::time_point &time);
		std::unique_ptr<std::basic_stringstream<char>> getStream();
		bool boot();
		void update();
		void draw();
		void free();

	private:
		// window data
		std::basic_string<char> title {"anya"};
		std::basic_string<char> errStr {};
		PTR<SDL_Window> window {nullptr};
		PTR<SDL_Renderer> renderer {nullptr};
		SDL_Event ev {};
		bool shouldRun {false};
		uint32_t windowWidth {148};
		uint32_t windowHeight {89};
		// start: for debug
		//uint32_t windowWidth {1920};
		//uint32_t windowHeight {1080};
		// end
		std::chrono::steady_clock::time_point begin {};
		std::chrono::steady_clock::time_point end {};
		std::chrono::duration<double, std::milli> deltaTime {};
		int FPS {8};
		int delay {1000 / FPS};

	private:
		std::unique_ptr<Helper::UInterface> interfacePtr {nullptr};
		std::unique_ptr<Helper::Image> imagePtr {nullptr};
		std::basic_string<char> path {};
		std::basic_stringstream<char> str {};
		Helper::IMD backgroundGIF {nullptr};
		Helper::IMD background {nullptr};
		Helper::IMD msg {nullptr};
	};
} // namespace Application


