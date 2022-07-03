#include <iostream>

namespace Application::Helper {
	inline void Scene::createScene(std::string_view name) {
		sceneList.emplace_back(name);
	}

	inline void Scene::setScene(std::string_view name) {
		const auto it = std::find(sceneList.begin(), sceneList.end(), name);
		const auto sceneIndex = it - sceneList.begin();
		currentScene = sceneIndex;

		//std::cout << "Current Scene: " << currentScene << ", " << name << '\n';
	}

	inline constexpr uint64_t Scene::getCurrentScene() {
		return currentScene;
	}

	inline constexpr uint64_t Scene::findScene(std::string_view name) {
		const auto it = std::find(sceneList.begin(), sceneList.end(), name);
		const auto sceneIndex = it - sceneList.begin();

		return sceneIndex;
	}
} // namespace Application::Helper