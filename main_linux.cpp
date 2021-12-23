
// Dear ImGui: standalone example application for SDL2 + OpenGL
// (SDL is a cross-platform general purpose library for handling windows, inputs, OpenGL/Vulkan/Metal graphics context creation, etc.)
// If you are new to Dear ImGui, read documentation from the docs/ folder + read the top of imgui.cpp.
// Read online: https://github.com/ocornut/imgui/tree/master/docs

#include "imgui/imgui.h"
#include "imgui/backends/imgui_impl_sdl.h"
#include "imgui/backends/imgui_impl_opengl3.h"
#include <stdio.h>
#include "SDL.h"
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <SDL_opengles2.h>
#else
#include <SDL_opengl.h>
#endif

#include "MegaBoyDebugger.h"


#define MOOSEPIC_W 64
#define MOOSEPIC_H 88

#define MOOSEFRAME_SIZE (MOOSEPIC_W * MOOSEPIC_H)
#define MOOSEFRAMES_COUNT 10

SDL_Color MooseColors[84] = {
        {49, 49, 49, 255}, {66, 24, 0, 255}, {66, 33, 0, 255}, {66, 66, 66, 255},
        {66, 115, 49, 255}, {74, 33, 0, 255}, {74, 41, 16, 255}, {82, 33, 8, 255},
        {82, 41, 8, 255}, {82, 49, 16, 255}, {82, 82, 82, 255}, {90, 41, 8, 255},
        {90, 41, 16, 255}, {90, 57, 24, 255}, {99, 49, 16, 255}, {99, 66, 24, 255},
        {99, 66, 33, 255}, {99, 74, 33, 255}, {107, 57, 24, 255}, {107, 82, 41, 255},
        {115, 57, 33, 255}, {115, 66, 33, 255}, {115, 66, 41, 255}, {115, 74, 0, 255},
        {115, 90, 49, 255}, {115, 115, 115, 255}, {123, 82, 0, 255}, {123, 99, 57, 255},
        {132, 66, 41, 255}, {132, 74, 41, 255}, {132, 90, 8, 255}, {132, 99, 33, 255},
        {132, 99, 66, 255}, {132, 107, 66, 255}, {140, 74, 49, 255}, {140, 99, 16, 255},
        {140, 107, 74, 255}, {140, 115, 74, 255}, {148, 107, 24, 255}, {148, 115, 82, 255},
        {148, 123, 74, 255}, {148, 123, 90, 255}, {156, 115, 33, 255}, {156, 115, 90, 255},
        {156, 123, 82, 255}, {156, 132, 82, 255}, {156, 132, 99, 255}, {156, 156, 156, 255},
        {165, 123, 49, 255}, {165, 123, 90, 255}, {165, 132, 82, 255}, {165, 132, 90, 255},
        {165, 132, 99, 255}, {165, 140, 90, 255}, {173, 132, 57, 255}, {173, 132, 99, 255},
        {173, 140, 107, 255}, {173, 140, 115, 255}, {173, 148, 99, 255}, {173, 173, 173, 255},
        {181, 140, 74, 255}, {181, 148, 115, 255}, {181, 148, 123, 255}, {181, 156, 107, 255},
        {189, 148, 123, 255}, {189, 156, 82, 255}, {189, 156, 123, 255}, {189, 156, 132, 255},
        {189, 189, 189, 255}, {198, 156, 123, 255}, {198, 165, 132, 255}, {206, 165, 99, 255},
        {206, 165, 132, 255}, {206, 173, 140, 255}, {206, 206, 206, 255}, {214, 173, 115, 255},
        {214, 173, 140, 255}, {222, 181, 148, 255}, {222, 189, 132, 255}, {222, 189, 156, 255},
        {222, 222, 222, 255}, {231, 198, 165, 255}, {231, 231, 231, 255}, {239, 206, 173, 255}
};

Uint8 MooseFrames[MOOSEFRAMES_COUNT][MOOSEFRAME_SIZE];

SDL_Renderer *renderer;
int frame;
SDL_Texture *MooseTexture;

void UpdateTexture(SDL_Texture *texture, int frame)
{
    SDL_Color *color;
    Uint8 *src;
    Uint32 *dst;
    int row, col;
    void *pixels;
    int pitch;

    if (SDL_LockTexture(texture, NULL, &pixels, &pitch) < 0) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't lock texture: %s\n", SDL_GetError());
    }
    src = MooseFrames[frame];
    for (row = 0; row < MOOSEPIC_H; ++row) {
        dst = (Uint32*)((Uint8*)pixels + row * pitch);
        for (col = 0; col < MOOSEPIC_W; ++col) {
            color = &MooseColors[*src++];
            *dst++ = (0xFF000000|(color->r<<16)|(color->g<<8)|color->b);
        }
    }
    SDL_UnlockTexture(texture);
}

// Main code
int main(int, char**)
{
    // Setup SDL
    // (Some versions of SDL before <2.0.10 appears to have performance/stalling issues on a minority of Windows systems,
    // depending on whether SDL_INIT_GAMECONTROLLER is enabled or disabled.. updating to latest version of SDL is recommended!)
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0)
    {
        printf("Error: %s\n", SDL_GetError());
        return -1;
    }

    // Decide GL+GLSL versions
#if defined(IMGUI_IMPL_OPENGL_ES2)
    // GL ES 2.0 + GLSL 100
    const char* glsl_version = "#version 100";
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#elif defined(__APPLE__)
    // GL 3.2 Core + GLSL 150
    const char* glsl_version = "#version 150";
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG); // Always required on Mac
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
#else
    // GL 3.0 + GLSL 130
    const char* glsl_version = "#version 130";
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#endif

    // Create window with graphics context
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
    SDL_Window* window = SDL_CreateWindow("MegaBoy debugger", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, window_flags);
    SDL_GLContext gl_context = SDL_GL_CreateContext(window);
    SDL_GL_MakeCurrent(window, gl_context);
    SDL_GL_SetSwapInterval(1); // Enable vsync

    //renderer = SDL_GetRenderer(window);
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();

    // Setup Platform/Renderer backends
    ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
    ImGui_ImplOpenGL3_Init(glsl_version);
/*
    int oglIdx = -1;
    int nRD = SDL_GetNumRenderDrivers();
    for(int i=0; i<nRD; i++)
    {
        SDL_RendererInfo info;
        if(!SDL_GetRenderDriverInfo(i, &info))
        {
            if(!strcmp(info.name, "opengl"))
            {
                oglIdx = i;
            }
        }
    }

    renderer = SDL_CreateRenderer(window, 2, 0);
    if (!renderer) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL Error: %s\n", SDL_GetError());
    }
    */
/*
    MooseTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, MOOSEPIC_W, MOOSEPIC_H);
    if (!MooseTexture) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't set create texture: %s\n", SDL_GetError());
    }
*/

    // Load Fonts
    // - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
    // - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
    // - If the file cannot be loaded, the function will return NULL. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
    // - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
    // - Read 'docs/FONTS.md' for more instructions and details.
    // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
    //io.Fonts->AddFontDefault();
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/ProggyTiny.ttf", 10.0f);
    //ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
    //IM_ASSERT(font != NULL);

    // Our state
    bool show_demo_window = true;
    bool show_another_window = false;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    MegaBoyDebugger debugger;
    debugger.LoadTestRom();

    // Create a texture
    GLuint textureId;
    glGenTextures(1, &textureId);


    glBindTexture(GL_TEXTURE_2D, textureId);
    // Setup filtering parameters for display
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // This is required on WebGL for non power-of-two textures
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // Same
    glTexImage2D(GL_TEXTURE_2D, 0, 3, MegaBoyDebugger::GB_SCREEN_WIDTH, MegaBoyDebugger::GB_SCREEN_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, (GLvoid*)debugger.screenData);




    // Main loop
    bool done = false;
    while (!done)
    {
        // Poll and handle events (inputs, window resize, etc.)
        // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
        // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
        // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
        // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            ImGui_ImplSDL2_ProcessEvent(&event);
            if (event.type == SDL_QUIT)
                done = true;
            if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(window))
                done = true;
        }

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

        debugger.UpdateUI();

        // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
        if (show_demo_window)
            ImGui::ShowDemoWindow(&show_demo_window);

        // 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
        {
            static float f = 0.0f;
            static int counter = 0;

            ImGui::Begin("Gameboy screen:");                          // Create a window called "Hello, world!" and append into it.

            // Update Texture
            glTexSubImage2D(GL_TEXTURE_2D, 0 ,0, 0, MegaBoyDebugger::GB_SCREEN_WIDTH, MegaBoyDebugger::GB_SCREEN_HEIGHT, GL_RGB, GL_UNSIGNED_BYTE, (GLvoid*)debugger.screenData);

            ImGui::Image((void*)(intptr_t)textureId, ImVec2(MegaBoyDebugger::GB_SCREEN_WIDTH*4, MegaBoyDebugger::GB_SCREEN_HEIGHT*4));

            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            ImGui::End();
        }

        // Rendering
        ImGui::Render();
        glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
        glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);


        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        frame = (frame + 1) % MOOSEFRAMES_COUNT;
        //UpdateTexture(MooseTexture, frame);
        //SDL_RenderClear(renderer);
        //SDL_RenderCopy(renderer, MooseTexture, NULL, NULL);

        SDL_GL_SwapWindow(window);
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_GL_DeleteContext(gl_context);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
