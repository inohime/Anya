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
		SDL_assert(IMG_Init(IMG_INIT_PNG) != 0);
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
		ptr = std::make_unique<Helper::Image>();

		// load assets
		image = ptr->create(path + "assets/beep_1.png", renderer.get());
		
		// create buttons here
		// add animation for gifs
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
			draw();
		}
		free();
	}

	// usually you want this to be independent
	void Anya::draw() {
		msg = ptr->create({timeToStr(std::chrono::system_clock::now()), path + "assets/bahnschrift.ttf", {255, 255, 255, 255}, 28}, renderer.get());

		SDL_SetRenderDrawColor(renderer.get(), 10, 10, 25, 255);
		SDL_RenderClear(renderer.get());
		
		ptr->draw(image, renderer.get(), 0, 0);
		ptr->draw(msg, renderer.get(), static_cast<int>(windowWidth / 5.5), static_cast<int>(windowHeight / 1.6));

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