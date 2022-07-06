#include "anya.hpp"
#include <iostream>
#ifdef _WIN32
#include <windows.h>
#include <shellapi.h>
#endif

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

		// initialize components
		imagePtr = std::make_unique<Helper::Image>();
		interfacePtr = std::make_unique<Helper::UInterface>();
		scenePtr = std::make_unique<Helper::Scene>();

		// load assets
		background = imagePtr->createImage(path + "assets/beep_1.png", renderer.get());
		backgroundGIF = imagePtr->createPack("canvas", path + "assets/gif-extract/", renderer.get());
		imagePtr->getAnimPtr()->addAnimation(68, 0, 0, 148, 89);

		// create scenes
		scenePtr->createScene("Main");
		scenePtr->createScene("Settings");
		scenePtr->createScene("Settings-Themes");

		// replace ColorData with ColorState
		// main
		/*
		settingsBtn = interfacePtr->createButton("+", nullptr, {{255, 0, 0, 255}, {0, 255, 0, 255}}, "Main", -2, -15, 25, 50);
		settingsBtn->isEnabled = true;

		// settings
		quitBtn = interfacePtr->createButton("Quit", nullptr, {{255, 0, 0, 255}, {0, 255, 0, 255}}, "Settings", 45, 45, 55, 40);
		quitBtn->canQuit = true;
		quitBtn->isEnabled = true;
		settingsExitBtn = interfacePtr->createButton("x", nullptr, {{255, 0, 0, 255}, {0, 255, 0, 255}}, "Settings", 0, -5, 20, 28);
		themesBtn = interfacePtr->createButton("Themes", nullptr, {{255, 0, 0, 255}, {0, 255, 0, 255}}, "Settings", 35, 10, 80, 40);
		themesExitBtn = interfacePtr->createButton("<-", nullptr, {{255, 0, 0, 255}, {0, 255, 0, 255}}, "Settings", 0, -5, 20, 28);
		// setLayoutBtn = interfacePtr->createButton();

		// settings-themes
		minimalBtn = interfacePtr->createButton("Minimal Mode", nullptr, {{255, 0, 0, 255}, {0, 255, 0, 255}}, "Settings-Themes", 0, 10, 100, 25);
		setBGBtn = interfacePtr->createButton("Set Background", nullptr, {{255, 0, 0, 255}, {0, 255, 0, 255}}, "Settings-Themes", 0, 20, 100, 25);
		openFileBtn = interfacePtr->createButton("Open File", nullptr, {{255, 0, 0, 255}, {0, 255, 0, 255}}, "Settings-Themes", 50, 0, 50, 25);
		setBGColorBtn = interfacePtr->createButton("Set Color", nullptr, {{255, 0, 0, 255}, {0, 255, 0, 255}}, "Settings-Themes", 50, 40, 50, 25);
		setTextFontBtn = interfacePtr->createButton("Set Time Font", nullptr, {{255, 0, 0, 255}, {0, 255, 0, 255}}, "Settings-Themes", 0, 50, 75, 25);
		*/
		
		// MAKE A GITHUB BUTTON utilizing shellexecute

		// main
		
		//settingsBtn = interfacePtr->createButton("+", -2, -15, 25, 50);
		settingsBtn = interfacePtr->createButton("+", 5, 5, 25, 50);
		settingsBtn->isEnabled = true;

		// settings
		quitBtn = interfacePtr->createButton("Quit", 45, 45, 55, 40);
		quitBtn->canQuit = true;
		quitBtn->isEnabled = true;
		settingsExitBtn = interfacePtr->createButton("x", 0, -5, 20, 28);
		themesBtn = interfacePtr->createButton("Themes", 35, 10, 80, 40);
		themesExitBtn = interfacePtr->createButton("<-", 0, -5, 20, 28);
		// setLayoutBtn = interfacePtr->createButton();

		// settings-themes
		minimalBtn = interfacePtr->createButton("Minimal Mode", 0, 10, 100, 25);
		setBGBtn = interfacePtr->createButton("Set Background", 0, 20, 100, 25);
		openFileBtn = interfacePtr->createButton("Open File", 50, 0, 50, 25);
		setBGColorBtn = interfacePtr->createButton("Set Color", 50, 40, 50, 25);
		setTextFontBtn = interfacePtr->createButton("Set Time Font", 0, 50, 75, 25);

		for (auto &button : interfacePtr->getButtonList())
			interfacePtr->setButtonTheme(button, {{67, 48, 46}, {168, 124, 116}, {240, 209, 189}});
		
		// set the scene to be displayed
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

						if (button->canQuit && interfacePtr->cursorInBounds(button, interfacePtr->getMousePos())) {
							if (scenePtr->getCurrentScene() == scenePtr->findScene("Settings"))
								shouldRun = false;
						}
					}
					// checking if the button can be pressed
					
					if (interfacePtr->cursorInBounds(settingsBtn, interfacePtr->getMousePos()) && settingsBtn->isEnabled) {
						scenePtr->setScene("Settings");
						settingsBtn->isEnabled = false;
					}

					//GITHUB BUTTON (Cross platform, use 
					/*
					if (interfacePtr->cursorInBounds(settingsBtn, interfacePtr->getMousePos()) && settingsBtn->isEnabled) {
						
#ifdef _WIN32
						ShellExecute(0, 0, L"https://www.github.com/inohime", 0, 0, SW_SHOW);
#endif

						system("start chrome https://www.github.com/inohime"); // works on windows, test on linux
					}
					*/
					

					if (interfacePtr->cursorInBounds(settingsExitBtn, interfacePtr->getMousePos()) && settingsExitBtn->isEnabled) {
						scenePtr->setScene("Main");
						settingsExitBtn->isEnabled = false;
						themesBtn->isEnabled = false;
						settingsBtn->isEnabled = true;
					}

					if (interfacePtr->cursorInBounds(themesBtn, interfacePtr->getMousePos()) && themesBtn->isEnabled) {
						scenePtr->setScene("Settings-Themes");
						themesBtn->isEnabled = false;
						themesExitBtn->isEnabled = true;
						settingsExitBtn->isEnabled = false;
					}

					if (interfacePtr->cursorInBounds(themesExitBtn, interfacePtr->getMousePos()) && themesExitBtn->isEnabled) {
						scenePtr->setScene("Settings");
						settingsExitBtn->isEnabled = true;
						themesExitBtn->isEnabled = false;
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
			// test button fade in/out (maybe add isEnabled check?
			/*
			if (scenePtr->getCurrentScene() == scenePtr->findScene("Main")) {
				if (interfacePtr->cursorInBounds(settingsBtn, interfacePtr->getMousePos())) {
					std::cout << buttonOpacity << "\n";
					buttonOpacity += 0.35f * deltaTime.count();
					if (buttonOpacity >= SDL_ALPHA_OPAQUE)
						buttonOpacity = SDL_ALPHA_OPAQUE;
				} else {
					buttonOpacity -= 0.35f * deltaTime.count();
					if (buttonOpacity <= 191.25f)
						buttonOpacity = 191.25f;
				}
			}
			*/

			if (scenePtr->getCurrentScene() == scenePtr->findScene("Settings") && !settingsBtn->isEnabled) {
				settingsExitBtn->isEnabled = true;
				themesBtn->isEnabled = true;
			}

			imagePtr->getAnimPtr()->update(40, deltaTime.count());
			interfacePtr->update(&ev, deltaTime.count());

			draw();
		}
		free();
	}

	// usually you want this to be independent
	void Anya::draw() {
		// put these with their respective scenes
		/*
		timeText = imagePtr->createTextA({timeToStr(std::chrono::system_clock::now()), path + "assets/OnestRegular1602-hint.ttf", {255, 255, 255}, 28}, renderer.get());
		quitText = imagePtr->createText({quitBtn->text, path + "assets/OnestRegular1602-hint.ttf", {255, 255, 255}, 28}, renderer.get());
		settingsText = imagePtr->createText({settingsBtn->text, path + "assets/OnestRegular1602-hint.ttf", {240, 209, 189}, 50}, renderer.get()); // 196
		settingsExitText = imagePtr->createText({settingsExitBtn->text, path + "assets/OnestRegular1602-hint.ttf", {255, 255, 255}, 96}, renderer.get());
		themesText = imagePtr->createText({themesBtn->text, path + "assets/OnestRegular1602-hint.ttf", {255, 255, 255}, 32}, renderer.get());
		themesExitText = imagePtr->createText({themesExitBtn->text, path + "assets/OnestRegular1602-hint.ttf", {255, 255, 255}, 96}, renderer.get());
		minimalText = imagePtr->createText({minimalBtn->text, path + "assets/OnestRegular1602-hint.ttf", {255, 255, 255}, 96}, renderer.get());
		setBGText = imagePtr->createText({setBGBtn->text, path + "assets/OnestRegular1602-hint.ttf", {255, 255, 255}, 96}, renderer.get());
		openFileText = imagePtr->createText({openFileBtn->text, path + "assets/OnestRegular1602-hint.ttf", {255, 255, 255}, 96}, renderer.get());
		setBGColorText = imagePtr->createText({setBGColorBtn->text, path + "assets/OnestRegular1602-hint.ttf", {255, 255, 255}, 28}, renderer.get());
		setTextFontText = imagePtr->createText({setTextFontBtn->text, path + "assets/OnestRegular1602-hint.ttf", {255, 255, 255}, 96}, renderer.get());
		*/
		SDL_SetRenderDrawBlendMode(renderer.get(), SDL_BLENDMODE_BLEND);
		SDL_SetRenderDrawColor(renderer.get(), 255, 255, 255, 255); // 10, 10, 25
		SDL_RenderClear(renderer.get());

		if (scenePtr->getCurrentScene() == scenePtr->findScene("Main")) {
			timeText = imagePtr->createTextA({timeToStr(std::chrono::system_clock::now()), path + "assets/OnestRegular1602-hint.ttf", {{0}, {0}, {255, 255, 255}}, 28}, renderer.get());
			settingsText = imagePtr->createText({settingsBtn->text, path + "assets/OnestRegular1602-hint.ttf", settingsBtn->buttonColor, 50}, renderer.get()); // 196
			quitText = imagePtr->createText({quitBtn->text, path + "assets/OnestRegular1602-hint.ttf", quitBtn->buttonColor, 28}, renderer.get());

			//imagePtr->draw(background, renderer.get(), 0, 0); // make buttons to change backgrounds
			imagePtr->drawAnimation(backgroundGIF, renderer.get(), 0, 0);
			imagePtr->draw(timeText, renderer.get(), static_cast<int>(windowWidth / 10), static_cast<int>(windowHeight / 1.6));

			//interfacePtr->draw(settingsBtn, settingsText, renderer.get());

			// make a test button and style it
			/*
			SDL_Rect bg = {5, 5, 25, 25}; // x, y -> 50 (last position)
			SDL_SetRenderDrawColor(renderer.get(), 168, 124, 116, buttonOpacity);
			SDL_RenderFillRect(renderer.get(), &bg);

			// outline
			SDL_Rect o1 = {4, 4, 27, 27}; // x, y -> 49
			SDL_SetRenderDrawColor(renderer.get(), 67, 48, 46, buttonOpacity);
			SDL_RenderDrawRect(renderer.get(), &o1);

			SDL_Rect o2 = {3, 3, 29, 29}; // x, y -> 48
			SDL_SetRenderDrawColor(renderer.get(), 67, 48, 46, buttonOpacity);
			SDL_RenderDrawRect(renderer.get(), &o2);
			*/

			//interfacePtr->setButtonPos(settingsBtn, 5, -7); // x, y -> 50, 38 (last position)
			interfacePtr->draw(settingsBtn, settingsText, renderer.get());
		}

		if (scenePtr->getCurrentScene() == scenePtr->findScene("Settings")) {
			settingsExitText = imagePtr->createText({settingsExitBtn->text, path + "assets/OnestRegular1602-hint.ttf", settingsExitBtn->buttonColor, 96}, renderer.get());
			themesText = imagePtr->createText({themesBtn->text, path + "assets/OnestRegular1602-hint.ttf", themesBtn->buttonColor, 32}, renderer.get());

			SDL_SetRenderDrawColor(renderer.get(), 0, 0, 0, 255);
			SDL_RenderFillRect(renderer.get(), &settingsView);

			interfacePtr->draw(settingsExitBtn, settingsExitText, renderer.get());
			interfacePtr->draw(quitBtn, quitText, renderer.get());
			interfacePtr->draw(themesBtn, themesText, renderer.get());
		}

		if (scenePtr->getCurrentScene() == scenePtr->findScene("Settings-Themes")) {
			themesExitText = imagePtr->createText({themesExitBtn->text, path + "assets/OnestRegular1602-hint.ttf", themesExitBtn->buttonColor, 96}, renderer.get());
			minimalText = imagePtr->createText({minimalBtn->text, path + "assets/OnestRegular1602-hint.ttf", minimalBtn->buttonColor, 96}, renderer.get());
			setBGText = imagePtr->createText({setBGBtn->text, path + "assets/OnestRegular1602-hint.ttf", setBGBtn->buttonColor, 96}, renderer.get());
			openFileText = imagePtr->createText({openFileBtn->text, path + "assets/OnestRegular1602-hint.ttf", openFileBtn->buttonColor, 96}, renderer.get());
			setBGColorText = imagePtr->createText({setBGColorBtn->text, path + "assets/OnestRegular1602-hint.ttf", setBGColorBtn->buttonColor, 28}, renderer.get());
			setTextFontText = imagePtr->createText({setTextFontBtn->text, path + "assets/OnestRegular1602-hint.ttf", setTextFontBtn->buttonColor, 96}, renderer.get());

			SDL_SetRenderDrawColor(renderer.get(), 0, 0, 0, 255);
			SDL_RenderFillRect(renderer.get(), &settingsThemesView);

			interfacePtr->draw(themesExitBtn, themesExitText, renderer.get());
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