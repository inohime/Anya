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

		// load assets
		background = imagePtr->createImage(path + "assets/beep_1.png", renderer.get());
		backgroundGIF = imagePtr->createPack("canvas", path + "assets/gif-extract/", renderer.get());
		imagePtr->getAnimPtr()->addAnimation(68, 0, 0, 148, 89);
		// create buttons here (add textures)
		//testButton = interfacePtr->createButton("hello", background, {255, 0, 0, 255}, 0, 0, 50, 50);
		//testButton->boxOutline = true;
		closeBtn = interfacePtr->createButton("x", nullptr, {255, 255, 255, 255}, 120, 0, 25, 25);
		//interfacePtr->setButtonFlag(closeBtn, false);
		closeBtn->showOutline = true;
		closeBtn->canQuit = true;

		settingsBtn = interfacePtr->createButton("O", nullptr, {255, 255, 255, 255}, 0, 0, 25, 25);
		//interfacePtr->setButtonFlag(settingsBtn, false);
		settingsBtn->showOutline = true;
	
		minimizeBtn = interfacePtr->createButton("-", nullptr, {255, 255, 255, 255}, 80, 0, 25, 25);
		//interfacePtr->setButtonFlag(minimizeBtn, true);
		minimizeBtn->showOutline = true;
		minimizeBtn->canMinimize = true;
		

		/** The UI design:
		* the application on start will display a clock running and a default image of anya
		* in the top right corner, there will be a minimize and close button
		* in the top left corner, there will be a settings cog button to modify the clock:
		* - X button to close settings
		* - Themes
		*	- Back arrow to settings | X button to close settings
		*	- Minimal Mode (Minimal)
		*	- Set Background (Background) -> expands -> Open File | Color (with color icon filled)
		*	- Set Text Font (Font)
		* - Minimize (window should be borderless)
		* - Quit
		*
		* the cog, min, and quit buttons should be transparent on window unfocused, otherwise they should be 25% transparent
		*	- on hover, they should be opaque (smoothed transition in/out)
		* Screen should fade to next scene (fast)
		* 
		* due to me not wanting to do this anymore, window will be bordered 
		* (if you'd like it to be borderless and have dragging + window shadow, go to my github/SDL2_Aero)
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
			imagePtr->getAnimPtr()->update(37, deltaTime.count());
			interfacePtr->update(&ev);

			draw();
		}
		free();
	}

	// usually you want this to be independent
	void Anya::draw() {
		msg = imagePtr->createTextA({timeToStr(std::chrono::system_clock::now()), path + "assets/bahnschrift.ttf", {255, 255, 255, 255}, 28}, renderer.get());
		//buttonText = imagePtr->createTextA({testButton->text, path + "assets/bahnschrift.ttf", {255, 255, 255, 255}, 16}, renderer.get());
		settingsText = imagePtr->createText({settingsBtn->text, path + "assets/bahnschrift.ttf", {255, 255, 255, 255}, 96}, renderer.get());
		minimizeText = imagePtr->createText({minimizeBtn->text, path + "assets/bahnschrift.ttf", {255, 255, 255, 255}, 96}, renderer.get());
		closeText = imagePtr->createText({closeBtn->text, path + "assets/bahnschrift.ttf", {255, 255, 255, 255}, 96}, renderer.get());

		SDL_SetRenderDrawColor(renderer.get(), 10, 10, 25, 255);
		SDL_RenderClear(renderer.get());

		//imagePtr->draw(background, renderer.get(), 0, 0); // make buttons to change backgrounds
		imagePtr->drawAnimation(backgroundGIF, renderer.get(), 0, 0);
		imagePtr->draw(msg, renderer.get(), static_cast<int>(windowWidth / 5.5), static_cast<int>(windowHeight / 1.6));

		//interfacePtr->draw(testButton, buttonText, renderer.get(), testButton->box.x, testButton->box.y);

		interfacePtr->draw(settingsBtn, settingsText, renderer.get());
		interfacePtr->draw(minimizeBtn, minimizeText, renderer.get());
		interfacePtr->draw(closeBtn, closeText, renderer.get());

		// let's create our ui now 

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

		if (std::chrono::is_pm(hour))
			return std::format("{:%OI:%M}PM", std::chrono::current_zone()->to_local(time));
		else
			return std::format("{:%OI:%M}AM", std::chrono::current_zone()->to_local(time));
	}

	std::unique_ptr<std::basic_stringstream<char>> Anya::getStream() {
		auto ptr = std::make_unique<std::basic_stringstream<char>>(std::move(str));
		return ptr;
	}
} // namespace Application