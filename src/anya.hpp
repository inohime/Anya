#pragma once

#include <SDL.h>
#include "image.hpp"
#include "uinterface.hpp"
#include "util.hpp"
#include "scene.hpp"
#include <chrono>
#include <format>
#include <sstream>

// low memory | low cpu utilization app (not the lowest since added features and no optimizations)
// at its current state, it takes 3.6mb on debug, too high. (11mb with gif alloc)

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
		static SDL_HitTestResult hitTestResult(SDL_Window *window, const SDL_Point *pt, void *data);
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
		std::chrono::steady_clock::time_point begin {};
		std::chrono::steady_clock::time_point end {};
		std::chrono::duration<double, std::milli> deltaTime {};
		int FPS {30};
		int delay {1000 / FPS};

	private:
		std::unique_ptr<Helper::UInterface> interfacePtr {nullptr};
		std::unique_ptr<Helper::Image> imagePtr {nullptr};
		std::unique_ptr<Helper::Scene> scenePtr {nullptr};
		std::basic_string<char> path {};
		std::basic_stringstream<char> str {};

		bool setBGIsPressed {false};
		bool setBGToColor {false};
		bool minimalMode {false};
		bool showDate {false};

		float sceneAlpha {SDL_ALPHA_TRANSPARENT};

		SDL_Rect settingsView {0, 0, (int)windowWidth, (int)windowHeight};
		SDL_Rect settingsThemesView {0, 0, (int)windowWidth, (int)windowHeight};
		SDL_Rect fillBGColor {0, 0, (int)windowWidth, (int)windowHeight};
		//SDL_Rect minimalDragRect;

		Helper::IMD backgroundGIF {nullptr};
		Helper::IMD backgroundImg {nullptr};
		Helper::IMD githubImg {nullptr};
		Helper::IMD calendarImg {nullptr};
		// text
		Helper::IMD timeText {nullptr};
		Helper::IMD dateText {nullptr};
		Helper::IMD settingsText {nullptr};
		Helper::IMD mainQuitText {nullptr};
		Helper::IMD minimizeText {nullptr};
		Helper::IMD quitText {nullptr};
		Helper::IMD settingsExitText {nullptr};
		Helper::IMD themesText {nullptr};
		Helper::IMD themesExitText {nullptr};
		// Helper::IMD setLayoutText {nullptr};
		Helper::IMD minimalText {nullptr};
		Helper::IMD setBGText {nullptr};
		Helper::IMD openFileText {nullptr};
		Helper::IMD setBGColorText {nullptr};
		Helper::IMD setTextFontText {nullptr};
		// test button theme changing
		/*
		Helper::IMD themesOCText {nullptr};
		Helper::IMD themesBGCText {nullptr};
		Helper::IMD themesTCText {nullptr};
		*/

		// test console rgb background
		std::basic_string<char> rgbVal {};
		int rVal, gVal, bVal;

		// buttons
		Helper::BUTTONPTR settingsBtn {nullptr};
		Helper::BUTTONPTR mainQuitBtn {nullptr};
		Helper::BUTTONPTR minimizeBtn {nullptr};
		Helper::BUTTONPTR settingsQuitBtn {nullptr};
		Helper::BUTTONPTR settingsExitBtn {nullptr};
		Helper::BUTTONPTR themesBtn {nullptr};
		Helper::BUTTONPTR githubBtn {nullptr};
		Helper::BUTTONPTR calendarBtn {nullptr};
		//Helper::BUTTONPTR setLayoutBtn {nullptr};
		Helper::BUTTONPTR themesExitBtn {nullptr};
		Helper::BUTTONPTR minimalBtn {nullptr};
		Helper::BUTTONPTR setBGBtn {nullptr};
		Helper::BUTTONPTR openFileBtn {nullptr};
		Helper::BUTTONPTR setBGColorBtn {nullptr};
		Helper::BUTTONPTR setTextFontBtn {nullptr};
		// test button theme changing
		/*
		Helper::BUTTONPTR themesOCBtn {nullptr};
		Helper::BUTTONPTR themesBGCBtn {nullptr};
		Helper::BUTTONPTR themesTCBtn {nullptr};
		*/
	};
} // namespace Application


