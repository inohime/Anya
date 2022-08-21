#pragma once

#include <SDL.h>
#include "image.hpp"
#include "uinterface.hpp"
#include "util.hpp"
#include "scene.hpp"
#include <chrono>
#include <format>
#ifdef _WIN32
    #define WIN32_LEAN_AND_MEAN
    #include <dwmapi.h>
    #include <windows.h>
    #include <shellapi.h>
#endif

// low memory | low cpu utilization app (not the lowest since added features and no optimizations)
// at its current state, it takes 3.6mb on debug, too high. (11mb with gif alloc)

namespace Application {
    using namespace Helper::Utils;

    class Anya final {
    public:
        Anya();

        std::basic_string<char> timeToStr(const std::chrono::system_clock::time_point &time);
        bool boot();
        void update();
        void draw();
        void free();

    private:
        // window data
        std::basic_string<char> title {"anya"};
        std::basic_string<char> errStr;
        SMD<SDL_Window> window {nullptr};
        SMD<SDL_Renderer> renderer {nullptr};
        SDL_Event ev;
        bool shouldRun {false};
        uint32_t windowWidth {148};
        uint32_t windowHeight {89};
        int minWindowWidth {120};
        int minWindowHeight {50};
        std::chrono::steady_clock::time_point begin;
        std::chrono::steady_clock::time_point end;
        double deltaTime;
        int FPS {30};
        const int delay {1000 / FPS};

    private:
        std::unique_ptr<Helper::UInterface> interfacePtr {nullptr};
        std::unique_ptr<Helper::Image> imagePtr {nullptr};
        std::unique_ptr<Helper::Scene> scenePtr {nullptr};
        // directory path
        std::basic_string<char> dirPath;
        std::basic_string<char> typographyStr;
        // set background colour
        int redViewColor {0};
        int greenViewColor {0};
        int blueViewColor {0};

        bool setTypographyIsPressed {false};
        // setBG button on/off
        bool setBGIsPressed {false};
        // setBGColor button on/off
        bool setBGToColor {false};
        bool setBGtoImg {false};
        bool minimalMode {false};
        bool showDate {false};
        bool setThemeIsPressed {false};
        bool inColorPickerBounds {false};

        // scene view separators
        SDL_Rect settingsView;
        SDL_Rect settingsThemesView;
        SDL_Rect fillBGColor;
        // add an colour alpha slider
        SDL_Rect colorPickerBounds;
        SDL_Rect colorSliderBounds;
        SDL_Rect themesColorPicker;
        // replace with non-filled circle
        SDL_Vertex themesSlider[3];
        SDL_Vertex themesSliderOutline[3];

#ifdef _WIN32
        HWND hwnd;
#endif

        Helper::IMD backgroundGIF {nullptr};
        Helper::IMD backgroundImg {nullptr};
        Helper::IMD githubImg {nullptr};
        Helper::IMD calendarImg {nullptr};
        Helper::IMD typographyImg {nullptr};
        Helper::IMD returnImg {nullptr};
        Helper::IMD setThemeImg {nullptr};
        // text
        Helper::IMD timeText {nullptr};
        Helper::IMD dateText {nullptr};
        Helper::IMD settingsText {nullptr};
        Helper::IMD mainQuitText {nullptr};
        Helper::IMD minimizeText {nullptr};
        Helper::IMD quitText {nullptr};
        Helper::IMD settingsExitText {nullptr};
        Helper::IMD themesText {nullptr};
        Helper::IMD themesExitText {nullptr};
        // Helper::IMD setLayoutText {nullptr};
        Helper::IMD minimalText {nullptr};
        Helper::IMD setBGText {nullptr};
        Helper::IMD openFileText {nullptr};
        Helper::IMD bgColorInputText {nullptr};
        Helper::IMD typographyInputText {nullptr};

        ////////////////////////////////
        // test for button/menu theming
        Helper::IMD themesMenuBGText {nullptr};
        Helper::IMD themesBGCText {nullptr};
        Helper::IMD themesOCText {nullptr};
        Helper::IMD themesTCText {nullptr};
        Helper::IMD buttonColorInputText {nullptr};
        Helper::IMD exitThemeCreatorText {nullptr};
        ////////////////////////////////

        // buttons
        Helper::BUTTONPTR settingsBtn {nullptr};
        Helper::BUTTONPTR mainQuitBtn {nullptr};
        Helper::BUTTONPTR minimizeBtn {nullptr};
        Helper::BUTTONPTR returnBtn {nullptr};
        Helper::BUTTONPTR settingsQuitBtn {nullptr};
        Helper::BUTTONPTR settingsExitBtn {nullptr};
        Helper::BUTTONPTR themesBtn {nullptr};
        Helper::BUTTONPTR githubBtn {nullptr};
        Helper::BUTTONPTR calendarBtn {nullptr};
        // Helper::BUTTONPTR setLayoutBtn {nullptr};
        Helper::BUTTONPTR themesExitBtn {nullptr};
        Helper::BUTTONPTR minimalBtn {nullptr};
        Helper::BUTTONPTR setBGBtn {nullptr};
        Helper::BUTTONPTR openFileBtn {nullptr};
        Helper::BUTTONPTR bgColorInputBtn {nullptr};
        Helper::BUTTONPTR setTypographyBtn {nullptr};
        Helper::BUTTONPTR typographyInputBtn {nullptr};
        Helper::BUTTONPTR setThemeBtn {nullptr};

        ////////////////////////////////
        // test for button/menu theming
        Helper::BUTTONPTR setMenuBGBtn {nullptr};
        Helper::BUTTONPTR setButtonBGCBtn {nullptr};
        Helper::BUTTONPTR setButtonOCBtn {nullptr};
        Helper::BUTTONPTR setButtonTCBtn {nullptr};
        Helper::BUTTONPTR exitThemeCreatorBtn {nullptr};
        // set the enter key to submit the value based on the button selected
        Helper::BUTTONPTR buttonColorInputBtn {nullptr};
        ////////////////////////////////
    };
} // namespace Application
