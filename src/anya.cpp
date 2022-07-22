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
		dirPath = base;
		SDL_free(base);

		// setup minimal mode window dragging
		const auto hitTestResult = [](SDL_Window *window, const SDL_Point *pt, void *data) -> SDL_HitTestResult {
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

		// set the default font
		typographyStr = dirPath + "assets/OnestRegular1602-hint.ttf";

		// load assets
		backgroundImg = imagePtr->createImage(dirPath + "assets/beep_1.png", renderer.get());
		githubImg = imagePtr->createImage(dirPath + "assets/25231.png", renderer.get());
		calendarImg = imagePtr->createImage(dirPath + "assets/calendar.png", renderer.get());
		typographyImg = imagePtr->createImage(dirPath + "assets/typography.png", renderer.get());
		returnImg = imagePtr->createImage(dirPath + "assets/return.png", renderer.get());
		backgroundGIF = imagePtr->createPack("canvas", dirPath + "assets/gif-extract/", renderer.get());
		imagePtr->getAnimPtr()->addAnimation(68, 0, 0, 148, 89);

		// create scenes
		scenePtr->createScene("Main");
		scenePtr->createScene("Settings");
		scenePtr->createScene("Settings-Themes");
		scenePtr->createScene("Themes-Background-Color");

		// main
		settingsBtn = interfacePtr->createButton("+", 5, 5, 20, 20);
		settingsBtn->isEnabled = true;
		mainQuitBtn = interfacePtr->createButton("x", 103, 5, 12, 12);
		mainQuitBtn->canQuit = true;
		minimizeBtn = interfacePtr->createButton("-", 85, 5, 12, 12);
		minimizeBtn->canMinimize = true;
		returnBtn = interfacePtr->createButton("", returnImg, 67, 5, 12, 12);

		// settings
		settingsQuitBtn = interfacePtr->createButton("Quit", 108, 5, 35, 25);
		settingsQuitBtn->canQuit = true;
		settingsQuitBtn->isEnabled = true;
		settingsExitBtn = interfacePtr->createButton("x", 65, 60, 20, 20);
		themesBtn = interfacePtr->createButton("Themes", 39, 5, 60, 25);
		githubBtn = interfacePtr->createButton("", githubImg, 5, 5, 25, 25);
		calendarBtn = interfacePtr->createButton("", calendarImg, 5, 39, 25, 25);

		// settings-themes
		themesExitBtn = interfacePtr->createButton("x", 65, 60, 20, 20);
		minimalBtn = interfacePtr->createButton("Minimal", 39, 5, 55, 25);
		setBGBtn = interfacePtr->createButton("Set BG", 103, 5, 40, 25);
		openFileBtn = interfacePtr->createButton("Open File", 25, 35, 50, 15);
		setBGColorBtn = interfacePtr->createButton("Set Color", 80, 35, 50, 15);
		setTypographyBtn = interfacePtr->createButton("", typographyImg, 5, 5, 25, 25);
		typographyInputBtn = interfacePtr->createButton("Set Font", setTypographyBtn->box.w / 2, 35,
														(setTypographyBtn->box.x + (setTypographyBtn->box.w / 2)) +
														(setBGColorBtn->box.x + (setBGColorBtn->box.w / 2)), 15);

		for (auto &button : interfacePtr->getButtonList())
			interfacePtr->setButtonTheme(button, {{67, 48, 46}, {168, 124, 116}, {240, 209, 189}});

		imagePtr->setTextureColor(githubImg, {240, 209, 189, (uint8_t)githubBtn->colorAlpha});
		imagePtr->setTextureColor(calendarImg, {240, 209, 189, (uint8_t)calendarBtn->colorAlpha});
		imagePtr->setTextureColor(typographyImg, {240, 209, 189, (uint8_t)setTypographyBtn->colorAlpha});
		imagePtr->setTextureColor(returnImg, {240, 209, 189, (uint8_t)returnBtn->colorAlpha});

		// set the scene to be displayed
		scenePtr->setScene("Main");

		shouldRun = true;

		return true;
	}

	void Anya::update() {
		while (shouldRun) {
			SDL_PollEvent(&ev);

			switch (ev.type) {
				case SDL_QUIT: {
					shouldRun = false;
				} break;

				case SDL_MOUSEBUTTONDOWN: {
					for (auto &button : interfacePtr->getButtonList()) {
						if (button->canMinimize && interfacePtr->cursorInBounds(button, interfacePtr->getMousePos()))
							if (scenePtr->getCurrentScene() == scenePtr->findScene("Main") && minimizeBtn->isEnabled)
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
						calendarBtn->isEnabled = false;
						settingsBtn->isEnabled = true;
					}

					if (interfacePtr->cursorInBounds(themesBtn, interfacePtr->getMousePos()) && themesBtn->isEnabled) {
						scenePtr->setScene("Settings-Themes");
						themesBtn->isEnabled = false;
						githubBtn->isEnabled = false;
						calendarBtn->isEnabled = false;
						themesExitBtn->isEnabled = true;
						settingsExitBtn->isEnabled = false;
					}

					if (interfacePtr->cursorInBounds(calendarBtn, interfacePtr->getMousePos()) && calendarBtn->isEnabled && !showDate) {
						showDate = true;
					} else if (interfacePtr->cursorInBounds(calendarBtn, interfacePtr->getMousePos()) && calendarBtn->isEnabled && showDate) {
						showDate = false;
					}

					if (interfacePtr->cursorInBounds(setBGBtn, interfacePtr->getMousePos()) && setBGBtn->isEnabled && !setBGIsPressed) {
						setTypographyIsPressed = false;
						setBGIsPressed = true;
						openFileBtn->isEnabled = true;
						setBGColorBtn->isEnabled = true;
					} else if (interfacePtr->cursorInBounds(setBGBtn, interfacePtr->getMousePos()) && setBGBtn->isEnabled && setBGIsPressed) {
						setBGIsPressed = false;
						openFileBtn->isEnabled = false;
						setBGColorBtn->isEnabled = false;
					}

					if (interfacePtr->cursorInBounds(setTypographyBtn, interfacePtr->getMousePos()) && setTypographyBtn->isEnabled && !setTypographyIsPressed) {
						setTypographyIsPressed = true;
						setBGIsPressed = false;
						typographyInputBtn->isEnabled = true;
					} else if (interfacePtr->cursorInBounds(setTypographyBtn, interfacePtr->getMousePos()) && setTypographyBtn->isEnabled && setTypographyIsPressed) {
						setTypographyIsPressed = false;
						typographyInputBtn->isEnabled = false;
					}

					if (interfacePtr->cursorInBounds(typographyInputBtn, interfacePtr->getMousePos()) && typographyInputBtn->isEnabled) {
						typographyInputBtn->text = "";
					}

					if (interfacePtr->cursorInBounds(setBGColorBtn, interfacePtr->getMousePos()) && setBGColorBtn->isEnabled) {
						setBGToColor = true;
						setBGColorBtn->text = "";
					}

					if (interfacePtr->cursorInBounds(minimalBtn, interfacePtr->getMousePos()) && minimalBtn->isEnabled) {
						minimalMode = true;
						themesExitBtn->isEnabled = false;
						minimalBtn->isEnabled = false;
						setBGBtn->isEnabled = false;
						openFileBtn->isEnabled = false;
						SDL_SetWindowBordered(window.get(), SDL_FALSE);
						SDL_SetWindowSize(window.get(), 120, 50);
						scenePtr->setScene("Main");
					}

					if (interfacePtr->cursorInBounds(returnBtn, interfacePtr->getMousePos()) && returnBtn->isEnabled && minimalMode) {
						minimalMode = false;
						themesExitBtn->isEnabled = true;
						SDL_SetWindowBordered(window.get(), SDL_TRUE);
						SDL_SetWindowSize(window.get(), windowWidth, windowHeight);
						scenePtr->setScene("Settings-Themes");
					}

					if (interfacePtr->cursorInBounds(themesExitBtn, interfacePtr->getMousePos()) && themesExitBtn->isEnabled) {
						scenePtr->setScene("Settings");
						settingsExitBtn->isEnabled = true;
						themesExitBtn->isEnabled = false;
						minimalBtn->isEnabled = false;
						setBGBtn->isEnabled = false;
						openFileBtn->isEnabled = false;
						setTypographyBtn->isEnabled = false;
						setBGIsPressed = false;
						setTypographyIsPressed = false;
					}
				} break;

				case SDL_KEYDOWN: {
					switch (ev.key.keysym.sym) {
						case SDLK_RETURN: {
							if (setBGIsPressed) {
								auto &bgColorText = setBGColorBtn->text;
								// apply the colour to the background and reset the text
								if (bgColorText.contains(',')) {
									bgColorText.erase(std::remove(bgColorText.begin(), bgColorText.end(), ','));
									// delete the duplicate character at the end
									bgColorText.pop_back();
									// find the all of the spaces
									auto first = bgColorText.find_first_of(' ');
									auto second = bgColorText.find_last_of(' ');
									// get the positions of the colour values and apply them
									rVal = std::stoi(bgColorText.substr(0, first));
									gVal = std::stoi(bgColorText.substr(first, second));
									bVal = std::stoi(bgColorText.substr(second, bgColorText.back()));
									// 255 163 210 (demo colour)
								} else if (bgColorText.contains('#')) {
									char const *hexVal = bgColorText.c_str();
									// convert the hex to rgb
									sscanf_s(hexVal, "#%02x%02x%02x", &rVal, &gVal, &bVal);
								}
								setBGColorBtn->text = "Set Color";
							} else if (setTypographyIsPressed) {
								typographyStr = dirPath + "assets/" + typographyInputBtn->text;
								typographyInputBtn->text = "Set Font";
							}
						} break;

						case SDLK_c: {
							if (setBGIsPressed) {
								if (SDL_GetModState() & KMOD_CTRL)
									SDL_SetClipboardText(setBGColorBtn->text.c_str());
							} else if (setTypographyIsPressed) {
								if (SDL_GetModState() & KMOD_CTRL)
									SDL_SetClipboardText(typographyInputBtn->text.c_str());
							}
						} break;

						case SDLK_v: {
							if (setBGIsPressed) {
								if (SDL_GetModState() & KMOD_CTRL)
									setBGColorBtn->text = SDL_GetClipboardText();
							} else if (setTypographyIsPressed) {
								if (SDL_GetModState() & KMOD_CTRL)
									typographyInputBtn->text = SDL_GetClipboardText();
							}
						} break;

						case SDLK_BACKSPACE: {
							if (setBGIsPressed) {
								if (setBGColorBtn->text.contains("Set Color"))
									break;

								if (setBGColorBtn->text.length() > 0)
									setBGColorBtn->text.pop_back();
							} else if (setTypographyIsPressed) {
								if (typographyInputBtn->text.contains("Set Font"))
									break;

								if (typographyInputBtn->text.length() > 0)
									typographyInputBtn->text.pop_back();
							}
						} break;
					}
				} break;

				case SDL_TEXTINPUT: {
					if (setBGColorBtn->isEnabled || setTypographyBtn->isEnabled) {
						if (!(SDL_GetModState() & KMOD_CTRL && (ev.text.text[0] == 'c' || ev.text.text[0] == 'C' ||
																ev.text.text[0] == 'v' || ev.text.text[0] == 'V'))) {
							if (setBGToColor) {
								if (setBGColorBtn->text.contains("Set Color"))
									break;

								setBGColorBtn->text += ev.text.text;
							} else if (setTypographyIsPressed) {
								if (typographyInputBtn->text.contains("Set Font"))
									break;

								typographyInputBtn->text += ev.text.text;
							}
						}
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
			// enable these buttons when first layer's buttons are disabled
			if (scenePtr->getCurrentScene() == scenePtr->findScene("Settings") && !settingsBtn->isEnabled) {
				settingsExitBtn->isEnabled = true;
				themesBtn->isEnabled = true;
				githubBtn->isEnabled = true;
				calendarBtn->isEnabled = true;
			}

			// enable these buttons when the second layer's butons are disabled
			if (scenePtr->getCurrentScene() == scenePtr->findScene("Settings-Themes") && !themesBtn->isEnabled) {
				setBGBtn->isEnabled = true;
				minimalBtn->isEnabled = true;
				setTypographyBtn->isEnabled = true;
			}

			// when minimalMode is false, we can keep the returnBtn from being clickable through the Settings-Theme layer
			if (minimalMode) {
				mainQuitBtn->isEnabled = true;
				minimizeBtn->isEnabled = true;
				returnBtn->isEnabled = true;
			} else {
				mainQuitBtn->isEnabled = false;
				minimizeBtn->isEnabled = false;
				returnBtn->isEnabled = false;
			}

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
			timeText = imagePtr->createTextA({timeToStr(std::chrono::system_clock::now()), typographyStr, {{0}, {0}, {255, 255, 255}}, 28}, renderer.get());
			dateText = imagePtr->createTextA({std::format("{:%Ex}", std::chrono::current_zone()->to_local(std::chrono::system_clock::now())), dirPath + "assets/OnestRegular1602-hint.ttf", {{0}, {0}, {255, 255, 255}}, 16}, renderer.get());
			settingsText = imagePtr->createText({settingsBtn->text, dirPath + "assets/OnestRegular1602-hint.ttf", settingsBtn->buttonColor, 96}, renderer.get());

			if (setBGToColor) {
				SDL_SetRenderDrawColor(renderer.get(), rVal, gVal, bVal, 255);
				SDL_RenderFillRect(renderer.get(), &fillBGColor);
			} else {
				imagePtr->drawAnimation(backgroundGIF, renderer.get(), 0, 0);
			}

			if (minimalMode) {
				mainQuitText = imagePtr->createText({mainQuitBtn->text, dirPath + "assets/OnestRegular1602-hint.ttf", mainQuitBtn->buttonColor, 96}, renderer.get());
				minimizeText = imagePtr->createText({minimizeBtn->text, dirPath + "assets/OnestRegular1602-hint.ttf", minimizeBtn->buttonColor, 96}, renderer.get());

				SDL_SetRenderDrawColor(renderer.get(), 0, 0, 0, 255);
				SDL_RenderFillRect(renderer.get(), &fillBGColor);

				imagePtr->draw(timeText, renderer.get(), 0, 18);

				interfacePtr->setButtonTextSize(mainQuitText, -2, 0);
				interfacePtr->draw(mainQuitBtn, mainQuitText, renderer.get());
				interfacePtr->draw(minimizeBtn, minimizeText, renderer.get());
				interfacePtr->draw(returnBtn, nullptr, renderer.get());
			} else {
				if (showDate) {
					imagePtr->draw(timeText, renderer.get(), static_cast<int>(windowWidth / 10), static_cast<int>(windowHeight / 1.6));
					imagePtr->draw(dateText, renderer.get(), static_cast<int>(windowWidth / 4), static_cast<int>(windowHeight / 2.1));
				} else {
					imagePtr->draw(timeText, renderer.get(), static_cast<int>(windowWidth / 10), static_cast<int>(windowHeight / 1.6));
				}
				// put the settings button in non minimal mode for now, resize & set button pos for minimal mode later
				interfacePtr->setButtonTextSize(settingsText, 1, 16);
				interfacePtr->draw(settingsBtn, settingsText, renderer.get());
			}
		}

		if (scenePtr->getCurrentScene() == scenePtr->findScene("Settings")) {
			settingsExitText = imagePtr->createText({settingsExitBtn->text, dirPath + "assets/OnestRegular1602-hint.ttf", settingsExitBtn->buttonColor, 72}, renderer.get());
			themesText = imagePtr->createText({themesBtn->text, dirPath + "assets/OnestRegular1602-hint.ttf", themesBtn->buttonColor, 32}, renderer.get());
			quitText = imagePtr->createText({settingsQuitBtn->text, dirPath + "assets/OnestRegular1602-hint.ttf", settingsQuitBtn->buttonColor, 96}, renderer.get());
			// brown background colour
			SDL_SetRenderDrawColor(renderer.get(), 26, 17, 16, 255);
			SDL_RenderFillRect(renderer.get(), &settingsView);

			interfacePtr->draw(settingsExitBtn, settingsExitText, renderer.get());
			interfacePtr->draw(settingsQuitBtn, quitText, renderer.get());
			interfacePtr->draw(githubBtn, nullptr, renderer.get());
			interfacePtr->draw(themesBtn, themesText, renderer.get());
			interfacePtr->draw(calendarBtn, nullptr, renderer.get());
		}

		if (scenePtr->getCurrentScene() == scenePtr->findScene("Settings-Themes")) {
			themesExitText = imagePtr->createText({themesExitBtn->text, dirPath + "assets/OnestRegular1602-hint.ttf", themesExitBtn->buttonColor, 96}, renderer.get());
			minimalText = imagePtr->createText({minimalBtn->text, dirPath + "assets/OnestRegular1602-hint.ttf", minimalBtn->buttonColor, 96}, renderer.get());
			setBGText = imagePtr->createText({setBGBtn->text, dirPath + "assets/OnestRegular1602-hint.ttf", setBGBtn->buttonColor, 96}, renderer.get());
			// brown background colour
			SDL_SetRenderDrawColor(renderer.get(), 26, 17, 16, 255);
			SDL_RenderFillRect(renderer.get(), &settingsThemesView);

			interfacePtr->draw(themesExitBtn, themesExitText, renderer.get());
			interfacePtr->draw(minimalBtn, minimalText, renderer.get());
			interfacePtr->draw(setBGBtn, setBGText, renderer.get());
			interfacePtr->draw(setTypographyBtn, nullptr, renderer.get());

			if (setTypographyIsPressed) {
				typographyInputText = imagePtr->createText({typographyInputBtn->text, dirPath + "assets/OnestRegular1602-hint.ttf", openFileBtn->buttonColor, 96}, renderer.get());

				interfacePtr->setButtonTextSize(typographyInputText, -45, 2);
				interfacePtr->draw(typographyInputBtn, typographyInputText, renderer.get());
			}

			if (setBGIsPressed) {
				openFileText = imagePtr->createText({openFileBtn->text, dirPath + "assets/OnestRegular1602-hint.ttf", openFileBtn->buttonColor, 96}, renderer.get());
				setBGColorText = imagePtr->createText({setBGColorBtn->text, dirPath + "assets/OnestRegular1602-hint.ttf", setBGColorBtn->buttonColor, 28}, renderer.get());

				interfacePtr->draw(openFileBtn, openFileText, renderer.get());
				interfacePtr->draw(setBGColorBtn, setBGColorText, renderer.get());
			}
		}

		SDL_RenderPresent(renderer.get());

		if (delay > deltaTime.count())
			SDL_Delay(delay - static_cast<int>(deltaTime.count()));
	}

	void Anya::free() {
		std::cout << "releasing allocated resources..\n";
		SDL_StopTextInput();
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