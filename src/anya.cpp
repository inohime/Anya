#include "anya.hpp"
#include <iostream>

namespace Application {
	Anya::Anya() {
		if (!boot()) {
			SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, title.c_str(), errStr.c_str(), window.get());
		} else {
			update();
		}
	}

#ifdef _DEBUG
	Anya::Anya(const std::chrono::system_clock::time_point &time) {
		str << timeToStr(time);
	}
#endif

	bool Anya::boot() {
		SDL_assert(SDL_Init(SDL_INIT_EVERYTHING) == 0);
		SDL_assert(IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG) != 0);
		if (TTF_Init() == -1) return false;

		begin = std::chrono::steady_clock::now();

		window = PTR<SDL_Window>(SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, windowWidth, windowHeight, 0));
		renderer = PTR<SDL_Renderer>(SDL_CreateRenderer(window.get(), -1, SDL_RENDERER_SOFTWARE));
		if (!window || !renderer) {
			errStr = SDL_GetError();
			std::cout << "failed to boot: " << errStr << '\n';
			free();
			return shouldRun;
		}

		char *const base = SDL_GetBasePath();
		path = base;
		SDL_free(base);

		// initialize image
		imagePtr = std::make_unique<Helper::Image>();
		interfacePtr = std::make_unique<Helper::UInterface>();
		scenePtr = std::make_unique<Helper::Scene>();

		// load assets
		background = imagePtr->createImage(path + "assets/beep_1.png", renderer.get());
		settingsIcon = imagePtr->createImage(path + "assets/setting.png", renderer.get());
		backgroundGIF = imagePtr->createPack("canvas", path + "assets/gif-extract/", renderer.get());
		imagePtr->getAnimPtr()->addAnimation(68, 0, 0, 148, 89);

		// replace ColorData with ColorState
		// main
		settingsBtn = interfacePtr->createButton("", settingsIcon, {{255, 0, 0}, {0, 255, 0}}, 0, 50, 25, 25);
		settingsBtn->showOutline = true;
		//settingsBtn->isEnabled = true;

		minimizeBtn = interfacePtr->createButton("-", nullptr, {{255, 0, 0}, {0, 255, 0}}, 80, 0, 25, 25);
		minimizeBtn->showOutline = true;
		minimizeBtn->canMinimize = true;

		quitBtn = interfacePtr->createButton("Quit", nullptr, {{255, 0, 0}, {0, 255, 0}}, 120, 25, 25, 25);
		quitBtn->showOutline = true;
		quitBtn->canQuit = true;

		// settings
		settingsExitBtn = interfacePtr->createButton("x", nullptr, {{255, 0, 0}, {0, 255, 0}}, 0, 0, 25, 25);
		settingsExitBtn->showOutline = true;

		themesBtn = interfacePtr->createButton("Themes", nullptr, {{255, 0, 0}, {0, 255, 0}}, 100, 0, 50, 25);
		themesBtn->showOutline = true;

		// settings-themes
		minimalBtn = interfacePtr->createButton("Minimal Mode", nullptr, {{255, 0, 0}, {0, 255, 0}}, 0, 10, 50, 25);
		minimalBtn->showOutline = true;

		setBGBtn = interfacePtr->createButton("Set Background", nullptr, {{255, 0, 0}, {0, 255, 0}}, 0, 20, 75, 25);
		setBGBtn->showOutline = true;

		openFileBtn = interfacePtr->createButton("Open File", nullptr, {{255, 0, 0}, {0, 255, 0}}, 0, 0, 30, 25);
		openFileBtn->showOutline = true;

		setBGColorBtn = interfacePtr->createButton("Set Color", nullptr, {{255, 0, 0}, {0, 255, 0}}, 0, 40, 50, 25);
		setBGColorBtn->showOutline = true;

		setTextFontBtn = interfacePtr->createButton("Set Time Font", nullptr, {{255, 0, 0}, {0, 255, 0}}, 0, 50, 50, 25);
		setTextFontBtn->showOutline = true;

		// create scenes
		scenePtr->createScene("Main");
		scenePtr->createScene("Settings");
		scenePtr->createScene("Settings-Themes");

		scenePtr->setScene("Main");


		/** The UI design:
		* the application on start will display a clock running and a default image of anya
		* in the top left corner, there will be a settings cog button to modify the clock:
		* - X button to close settings
		* - Themes
		*	- Back arrow to settings | X button to close settings
		*	- Minimal Mode (Minimal)
		*	- Set Background (Background) -> expands -> Open File | Color (with color icon filled)
		*	- Set Text Font (Font)
		* - Quit
		*
		* the settings button should be transparent on window unfocused, otherwise they should be 25% transparent
		*	- on hover, they should be opaque (smooth transition in/out)
		* Screen should fade to next scene (fast)
		*/

		shouldRun = true;

		return true;
	}

	void Anya::update() {
		while (shouldRun == true) {
			SDL_PollEvent(&ev);
			switch (ev.type) {
				case SDL_QUIT: {
					shouldRun = false;
				} break;

				case SDL_MOUSEBUTTONDOWN: {
					for (auto &button : interfacePtr->getButtonList()) {
						if (button->canMinimize && interfacePtr->cursorInBounds(button, interfacePtr->getMousePos()))
							SDL_MinimizeWindow(window.get());

						if (button->canQuit && interfacePtr->cursorInBounds(button, interfacePtr->getMousePos()))
							shouldRun = false;
					}

					// append buttons to layers 
					if (interfacePtr->cursorInBounds(settingsBtn, interfacePtr->getMousePos())) {
						scenePtr->setScene("Settings");
					}

					if (interfacePtr->cursorInBounds(themesBtn, interfacePtr->getMousePos()))
						scenePtr->setScene("Settings-Themes");

					if (interfacePtr->cursorInBounds(settingsExitBtn, interfacePtr->getMousePos())) {
						scenePtr->setScene("Main");
					}

				} break;
			}
			end = std::chrono::steady_clock::now();
			deltaTime = std::chrono::duration<double, std::milli>(end - begin);
			begin = end;

#ifdef _DEBUG
			const auto getTime = [&](std::chrono::system_clock::time_point time) {
				return Anya(time).getStream()->str();
			};

			//std::cout << getTime(std::chrono::system_clock::now()) << '\n';
#endif
			imagePtr->getAnimPtr()->update(40, deltaTime.count());
			interfacePtr->update(&ev);

			draw();
		}
		free();
	}

	// usually you want this to be independent
	void Anya::draw() {
		timeText = imagePtr->createTextA({timeToStr(std::chrono::system_clock::now()), path + "assets/bahnschrift.ttf", {255, 255, 255}, 28}, renderer.get());
		quitText = imagePtr->createText({quitBtn->text, path + "assets/bahnschrift.ttf", {255, 255, 255}, 96}, renderer.get());
		settingsExitText = imagePtr->createText({settingsExitBtn->text, path + "assets/bahnschrift.ttf", {255, 255, 255}, 96}, renderer.get());
		themesText = imagePtr->createText({themesBtn->text, path + "assets/bahnschrift.ttf", {255, 255, 255}, 96}, renderer.get());
		minimalText = imagePtr->createText({minimalBtn->text, path + "assets/bahnschrift.ttf", {255, 255, 255}, 96}, renderer.get());
		setBGText = imagePtr->createText({setBGBtn->text, path + "assets/bahnschrift.ttf", {255, 255, 255}, 96}, renderer.get());
		openFileText = imagePtr->createText({openFileBtn->text, path + "assets/bahnschrift.ttf", {255, 255, 255}, 96}, renderer.get());
		setBGColorText = imagePtr->createText({setBGColorBtn->text, path + "assets/bahnschrift.ttf", {255, 255, 255}, 28}, renderer.get());
		setTextFontText = imagePtr->createText({setTextFontBtn->text, path + "assets/bahnschrift.ttf", {255, 255, 255}, 96}, renderer.get());

		SDL_SetRenderDrawColor(renderer.get(), 10, 10, 25, 255);
		SDL_RenderClear(renderer.get());

		if (scenePtr->getCurrentScene() == scenePtr->findScene("Main")) {
			//imagePtr->draw(background, renderer.get(), 0, 0); // make buttons to change backgrounds
			imagePtr->drawAnimation(backgroundGIF, renderer.get(), 0, 0);
			imagePtr->draw(timeText, renderer.get(), static_cast<int>(windowWidth / 5.5), static_cast<int>(windowHeight / 1.6));

			interfacePtr->draw(settingsBtn, nullptr, renderer.get());
		}

		if (scenePtr->getCurrentScene() == scenePtr->findScene("Settings")) {
			SDL_SetRenderDrawColor(renderer.get(), 0, 0, 0, 255);
			SDL_RenderFillRect(renderer.get(), &settingsView);

			// draw buttons
			interfacePtr->draw(settingsExitBtn, settingsExitText, renderer.get());
			interfacePtr->draw(quitBtn, quitText, renderer.get());
			interfacePtr->draw(themesBtn, themesText, renderer.get());
		}

		if (scenePtr->getCurrentScene() == scenePtr->findScene("Settings-Themes")) {
			SDL_SetRenderDrawColor(renderer.get(), 0, 0, 0, 255);
			SDL_RenderFillRect(renderer.get(), &settingsThemesView);

			interfacePtr->draw(minimalBtn, minimalText, renderer.get());
			interfacePtr->draw(setBGBtn, setBGText, renderer.get());
			interfacePtr->draw(openFileBtn, openFileText, renderer.get());
			interfacePtr->draw(setBGColorBtn, setBGColorText, renderer.get());
			interfacePtr->draw(setTextFontBtn, setTextFontText, renderer.get());
		}

		SDL_RenderPresent(renderer.get());

		if (delay > deltaTime.count())
			SDL_Delay(delay - static_cast<int>(deltaTime.count()));
	}

	void Anya::free() {
		std::cout << "releasing allocated resources..\n";
		TTF_Quit();
		IMG_Quit();
		SDL_Quit();
	}

	std::basic_string<char> Anya::timeToStr(const std::chrono::system_clock::time_point &time) {
		// convert time_point to a useable hour
		struct tm localTime;
		time_t currentTime = std::chrono::system_clock::to_time_t(time);
		localtime_s(&localTime, &currentTime);

		auto hour = std::chrono::hours(localTime.tm_hour);

		if (std::chrono::is_pm(hour)) {
			return std::format("{:%OI:%M}PM", std::chrono::current_zone()->to_local(time));
		} else {
			return std::format("{:%OI:%M}AM", std::chrono::current_zone()->to_local(time));
		}
	}

	std::unique_ptr<std::basic_stringstream<char>> Anya::getStream() {
		auto ptr = std::make_unique<std::basic_stringstream<char>>(std::move(str));
		return ptr;
	}
} // namespace Application