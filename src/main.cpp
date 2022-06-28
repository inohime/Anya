//#include "shared.hpp"
#include "anya.hpp"

// updated 6/25
// x-platform compatible
// added more features

// turn this into a lambda func
/*static std::basic_string<char> getTime(time_t clock) {
	return Anya(clock).getStream()->str();
}*/

using namespace Application;
int main(int, char **)
{
	auto inst = Anya();

	return 0;
}
	

// no, I'm not making this easier on the eyes, it only took 30 mins
/*
int main(int, char **)
{
	SDL_assert(SDL_Init(SDL_INIT_EVERYTHING) == 0);
	SDL_assert(IMG_Init(IMG_INIT_PNG) != 0);
	if (TTF_Init() == -1)
		return -1;

	constexpr int windowWidth = 148, windowHeight = 89;
	constexpr int FPS = 8;
	constexpr int delay = 1000 / FPS;

	auto window = PTR<SDL_Window>(SDL_CreateWindow("Anya", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, windowWidth, windowHeight, 0));
	auto renderer = PTR<SDL_Renderer>(SDL_CreateRenderer(window.get(), -1, SDL_RENDERER_SOFTWARE));

	char *const base = SDL_GetBasePath();
	const std::basic_string<char> &basePath = base;
	SDL_free(base);

	SDL_Texture *image = loadTexture(basePath + "assets/beep_1.png", renderer.get());
	SDL_Texture *msg = nullptr;
	SDL_Event ev;

	// create buttons here
	// add animation for gifs

	auto start = std::chrono::steady_clock::now();

	bool quit = false;
	while (!quit) {
		time_t sysClock = std::time(nullptr);

		SDL_PollEvent(&ev);
		switch (ev.type) {
			case SDL_QUIT:
				quit = true;
				break;
		}
		const auto end = std::chrono::steady_clock::now();
		const auto dt = std::chrono::duration<double, std::milli>(end - start);
		start = end;

		msg = loadTextOutline(getTime(sysClock), renderer.get(), basePath + "assets/bahnschrift.ttf", { 255, 255, 255, 255 }, 28);

		SDL_SetRenderDrawColor(renderer.get(), 0, 0, 0, 255);
		SDL_RenderClear(renderer.get());

		drawTexture(image, renderer.get(), 0, 0);
		drawTexture(msg, renderer.get(), (int)windowWidth / 5, (int)windowHeight / 1.6);

		SDL_RenderPresent(renderer.get());

		if (delay > dt.count())
			SDL_Delay(delay - dt.count());

		SDL_DestroyTexture(msg);
	}
	SDL_DestroyTexture(msg);
	SDL_DestroyTexture(image);

	TTF_Quit();
	IMG_Quit();
	SDL_Quit();

	return 0;
}
*/