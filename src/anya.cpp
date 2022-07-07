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

		// setup minimal mode window dragging
		const auto &hitTestResult = [](SDL_Window *window, const SDL_Point *pt, void *data) -> SDL_HitTestResult {
			SDL_Rect dragRect = {0, 0, 50, 10};
			if (SDL_PointInRect(pt, &dragRect))
				return SDL_HITTEST_DRAGGABLE;

			return SDL_HITTEST_NORMAL;
		};

		SDL_SetWindowHitTest(window.get(), hitTestResult, NULL);

		// initialize components
		imagePtr = std::make_unique<Helper::Image>();
		interfacePtr = std::make_unique<Helper::UInterface>();
		scenePtr = std::make_unique<Helper::Scene>();

		// load assets
		backgroundImg = imagePtr->createImage(path + "assets/beep_1.png", renderer.get());
		githubImg = imagePtr->createImage(path + "assets/25231.png", renderer.get());
		backgroundGIF = imagePtr->createPack("canvas", path + "assets/gif-extract/", renderer.get());
		imagePtr->getAnimPtr()->addAnimation(68, 0, 0, 148, 89);

		// create scenes
		scenePtr->createScene("Main");
		scenePtr->createScene("Settings");
		scenePtr->createScene("Settings-Themes");
		
		// main
		settingsBtn = interfacePtr->createButton("+", 5, 5, 20, 20); // 25 50
		settingsBtn->isEnabled = true;
		mainQuitBtn = interfacePtr->createButton("x", 103, 5, 12, 12);
		mainQuitBtn->isEnabled = false;
		mainQuitBtn->canQuit = true;
		minimizeBtn = interfacePtr->createButton("-", 85, 5, 12, 12);
		minimizeBtn->isEnabled = false;
		minimizeBtn->canMinimize = true;

		// settings
		settingsQuitBtn = interfacePtr->createButton("Quit", 108, 5, 35, 25); // 93, 5, 50, 25
		settingsQuitBtn->canQuit = true;
		settingsQuitBtn->isEnabled = true;
		settingsExitBtn = interfacePtr->createButton("x", 65, 60, 20, 20); 
		themesBtn = interfacePtr->createButton("Themes", 39, 5, 60, 25); // 39, 5, 45, 25
		githubBtn = interfacePtr->createButton("", githubImg, 5, 5, 25, 25);

		// settings-themes
		themesExitBtn = interfacePtr->createButton("x", 65, 60, 20, 20);
		minimalBtn = interfacePtr->createButton("Minimal", 25, 5, 60, 25);
		setBGBtn = interfacePtr->createButton("Set BG", 90, 5, 50, 25);
		openFileBtn = interfacePtr->createButton("Open File", 25, 35, 50, 15);
		setBGColorBtn = interfacePtr->createButton("Set Color", 80, 35, 50, 15);
		setTextFontBtn = interfacePtr->createButton("Set Font", 0, 50, 50, 25);

		for (auto &button : interfacePtr->getButtonList())
			interfacePtr->setButtonTheme(button, {{67, 48, 46}, {168, 124, 116}, {240, 209, 189}});

		imagePtr->setTextureColor(githubImg, {240, 209, 189, (uint8_t)githubBtn->colorAlpha});
		
		// set the scene to be displayed
		scenePtr->setScene("Main");

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
							if (scenePtr->getCurrentScene() == scenePtr->findScene("Main"))
								SDL_MinimizeWindow(window.get());

						if (button->canQuit && interfacePtr->cursorInBounds(button, interfacePtr->getMousePos())) {
							if (scenePtr->getCurrentScene() == scenePtr->findScene("Settings"))
								shouldRun = false;

							if (scenePtr->getCurrentScene() == scenePtr->findScene("Main") && mainQuitBtn->isEnabled)
								shouldRun = false;
						}
					}
					
					if (interfacePtr->cursorInBounds(settingsBtn, interfacePtr->getMousePos()) && settingsBtn->isEnabled) {
						scenePtr->setScene("Settings");
						settingsBtn->isEnabled = false;
					}
					

					//GITHUB BUTTON (Cross platform, use system for unix)
					if (interfacePtr->cursorInBounds(githubBtn, interfacePtr->getMousePos()) && githubBtn->isEnabled) {
#ifdef _WIN32
						ShellExecute(0, 0, L"https://www.github.com/inohime", 0, 0, SW_SHOW);
#elif defined __linux__
						system("xdg-open https://www.github.com/inohime");
#endif
					}
					
					
					if (interfacePtr->cursorInBounds(settingsExitBtn, interfacePtr->getMousePos()) && settingsExitBtn->isEnabled) {
						scenePtr->setScene("Main");
						settingsExitBtn->isEnabled = false;
						themesBtn->isEnabled = false;
						githubBtn->isEnabled = false;
						settingsBtn->isEnabled = true;
					}

					if (interfacePtr->cursorInBounds(themesBtn, interfacePtr->getMousePos()) && themesBtn->isEnabled) {
						scenePtr->setScene("Settings-Themes");
						themesBtn->isEnabled = false;
						githubBtn->isEnabled = false;
						themesExitBtn->isEnabled = true;
						settingsExitBtn->isEnabled = false;
						setBGBtn->isEnabled = true;
					}

					if (interfacePtr->cursorInBounds(setBGBtn, interfacePtr->getMousePos()) && setBGBtn->isEnabled) {
						setBGIsPressed = true;
						openFileBtn->isEnabled = true;
						setBGColorBtn->isEnabled = true;
					}

					if (interfacePtr->cursorInBounds(setBGColorBtn, interfacePtr->getMousePos()) && setBGColorBtn->isEnabled) {
						setBGToColor = true;
					}

					if (interfacePtr->cursorInBounds(minimalBtn, interfacePtr->getMousePos()) && minimalBtn->isEnabled) {
						minimalMode = true;
						mainQuitBtn->isEnabled = true;
						themesExitBtn->isEnabled = false;
						minimalBtn->isEnabled = false;
						setBGBtn->isEnabled = false;
						openFileBtn->isEnabled = false;
						SDL_SetWindowBordered(window.get(), SDL_FALSE);
						SDL_SetWindowSize(window.get(), 120, 50);
						scenePtr->setScene("Main");
					}

					if (interfacePtr->cursorInBounds(themesExitBtn, interfacePtr->getMousePos()) && themesExitBtn->isEnabled) {
						scenePtr->setScene("Settings");
						settingsExitBtn->isEnabled = true;
						themesExitBtn->isEnabled = false;
						minimalBtn->isEnabled = false;
						setBGBtn->isEnabled = false;
						openFileBtn->isEnabled = false;
						setBGIsPressed = false;
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

			if (scenePtr->getCurrentScene() == scenePtr->findScene("Settings") && !settingsBtn->isEnabled) {
				settingsExitBtn->isEnabled = true;
				themesBtn->isEnabled = true;
				githubBtn->isEnabled = true;
			}

			if (scenePtr->getCurrentScene() == scenePtr->findScene("Settings-Themes") && !settingsExitBtn->isEnabled)
				minimalBtn->isEnabled = true;

			imagePtr->getAnimPtr()->update(40, deltaTime.count());
			interfacePtr->update(&ev, deltaTime.count());

			draw();
		}
		free();
	}

	// usually you want this to be independent
	void Anya::draw() {
		SDL_SetRenderDrawBlendMode(renderer.get(), SDL_BLENDMODE_BLEND);
		SDL_SetRenderDrawColor(renderer.get(), 255, 255, 255, 255);
		SDL_RenderClear(renderer.get());

		if (scenePtr->getCurrentScene() == scenePtr->findScene("Main")) {
			timeText = imagePtr->createTextA({timeToStr(std::chrono::system_clock::now()), path + "assets/OnestRegular1602-hint.ttf", {{0}, {0}, {255, 255, 255}}, 28}, renderer.get());
			settingsText = imagePtr->createText({settingsBtn->text, path + "assets/OnestRegular1602-hint.ttf", settingsBtn->buttonColor, 96}, renderer.get()); // 196 / 50

			if (setBGToColor) {
				SDL_SetRenderDrawColor(renderer.get(), 0, 0, 0, 255);
				SDL_RenderFillRect(renderer.get(), &fillBGColor);
			} else {
				imagePtr->drawAnimation(backgroundGIF, renderer.get(), 0, 0);
			}

			if (minimalMode) {
				mainQuitText = imagePtr->createText({mainQuitBtn->text, path + "assets/OnestRegular1602-hint.ttf", mainQuitBtn->buttonColor, 96}, renderer.get());
				minimizeText = imagePtr->createText({minimizeBtn->text, path + "assets/OnestRegular1602-hint.ttf", minimizeBtn->buttonColor, 96}, renderer.get());

				SDL_SetRenderDrawColor(renderer.get(), 0, 0, 0, 255);
				SDL_RenderFillRect(renderer.get(), &fillBGColor);

				imagePtr->draw(timeText, renderer.get(), 0, 18);

				interfacePtr->setButtonTextSize(mainQuitText, -2, 0);
				interfacePtr->draw(mainQuitBtn, mainQuitText, renderer.get());
				//interfacePtr->setButtonTextSize(minimizeText, -2, 0);
				interfacePtr->draw(minimizeBtn, minimizeText, renderer.get());
			} else {
				imagePtr->draw(timeText, renderer.get(), static_cast<int>(windowWidth / 10), static_cast<int>(windowHeight / 1.6));
				// put the settings button in non minimal mode for now, resize & set button pos for minimal mode later
				interfacePtr->setButtonTextSize(settingsText, 1, 16);
				interfacePtr->draw(settingsBtn, settingsText, renderer.get());
			}
		}

		if (scenePtr->getCurrentScene() == scenePtr->findScene("Settings")) {
			settingsExitText = imagePtr->createText({settingsExitBtn->text, path + "assets/OnestRegular1602-hint.ttf", settingsExitBtn->buttonColor, 72}, renderer.get());
			themesText = imagePtr->createText({themesBtn->text, path + "assets/OnestRegular1602-hint.ttf", themesBtn->buttonColor, 32}, renderer.get());
			quitText = imagePtr->createText({settingsQuitBtn->text, path + "assets/OnestRegular1602-hint.ttf", settingsQuitBtn->buttonColor, 96}, renderer.get());

			SDL_SetRenderDrawColor(renderer.get(), 26, 17, 16, 255);
			SDL_RenderFillRect(renderer.get(), &settingsView);

			interfacePtr->draw(settingsExitBtn, settingsExitText, renderer.get());
			interfacePtr->draw(settingsQuitBtn, quitText, renderer.get());
			interfacePtr->draw(githubBtn, nullptr, renderer.get());
			interfacePtr->draw(themesBtn, themesText, renderer.get());
		}

		if (scenePtr->getCurrentScene() == scenePtr->findScene("Settings-Themes")) {
			themesExitText = imagePtr->createText({themesExitBtn->text, path + "assets/OnestRegular1602-hint.ttf", themesExitBtn->buttonColor, 96}, renderer.get());
			minimalText = imagePtr->createText({minimalBtn->text, path + "assets/OnestRegular1602-hint.ttf", minimalBtn->buttonColor, 96}, renderer.get());
			setBGText = imagePtr->createText({setBGBtn->text, path + "assets/OnestRegular1602-hint.ttf", setBGBtn->buttonColor, 96}, renderer.get());
			openFileText = imagePtr->createText({openFileBtn->text, path + "assets/OnestRegular1602-hint.ttf", openFileBtn->buttonColor, 96}, renderer.get());
			setBGColorText = imagePtr->createText({setBGColorBtn->text, path + "assets/OnestRegular1602-hint.ttf", setBGColorBtn->buttonColor, 28}, renderer.get());
			//setTextFontText = imagePtr->createText({setTextFontBtn->text, path + "assets/OnestRegular1602-hint.ttf", setTextFontBtn->buttonColor, 96}, renderer.get());

			SDL_SetRenderDrawColor(renderer.get(), 26, 17, 16, 255);
			SDL_RenderFillRect(renderer.get(), &settingsThemesView);

			interfacePtr->draw(themesExitBtn, themesExitText, renderer.get());
			interfacePtr->draw(minimalBtn, minimalText, renderer.get());
			interfacePtr->draw(setBGBtn, setBGText, renderer.get());

			if (setBGIsPressed) {
				interfacePtr->draw(openFileBtn, openFileText, renderer.get());
				interfacePtr->draw(setBGColorBtn, setBGColorText, renderer.get());
			}
			//interfacePtr->draw(setTextFontBtn, setTextFontText, renderer.get());
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