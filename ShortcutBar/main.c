#include <stdio.h>
#include <SDL.h>
#include <SDL_ttf.h>
#include <windows.h>
#include <stdbool.h>
#include <string.h>
#include <SDL_syswm.h>

#define HOTKEY_ID 1
#define WINDOW_WIDTH 500
#define WINDOW_HEIGHT 50

typedef struct {
    const char* alias;
    const char* path;
} Command;

static const Command COMMANDS[] = {
    {"gl", "https://www.google.com"},
    {"yt", "https://www.youtube.com"},
    {"gdt", "C:\\Users\\User\\AppData\\Roaming\\Microsoft\\Windows\\Start_Menu\\Programs\\Godot.exe"},
    {"cmd", "cmd.exe"},
    {"clion", "C:\\Users\\User\\C Stuff\\CLion 2026.1.1\\bin\\clion64.exe"}, // Fixed extra quote
    {"exp", "explorer.exe"},
    {"bnd", "C:\\Users\\User\\AppData\\Roaming\\Microsoft\\Windows\\Start Menu\\Programs\\Chrome Apps\\BandLab.lnk "},
    {"clr", "shell:::{3080F90D-D7AD-11D9-BD98-0000947B0257}"}
};
static const int CommandCount = sizeof(COMMANDS) / sizeof(COMMANDS[0]);

void execute_alias(const char* input) {
    for (int i = 0; i < CommandCount; i++) {
        if (strcmp(input, COMMANDS[i].alias) == 0) {
            ShellExecuteA(NULL, "open", COMMANDS[i].path, NULL, NULL, SW_SHOWNORMAL);
            return;
        }

    }
    if (strcmp(input, "killbar") == 0) {
        ShellExecuteA(NULL, "runas", "powershell.exe",
            "-Command \"Stop-Process -Name 'ShortcutBar' -Force\"", NULL, SW_SHOWNORMAL);
    }
    else if (strcmp(input, "killnumlock") == 0) {
        ShellExecuteA(NULL, "runas", "powershell.exe",
            "-Command \"Stop-Process -Name 'NumLock' -Force\"", NULL, SW_SHOWNORMAL);
    }
    else if (strcmp(input, "powershell") == 0) {
        ShellExecuteA(NULL, "runas", "powershell.exe",
            NULL, NULL, SW_SHOWNORMAL);
    }
}

int main(int argc, char* argv[]) {
    SDL_Init(SDL_INIT_VIDEO);
    TTF_Init();

    SDL_Window *window = SDL_CreateWindow(
        "QuickLauncher",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH, WINDOW_HEIGHT,
        SDL_WINDOW_BORDERLESS | SDL_WINDOW_HIDDEN | SDL_WINDOW_ALWAYS_ON_TOP
    );
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    TTF_Font *font = TTF_OpenFont("C:\\Windows\\Fonts\\arial.ttf", 18);

    HWND hwnd = NULL;
    SDL_SysWMinfo wmInfo;
    SDL_VERSION(&wmInfo.version);

    if (SDL_GetWindowWMInfo(window, &wmInfo)) {
        hwnd = wmInfo.info.win.window;
    }

    RegisterHotKey(hwnd, HOTKEY_ID, MOD_ALT, VK_SPACE);

    char input_buffer[256] = "";
    bool running = true;
    bool is_visible = false;

    SDL_StartTextInput();

    while (running) {
        // 1. Listen for Win32 Global Hotkey
        MSG msg;
        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_HOTKEY && msg.wParam == HOTKEY_ID) {
                is_visible = !is_visible;
                if (is_visible) {
                    SDL_ShowWindow(window);
                    SDL_RaiseWindow(window);
                }
                else {
                    SDL_HideWindow(window);
                }
            }
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        // 2. Poll SDL Events
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                running = false;
            }
            else if (is_visible) {
                if (e.type == SDL_TEXTINPUT) {
                    strcat(input_buffer, e.text.text);
                }
                else if (e.type == SDL_KEYDOWN) {
                    if (e.key.keysym.sym == SDLK_BACKSPACE && strlen(input_buffer) > 0) {
                        input_buffer[strlen(input_buffer) - 1] = '\0';
                    }
                    else if (e.key.keysym.sym == SDLK_RETURN) {
                        execute_alias(input_buffer);
                        input_buffer[0] = '\0';
                        is_visible = false;
                        SDL_HideWindow(window);
                    }
                    else if (e.key.keysym.sym == SDLK_ESCAPE) {
                        is_visible = false;
                        SDL_HideWindow(window);
                    }
                }
            }
        }

        // 3. Render Bar & Text
        if (is_visible) {
            SDL_SetRenderDrawColor(renderer, 15, 15, 15, 255);
            SDL_RenderClear(renderer);
            SDL_SetRenderDrawColor(renderer, 70, 70, 70, 255);
            SDL_Rect border = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT};
            SDL_RenderDrawRect(renderer, &border);

            if (strlen(input_buffer) > 0 && font) {
                SDL_Color TextColor = {255, 255, 255, 255};
                SDL_Surface* textsurface = TTF_RenderText_Blended(font, input_buffer, TextColor);
                SDL_Texture* texttexture = SDL_CreateTextureFromSurface(renderer, textsurface);

                SDL_Rect textRect = {15, (WINDOW_HEIGHT - textsurface->h) / 2, textsurface->w, textsurface->h};
                SDL_RenderCopy(renderer, texttexture, NULL, &textRect);

                SDL_FreeSurface(textsurface);
                SDL_DestroyTexture(texttexture);
            }
            SDL_RenderPresent(renderer);
        }

        SDL_Delay(16);
    }

    // Cleanup
    UnregisterHotKey(hwnd, HOTKEY_ID);
    TTF_CloseFont(font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();

    return 0;
}