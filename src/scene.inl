#include "scene.hpp"

namespace Application::Helper {
	inline constexpr void Scene::createScene(std::string_view name) {
		sceneList.emplace_back(name);
	}

	inline constexpr void Scene::setScene(std::string_view name) {
		const auto it = std::find(sceneList.begin(), sceneList.end(), name);
		const auto sceneIndex = it - sceneList.begin();

		currentScene = sceneIndex;

#ifdef _DEBUG
		printScene();
#endif
	}

	inline constexpr void Scene::printScene() {
		Utils::println(sceneList[currentScene]);
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