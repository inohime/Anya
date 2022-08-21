#pragma once

#include <SDL.h>
#include <string>
#include <vector>

namespace Application::Helper {
    class Scene {
    public:
        /* Scene Destructor; clears out any scenes in the array when the object is destroyed.
         */
        constexpr ~Scene() { sceneList.clear(); }
        /** Creates new scenes to use as layers.
         *
         * \param name -> name of the scene to be created
         */
        constexpr void createScene(std::string_view name);
        /** Sets a created scene to be used as the current layer.
         *
         * \param name -> name of the scene to be set as current
         */
        constexpr void setScene(std::string_view name);
        /* Prints the current scene being shown, to the console.
         */
        constexpr void printScene();
        /** Retrieves the current scene being shown.
         *
         * \return the current scene number.
         */
        constexpr uint64_t getCurrentScene();
        /** Retrieves the current scene being shown.
         *
         * \return the current scene name.
         */
        constexpr std::string_view getCurrentSceneName();
        /** Finds and returns the index of the scene being looked for.
         *  This is primarily used to be compared against `getCurrentScene`.
         *
         *  Example: getCurrentScene() == findScene("Menu")
         *
         * \param name -> name of the scene to find
         * \return the scene index of the scene name.
         */
        constexpr uint64_t findScene(std::string_view name);

    private:
        uint64_t currentScene;
        std::vector<std::basic_string<char>> sceneList;
    };
} // namespace Application::Helper

#include "scene.inl"