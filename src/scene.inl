#include "scene.hpp"
#include "util.hpp"

using namespace Application::Helper::Utils;

namespace Application::Helper {
	__forceinline constexpr void Scene::createScene(std::string_view name) {
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

	__forceinline constexpr void Scene::printScene() {
		println(sceneList[currentScene]);
	}

	__forceinline constexpr uint64_t Scene::getCurrentScene() {
		return currentScene;
	}

	__forceinline constexpr std::string_view Scene::getCurrentSceneName() {
		return sceneList[currentScene];
	}

	inline constexpr uint64_t Scene::findScene(std::string_view name) {
		const auto it = std::find(sceneList.begin(), sceneList.end(), name);
		const auto sceneIndex = it - sceneList.begin();

		return sceneIndex;
	}
} // namespace Application::Helper