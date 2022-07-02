#pragma once

#include <SDL.h>
#include <string>
#include <vector>

namespace Application::Helper {
	class Scene {
	public:
		~Scene() { sceneList.clear(); }

		void createScene(std::string_view name);
		void setScene(std::string_view name);
		constexpr uint64_t getCurrentScene();
		constexpr uint64_t findScene(std::string_view name);

	private:
		uint64_t currentScene;
		std::vector<std::basic_string<char>> sceneList;
	};
} // namespace Application::Helper

#include "scene.inl"