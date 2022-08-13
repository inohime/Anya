#include <SDL_syswm.h>
#include "anya.hpp"
#include "nfd.hpp"
#include <array>
#include <iostream>

namespace Application {
	Anya::Anya() {
		if (!boot()) {
			SDL_ShowSimpleMessageBox(
				SDL_MESSAGEBOX_ERROR,
				"Anya App Error",
				"BOOT FAILURE: Window or Renderer not initialized.\n\n"
				"Verify these application DLLs exist:\n"
				"SDL2.dll\n"
				"SDL2_image.dll\n"
				"SDL2_ttf.dll\n"
				"libfreetype-6.dll\n"
				"libpng16-16.dll\n"
				"zlib1.dll",
				window.get());

		} else {
			update();
		}
	}

	// make this a cross platform function (void * for handle)
#ifdef _WIN32 
	static void setWindowShadow(HWND handle, const MARGINS &margins) {
		DwmExtendFrameIntoClientArea(handle, &margins);
		SetWindowPos(handle, nullptr, 0, 0, 0, 0, SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOMOVE | SWP_NOSIZE | SWP_FRAMECHANGED);
	};
#endif

	bool Anya::boot() {
		SDL_assert(SDL_Init(SDL_INIT_EVERYTHING) == 0);
		SDL_assert(IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG) != 0);
		SDL_assert(TTF_Init() != -1);

		NFD::Guard nfdInit;

		begin = std::chrono::steady_clock::now();

		SDL_SetHintWithPriority("SDL_BORDERLESS_WINDOWED_STYLE", "1", SDL_HINT_OVERRIDE);

		window = cheesecake(SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, windowWidth, windowHeight, 0));
		renderer = cheesecake(SDL_CreateRenderer(window.get(), -1, SDL_RENDERER_SOFTWARE));
		if (!window || !renderer) {
			errStr = SDL_GetError();
			std::cout << "Failed to boot: " << errStr << '\n';
			free();
			return shouldRun;
		}

		SDL_SysWMinfo wmInfo;
		SDL_VERSION(&wmInfo.version);
		SDL_GetWindowWMInfo(window.get(), &wmInfo);
#ifdef _WIN32
		hwnd = wmInfo.info.win.window;
#endif

		// setup minimal mode window dragging
		const auto hitTestResult = [](SDL_Window *window, const SDL_Point *pt, void *data) -> SDL_HitTestResult {
			SDL_Rect dragRect = {0, 0, 50, 10};
			if (SDL_PointInRect(pt, &dragRect))
				return SDL_HITTEST_DRAGGABLE;

			return SDL_HITTEST_NORMAL;
		};

		SDL_SetWindowHitTest(window.get(), hitTestResult, NULL);

		char *const base = SDL_GetBasePath();
		dirPath = base;
		SDL_free(base);

		// initialize components
		imagePtr = std::make_unique<Helper::Image>();
		interfacePtr = std::make_unique<Helper::UInterface>();
		scenePtr = std::make_unique<Helper::Scene>();

		// set the default font
		typographyStr = dirPath + "assets/Onest.ttf";

		// load assets
		backgroundGIF = imagePtr->createPack("canvas", dirPath + "assets/gif-extract/", renderer.get());
		// default background image to use
		githubImg = imagePtr->createImage(dirPath + "assets/25231.png", renderer.get());
		calendarImg = imagePtr->createImage(dirPath + "assets/calendar.png", renderer.get());
		typographyImg = imagePtr->createImage(dirPath + "assets/typography.png", renderer.get());
		returnImg = imagePtr->createImage(dirPath + "assets/return.png", renderer.get());
		setThemeImg = imagePtr->createImage(dirPath + "assets/paintbrush.png", renderer.get());
		imagePtr->getAnimPtr()->addAnimation(68, 0, 0, 148, 89);

		// create scenes
		scenePtr->createScene("Main");
		scenePtr->createScene("Settings");
		scenePtr->createScene("Settings-Themes");
		scenePtr->createScene("Settings-Theme-Creator");

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
		typographyInputBtn = interfacePtr->createButton("Set Font", setTypographyBtn->box.w / 2, 35, 120, 15);
		setThemeBtn = interfacePtr->createButton("", setThemeImg, 5, 39, 25, 25);

		// settings-theme-creator
		exitThemeCreatorBtn = interfacePtr->createButton("x", 30, 74, 12, 12);
		setMenuBGBtn = interfacePtr->createButton("Menu BG", 13, 5, 48, 12);
		setButtonBGCBtn = interfacePtr->createButton("BKGD", 13, 22, 48, 12);
		setButtonOCBtn = interfacePtr->createButton("Outline", 13, 39, 48, 12);
		setButtonTCBtn = interfacePtr->createButton("Text", 13, 56, 48, 12);
		buttonColorInputBtn = interfacePtr->createButton("Input", ((int)windowWidth / 2) + 11, 76, 62, 12);

		for (auto &button : interfacePtr->getButtonList())
			interfacePtr->setButtonTheme(button, {{67, 48, 46}, {168, 124, 116}, {240, 209, 189}});

		imagePtr->setTextureColor(githubImg, {240, 209, 189, (uint8_t)githubBtn->colorAlpha});
		imagePtr->setTextureColor(calendarImg, {240, 209, 189, (uint8_t)calendarBtn->colorAlpha});
		imagePtr->setTextureColor(typographyImg, {240, 209, 189, (uint8_t)setTypographyBtn->colorAlpha});
		imagePtr->setTextureColor(returnImg, {240, 209, 189, (uint8_t)returnBtn->colorAlpha});
		imagePtr->setTextureColor(setThemeImg, {240, 209, 189, (uint8_t)setThemeBtn->colorAlpha});

		// extras
		// center
		themesSliderOutline[0].position.x = static_cast<float>((windowWidth / 2) + 12);
		themesSliderOutline[0].position.y = 50.0f; // follows mouse cursor when pressed on
		themesSliderOutline[0].color = {0, 0, 0, 255}; // set to the current colour hovered
		// left
		themesSliderOutline[1].position.x = static_cast<float>((windowWidth / 2) - 1);
		themesSliderOutline[1].position.y = 43.0f;
		themesSliderOutline[1].color = {0, 0, 0, 255};
		// right
		themesSliderOutline[2].position.x = static_cast<float>((windowWidth / 2) - 1);
		themesSliderOutline[2].position.y = 57.0f;
		themesSliderOutline[2].color = {0, 0, 0, 255};

		// center
		themesSlider[0].position.x = static_cast<float>((windowWidth / 2) + 10);
		themesSlider[0].position.y = 50.0f;
		themesSlider[0].color = {255, 255, 255, 255};
		// left
		themesSlider[1].position.x = static_cast<float>(windowWidth / 2);
		themesSlider[1].position.y = 45.0f;
		themesSlider[1].color = {255, 255, 255, 255};
		// right
		themesSlider[2].position.x = static_cast<float>(windowWidth / 2);
		themesSlider[2].position.y = 55.0f;
		themesSlider[2].color = {255, 255, 255, 255};

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
						if (setTypographyIsPressed) {
							setTypographyIsPressed = false;
							typographyInputBtn->isEnabled = false;
						}

						setBGIsPressed = true;
						openFileBtn->isEnabled = true;
						setBGColorBtn->isEnabled = true;
					} else if (interfacePtr->cursorInBounds(setBGBtn, interfacePtr->getMousePos()) && setBGBtn->isEnabled && setBGIsPressed) {
						setBGIsPressed = false;
						openFileBtn->isEnabled = false;
						setBGColorBtn->isEnabled = false;
						setBGColorBtn->text = "Set Color";
					}

					if (interfacePtr->cursorInBounds(openFileBtn, interfacePtr->getMousePos()) && openFileBtn->isEnabled && setBGIsPressed) {
						// this operation increases memory usage substantially
						NFD::UniquePath filePath = nullptr;
						const nfdfilteritem_t filterItem[1] = {"Image formats (*.jpg, *.jpeg, *.png)", "jpg,jpeg,png"};
						nfdresult_t result = NFD::OpenDialog(filePath, filterItem, 1, NULL);
#ifdef _DEBUG
						if (result == NFD_OKAY) {
							std::cout << "Success!\n";
							std::cout << filePath.get() << '\n';
						} else if (result == NFD_CANCEL) {
							std::cout << "Canceled file dialog operation\n";
						} else {
							std::cout << "Error: " << NFD_GetError() << '\n';
						}
#endif
						if (result == NFD_OKAY) {
							if (setBGToColor)
								setBGToColor = false;

							backgroundImg = imagePtr->createImage(filePath.get(), renderer.get());
							setBGtoImg = true;
						} else if (result == NFD_CANCEL) {
							break;
						}
					}

					if (interfacePtr->cursorInBounds(setTypographyBtn, interfacePtr->getMousePos()) && setTypographyBtn->isEnabled && !setTypographyIsPressed) {
						if (setBGIsPressed) {
							setBGIsPressed = false;
							setBGColorBtn->isEnabled = false;
						}

						setTypographyIsPressed = true;
						typographyInputBtn->isEnabled = true;
					} else if (interfacePtr->cursorInBounds(setTypographyBtn, interfacePtr->getMousePos()) && setTypographyBtn->isEnabled && setTypographyIsPressed) {
						setTypographyIsPressed = false;
						typographyInputBtn->isEnabled = false;
						typographyInputBtn->text = "Set Font";
					}

					if (interfacePtr->cursorInBounds(typographyInputBtn, interfacePtr->getMousePos()) && typographyInputBtn->isEnabled) {
						typographyInputBtn->text = "";
					}

					if (interfacePtr->cursorInBounds(setThemeBtn, interfacePtr->getMousePos()) && setThemeBtn->isEnabled) {
						setThemeIsPressed = true;
						setMenuBGBtn->isEnabled = true;
						setButtonBGCBtn->isEnabled = true;
						setButtonOCBtn->isEnabled = true;
						setButtonTCBtn->isEnabled = true;
					} else {
						setThemeIsPressed = false;
						setMenuBGBtn->isEnabled = false;
						setButtonBGCBtn->isEnabled = false;
						setButtonOCBtn->isEnabled = false;
						setButtonTCBtn->isEnabled = false;
					}

					if (interfacePtr->cursorInBounds(setBGColorBtn, interfacePtr->getMousePos()) && setBGColorBtn->isEnabled) {
						if (setBGtoImg)
							setBGtoImg = false;

						setBGToColor = true;
						setBGColorBtn->text = "";
					}

					if (interfacePtr->cursorInBounds(minimalBtn, interfacePtr->getMousePos()) && minimalBtn->isEnabled) {
						minimalMode = true;
						themesExitBtn->isEnabled = false;
						minimalBtn->isEnabled = false;
						setBGBtn->isEnabled = false;
						openFileBtn->isEnabled = false;
						setTypographyBtn->isEnabled = false;
						typographyInputBtn->isEnabled = false;
						SDL_SetWindowBordered(window.get(), SDL_FALSE);
						SDL_SetWindowSize(window.get(), 120, 50);
#ifdef _WIN32
						setWindowShadow(hwnd, {0, 0, 0, 1});
#endif
						scenePtr->setScene("Main");
					}

					if (interfacePtr->cursorInBounds(returnBtn, interfacePtr->getMousePos()) && returnBtn->isEnabled && minimalMode) {
						minimalMode = false;
						themesExitBtn->isEnabled = true;
						SDL_SetWindowBordered(window.get(), SDL_TRUE);
						SDL_SetWindowSize(window.get(), windowWidth, windowHeight);
#ifdef _WIN32
						setWindowShadow(hwnd, {0, 0, 0, 0});
#endif
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
									// check if a character is alphabetical
									const bool isAlpha = std::find_if(bgColorText.begin(), bgColorText.end(), isalpha) != bgColorText.end();
									if (isAlpha) {
										SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Background Color Error", "String input contains non-numerical characters!", window.get());
										setBGToColor = false;
										bgColorText = "Set Color";
										break;
									}
									// if there are no spaces, append spaces
									if (!bgColorText.contains(' ')) {
										const auto firstComma = bgColorText.find_first_of(',');
										const auto secondComma = bgColorText.find_last_of(',');
										// the next character in the string
										int nextCharacter = 1;

										bgColorText.insert(firstComma + nextCharacter, 1, ' ');
										bgColorText.insert(secondComma + nextCharacter, 1, ' ');
									}
									// delete all commas in the string
									std::erase(bgColorText, ',');
									// find all of the spaces
									const auto first = bgColorText.find_first_of(' ');
									const auto second = bgColorText.find_last_of(' ');
									// split the string into sections and find their lengths
									const std::array<std::basic_string<char>, 3> strSplit = {
										bgColorText.substr(0, first + 1), // temp fix (increases string count position to get the additional space char)
										bgColorText.substr(first, second - first),
										bgColorText.substr(second, bgColorText.back())
									};
									// check if a section is greater than 3 digits or less than 1 digit
									constexpr int colorSectionMax = 4;
									constexpr int colorSectionMin = 1;
									for (int i = 0; i < strSplit.size(); i++) {
										if (strSplit[i].length() > colorSectionMax || strSplit[i].length() < colorSectionMin) {
											SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Background Color Error", "String input size is invalid!", window.get());
											setBGToColor = false;
											bgColorText = "Set Color";
											goto breakout;
										}
									}
									// get the positions of the colour values and apply them
									redViewColor = std::stoi(bgColorText.substr(0, first));
									greenViewColor = std::stoi(bgColorText.substr(first, second));
									blueViewColor = std::stoi(bgColorText.substr(second, bgColorText.back()));
									// 255 163 210 (demo colour)
								} else if (bgColorText.contains('#')) {
									const char *hexVal = bgColorText.c_str();
									// convert the hex to rgb
									sscanf_s(hexVal, "#%02x%02x%02x", &redViewColor, &greenViewColor, &blueViewColor);
								} else if (!bgColorText.contains(',')) {
									SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Background Color Error", "String input does not contain commas!", window.get());
								}
								bgColorText = "Set Color";
							}

							if (setTypographyIsPressed) {
								// check path given
								if (!typographyInputBtn->text.contains(".ttf")) {
									SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Typography Error", "Font file not found!", window.get());
									typographyInputBtn->text = "Set Font";
								} else {
									typographyStr = dirPath + "assets/" + typographyInputBtn->text;
									typographyInputBtn->text = "Set Font";
								}
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
					if (!(SDL_GetModState() & KMOD_CTRL && (ev.text.text[0] == 'c' || ev.text.text[0] == 'C' ||
												ev.text.text[0] == 'v' || ev.text.text[0] == 'V'))) {
						if (setBGToColor && setBGColorBtn->isEnabled) {
							if (setBGColorBtn->text.contains("Set Color"))
								break;

							setBGColorBtn->text += ev.text.text;
						}

						if (setTypographyIsPressed && setTypographyBtn->isEnabled) {
							if (typographyInputBtn->text.contains("Set Font"))
								break;

							typographyInputBtn->text += ev.text.text;
						}
					}
				} break;

				case SDL_MOUSEMOTION: {
					if (interfacePtr->cursorInBounds(colorPickerBounds, interfacePtr->getMousePos())) {
						themesColorPicker.x = ev.motion.x - 5;
						themesColorPicker.y = ev.motion.y - 5;
						inColorPickerBounds = true;
					} else {
						inColorPickerBounds = false;
					}

					if (interfacePtr->cursorInBounds(colorSliderBounds, interfacePtr->getMousePos())) {
						// move slider base by 5px
						// center
						themesSlider[0].position.y = (float)ev.motion.y;
						// left
						themesSlider[1].position.y = (float)ev.motion.y - 5;
						// right
						themesSlider[2].position.y = (float)ev.motion.y + 5;
						// move slider outline by 7px
						// center
						themesSliderOutline[0].position.y = (float)ev.motion.y;
						// left
						themesSliderOutline[1].position.y = (float)ev.motion.y - 7;
						// right
						themesSliderOutline[2].position.y = (float)ev.motion.y + 7;
					}
				} break;

				breakout:
					break;
			}
			end = std::chrono::steady_clock::now();
			deltaTime = (double)std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
			begin = end;

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
				setThemeBtn->isEnabled = true;
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

			imagePtr->getAnimPtr()->update(37, deltaTime);
			interfacePtr->update(&ev, deltaTime);

			draw();
		}
		free();
	}

	// usually you want this to be independent
	void Anya::draw() {
		SDL_SetRenderDrawBlendMode(renderer.get(), SDL_BLENDMODE_BLEND);
		SDL_SetRenderDrawColor(renderer.get(), 0, 0, 0, 255);
		SDL_RenderClear(renderer.get());

		if (scenePtr->getCurrentScene() == scenePtr->findScene("Main")) {
			timeText = imagePtr->createTextA({timeToStr(std::chrono::system_clock::now()), typographyStr, {{0}, {0}, {255, 255, 255}}, 28}, renderer.get());
			dateText = imagePtr->createTextA({std::format("{:%Ex}", std::chrono::current_zone()->to_local(std::chrono::system_clock::now())), dirPath + "assets/Onest.ttf", {{0}, {0}, {255, 255, 255}}, 16}, renderer.get());
			settingsText = imagePtr->createText({settingsBtn->text, dirPath + "assets/Onest.ttf", settingsBtn->buttonColor, 96}, renderer.get());

			if (setBGToColor) {
				SDL_SetRenderDrawColor(renderer.get(), redViewColor, greenViewColor, blueViewColor, 255);
				SDL_RenderFillRect(renderer.get(), &fillBGColor);
			} else if (setBGtoImg) {
				imagePtr->draw(backgroundImg, renderer.get(), 0, 0);
			} else {
				imagePtr->drawAnimation(backgroundGIF, renderer.get(), 0, 0);
			}

			if (minimalMode) {
				mainQuitText = imagePtr->createText({mainQuitBtn->text, dirPath + "assets/Onest.ttf", mainQuitBtn->buttonColor, 96}, renderer.get());
				minimizeText = imagePtr->createText({minimizeBtn->text, dirPath + "assets/Onest.ttf", minimizeBtn->buttonColor, 96}, renderer.get());

				SDL_SetRenderDrawColor(renderer.get(), redViewColor, greenViewColor, blueViewColor, 255);
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

				interfacePtr->setButtonTextSize(settingsText, 1, 16);
				interfacePtr->draw(settingsBtn, settingsText, renderer.get());
			}
		}

		if (scenePtr->getCurrentScene() == scenePtr->findScene("Settings")) {
			settingsExitText = imagePtr->createText({settingsExitBtn->text, dirPath + "assets/Onest.ttf", settingsExitBtn->buttonColor, 72}, renderer.get());
			themesText = imagePtr->createText({themesBtn->text, dirPath + "assets/Onest.ttf", themesBtn->buttonColor, 32}, renderer.get());
			quitText = imagePtr->createText({settingsQuitBtn->text, dirPath + "assets/Onest.ttf", settingsQuitBtn->buttonColor, 96}, renderer.get());
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
			themesExitText = imagePtr->createText({themesExitBtn->text, dirPath + "assets/Onest.ttf", themesExitBtn->buttonColor, 96}, renderer.get());
			minimalText = imagePtr->createText({minimalBtn->text, dirPath + "assets/Onest.ttf", minimalBtn->buttonColor, 96}, renderer.get());
			setBGText = imagePtr->createText({setBGBtn->text, dirPath + "assets/Onest.ttf", setBGBtn->buttonColor, 96}, renderer.get());
			// brown background colour
			SDL_SetRenderDrawColor(renderer.get(), 26, 17, 16, 255);
			SDL_RenderFillRect(renderer.get(), &settingsThemesView);

			interfacePtr->draw(themesExitBtn, themesExitText, renderer.get());
			interfacePtr->draw(minimalBtn, minimalText, renderer.get());
			interfacePtr->draw(setBGBtn, setBGText, renderer.get());
			interfacePtr->draw(setTypographyBtn, nullptr, renderer.get());
			interfacePtr->draw(setThemeBtn, nullptr, renderer.get());

			if (setTypographyIsPressed) {
				typographyInputText = imagePtr->createText({typographyInputBtn->text, dirPath + "assets/Onest.ttf", openFileBtn->buttonColor, 96}, renderer.get());

				interfacePtr->setButtonTextSize(typographyInputText, -45, 2);
				interfacePtr->draw(typographyInputBtn, typographyInputText, renderer.get());
			}

			if (setBGIsPressed) {
				openFileText = imagePtr->createText({openFileBtn->text, dirPath + "assets/Onest.ttf", openFileBtn->buttonColor, 96}, renderer.get());
				setBGColorText = imagePtr->createText({setBGColorBtn->text, dirPath + "assets/Onest.ttf", setBGColorBtn->buttonColor, 28}, renderer.get());

				interfacePtr->draw(openFileBtn, openFileText, renderer.get());
				interfacePtr->draw(setBGColorBtn, setBGColorText, renderer.get());
			}

			if (setThemeIsPressed) {
				exitThemeCreatorText = imagePtr->createText({exitThemeCreatorBtn->text, dirPath + "assets/Onest.ttf", exitThemeCreatorBtn->buttonColor, 96}, renderer.get());
				themesMenuBGText = imagePtr->createText({setMenuBGBtn->text, dirPath + "assets/Onest.ttf", setMenuBGBtn->buttonColor, 96}, renderer.get());
				themesBGCText = imagePtr->createText({setButtonBGCBtn->text, dirPath + "assets/Onest.ttf", setButtonBGCBtn->buttonColor, 32}, renderer.get());
				themesOCText = imagePtr->createText({setButtonOCBtn->text, dirPath + "assets/Onest.ttf", setButtonOCBtn->buttonColor, 96}, renderer.get());
				themesTCText = imagePtr->createText({setButtonTCBtn->text, dirPath + "assets/Onest.ttf", setButtonTCBtn->buttonColor, 96}, renderer.get());
				buttonColorInputText = imagePtr->createText({buttonColorInputBtn->text, dirPath + "assets/Onest.ttf", buttonColorInputBtn->buttonColor, 100}, renderer.get());

				SDL_Rect paintingScreen = {0, 0, (int)windowWidth, (int)windowHeight};
				SDL_SetRenderDrawColor(renderer.get(), 26, 17, 16, 255);
				SDL_RenderFillRect(renderer.get(), &paintingScreen);

				interfacePtr->draw(exitThemeCreatorBtn, exitThemeCreatorText, renderer.get());
				// menu background colour
				interfacePtr->setButtonTextSize(themesMenuBGText, 0, 5);
				interfacePtr->draw(setMenuBGBtn, themesMenuBGText, renderer.get());
				// button background colour 
				interfacePtr->setButtonTextSize(themesBGCText, -15, 5);
				interfacePtr->draw(setButtonBGCBtn, themesBGCText, renderer.get());
				// button outline colour 
				interfacePtr->setButtonTextSize(themesOCText, -10, 5);
				interfacePtr->draw(setButtonOCBtn, themesOCText, renderer.get());
				// button text colour 
				interfacePtr->setButtonTextSize(themesTCText, -15, 5);
				interfacePtr->draw(setButtonTCBtn, themesTCText, renderer.get());

				// draw a quad
				SDL_Vertex colorPickerQuad[4] = {0};
				// bottom left
				colorPickerQuad[0].position.x = (float)(windowWidth / 2) + 9;
				colorPickerQuad[0].position.y = (float)(windowWidth / 2);
				colorPickerQuad[0].color = {0, 0, 0, 255};
				// top left
				colorPickerQuad[1].position.x = (float)(windowWidth / 2) + 9;
				colorPickerQuad[1].position.y = 0;
				colorPickerQuad[1].color = {255, 255, 255, 255};
				// top right
				colorPickerQuad[2].position.x = (float)(windowWidth / 2) + 80;
				colorPickerQuad[2].position.y = 0;
				colorPickerQuad[2].color = {255, 0, 0, 255};
				// bottom right
				colorPickerQuad[3].position.x = (float)(windowWidth / 2) + 80;
				colorPickerQuad[3].position.y = (float)(windowWidth / 2);
				colorPickerQuad[3].color = {0, 0, 0, 255};

				int colorPickerIndices[] = {0, 1, 2, 0, 2, 3};

				// draw a quad
				SDL_Vertex colorSliderQuad[4] = {0};
				// bottom left
				colorSliderQuad[0].position.x = (float)(windowWidth / 2);
				colorSliderQuad[0].position.y = 89.0f;
				colorSliderQuad[0].color = {0, 0, 0, 255};
				// top left
				colorSliderQuad[1].position.x = (float)(windowWidth / 2);
				colorSliderQuad[1].position.y = 0;
				colorSliderQuad[1].color = {255, 0, 0, 255};
				// top right
				colorSliderQuad[2].position.x = (float)(windowWidth / 2) + 8;
				colorSliderQuad[2].position.y = 0;
				colorSliderQuad[2].color = {255, 0, 0, 255};
				// bottom right
				colorSliderQuad[3].position.x = (float)(windowWidth / 2) + 8;
				colorSliderQuad[3].position.y = 89.0f;
				colorSliderQuad[3].color = {0, 0, 0, 255};

				int colorSliderIndices[] = {0, 1, 2, 0, 2, 3};

				SDL_RenderGeometry(renderer.get(), nullptr, colorSliderQuad, 4, colorSliderIndices, 6);

				interfacePtr->drawDivider({(int)windowWidth / 2, 0, 1, (int)windowHeight}, {240, 209, 189, 255}, renderer.get());
				interfacePtr->drawDivider({(int)(windowWidth / 2) + 8, 0, 1, (int)windowHeight}, {240, 209, 189, 255}, renderer.get());

				SDL_RenderGeometry(renderer.get(), nullptr, themesSliderOutline, 3, nullptr, 0);
				SDL_RenderGeometry(renderer.get(), nullptr, themesSlider, 3, nullptr, 0);
				SDL_RenderGeometry(renderer.get(), nullptr, colorPickerQuad, 4, colorPickerIndices, 6);

				if (inColorPickerBounds) {
					SDL_SetRenderDrawColor(renderer.get(), 255, 255, 255, 255);
					SDL_RenderDrawRect(renderer.get(), &themesColorPicker);
				}

				interfacePtr->setButtonTextSize(buttonColorInputText, -30, 5);
				interfacePtr->draw(buttonColorInputBtn, buttonColorInputText, renderer.get());
			}
		}

		SDL_RenderPresent(renderer.get());

		if (deltaTime < delay)
			SDL_Delay(delay - static_cast<int>(deltaTime));
	}

	void Anya::free() {
		std::cout << "releasing allocated resources..\n";
		SDL_StopTextInput();
		NFD_Quit();
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
} // namespace Application