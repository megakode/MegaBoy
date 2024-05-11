#include "imgui/imgui.h"
#include "imgui/backends/imgui_impl_sdl.h"
#include "imgui/backends/imgui_impl_metal.h"
#include <stdio.h>
#include <SDL.h>
#include "UI/UIConfig.h"
#include "MegaBoyDebugger.h"

#import <Metal/Metal.h>
#import <QuartzCore/QuartzCore.h>
#import <Cocoa/Cocoa.h>

MegaBoyDebugger debugger;

void showOpenFileDialog()
{
    NSOpenPanel* openPanel = [NSOpenPanel openPanel];
    [openPanel setCanChooseFiles:YES];
    [openPanel setCanChooseDirectories:YES];
    
    if ([openPanel runModal] == NSModalResponseOK) {
        NSURL* selectedURL = [openPanel URL];
        NSLog(@"Selected File: %@", [selectedURL path]);

        // Convert NSString to std::filesystem::path
        std::string strPath = [[selectedURL path] UTF8String];
        std::filesystem::path path(strPath);
    
        debugger.LoadRom(path);
    }
}

int main(int, char**)
{
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;   // Enable Gamepad Controls

    // Setup style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();

    // Load Fonts
    // - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
    // - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
    // - If the file cannot be loaded, the function will return NULL. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
    // - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
    // - Read 'docs/FONTS.txt' for more instructions and details.
    // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
    //io.Fonts->AddFontDefault();
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/ProggyTiny.ttf", 10.0f);
    //ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
    //IM_ASSERT(font != NULL);

    // Setup SDL
    // (Some versions of SDL before <2.0.10 appears to have performance/stalling issues on a minority of Windows systems,
    // depending on whether SDL_INIT_GAMECONTROLLER is enabled or disabled.. updating to latest version of SDL is recommended!)
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0)
    {
        printf("Error: %s\n", SDL_GetError());
        return -1;
    }

    // Inform SDL that we will be using metal for rendering. Without this hint initialization of metal renderer may fail.
    SDL_SetHint(SDL_HINT_RENDER_DRIVER, "metal");

    SDL_Window* window = SDL_CreateWindow("MegaBoy debugger", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
    if (window == NULL)
    {
        printf("Error creating window: %s\n", SDL_GetError());
        return -2;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (renderer == NULL)
    {
        printf("Error creating renderer: %s\n", SDL_GetError());
        return -3;
    }

     debugger.onOpenFile = [&](){
        showOpenFileDialog();
     };
     // TODO: if command line parameters, load rom
     //debugger.LoadRom();

    // Setup Platform/Renderer backends
    CAMetalLayer* layer = (__bridge CAMetalLayer*)SDL_RenderGetMetalLayer(renderer);
    layer.pixelFormat = MTLPixelFormatBGRA8Unorm;
    ImGui_ImplMetal_Init(layer.device);
    ImGui_ImplSDL2_InitForMetal(window);

    id<MTLCommandQueue> commandQueue = [layer.device newCommandQueue];
    MTLRenderPassDescriptor* renderPassDescriptor = [MTLRenderPassDescriptor new];

    // ImGuiIO& io = ImGui::GetIO();
    // io.Fonts->TexID = (__bridge void *)g_sharedMetalContext.fontTexture; // ImTextureID == void*
    MTLTextureDescriptor *textureDescriptor = [MTLTextureDescriptor texture2DDescriptorWithPixelFormat:MTLPixelFormatRGBA8Unorm
                                                                                                 width:MegaBoyDebugger::GB_SCREEN_WIDTH
                                                                                                height:MegaBoyDebugger::GB_SCREEN_HEIGHT
                                                                                             mipmapped:NO];
    textureDescriptor.usage = MTLTextureUsageShaderRead;
#if TARGET_OS_OSX
    textureDescriptor.storageMode = MTLStorageModeManaged;
#else
    textureDescriptor.storageMode = MTLStorageModeShared;
#endif
    id <MTLTexture> texture = [layer.device newTextureWithDescriptor:textureDescriptor];

    // Our state
    bool show_demo_window = true;
    bool show_another_window = false;

    // Main loop
    bool done = false;
    while (!done)
    {
        @autoreleasepool
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

                if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP)
                {
                    bool pressed = event.type == SDL_KEYDOWN;
                    if(event.key.keysym.sym == SDLK_LEFT) {
                        debugger.SetKeyState(Joypad::Button::Left,pressed);
                    } else if (event.key.keysym.sym == SDLK_RIGHT) {
                        debugger.SetKeyState(Joypad::Button::Right,pressed);
                    } else if (event.key.keysym.sym == SDLK_UP) {
                        debugger.SetKeyState(Joypad::Button::Up,pressed);
                    } else if (event.key.keysym.sym == SDLK_DOWN) {
                        debugger.SetKeyState(Joypad::Button::Down,pressed);
                    } else if (event.key.keysym.sym == SDLK_z) {
                        debugger.SetKeyState(Joypad::Button::A,pressed);
                    } else if (event.key.keysym.sym == SDLK_x) {
                        debugger.SetKeyState(Joypad::Button::B,pressed);
                    } else if (event.key.keysym.sym == SDLK_s) {
                        debugger.SetKeyState(Joypad::Button::Start,pressed);
                    } else if (event.key.keysym.sym == SDLK_a){
                        debugger.SetKeyState(Joypad::Button::Select,pressed);
                    }

                }

                if (event.type == SDL_QUIT)
                    done = true;
                if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(window))
                    done = true;
            }

            int width, height;
            SDL_GetRendererOutputSize(renderer, &width, &height);
            layer.drawableSize = CGSizeMake(width, height);
            id<CAMetalDrawable> drawable = [layer nextDrawable];

            id<MTLCommandBuffer> commandBuffer = [commandQueue commandBuffer];
            renderPassDescriptor.colorAttachments[0].clearColor = MTLClearColorMake(UIConfig::CLEAR_COLOR.x * UIConfig::CLEAR_COLOR.w, UIConfig::CLEAR_COLOR.y * UIConfig::CLEAR_COLOR.w, UIConfig::CLEAR_COLOR.z * UIConfig::CLEAR_COLOR.w, UIConfig::CLEAR_COLOR.w);
            renderPassDescriptor.colorAttachments[0].texture = drawable.texture;
            renderPassDescriptor.colorAttachments[0].loadAction = MTLLoadActionClear;
            renderPassDescriptor.colorAttachments[0].storeAction = MTLStoreActionStore;
            id <MTLRenderCommandEncoder> renderEncoder = [commandBuffer renderCommandEncoderWithDescriptor:renderPassDescriptor];
            [renderEncoder pushDebugGroup:@"ImGui demo"];

            // Start the Dear ImGui frame
            ImGui_ImplMetal_NewFrame(renderPassDescriptor);
            ImGui_ImplSDL2_NewFrame();
            ImGui::NewFrame();
            UIConfig::setupStyle();

            debugger.UpdateUI();

            {
                ImGui::Begin("Gameboy screen:");                          // Create a window called "Hello, world!" and append into it.

                // Update Texture
                MTLRegion region = {
                    { 0, 0, 0 }, // MTLOrigin
                    {MegaBoyDebugger::GB_SCREEN_WIDTH, MegaBoyDebugger::GB_SCREEN_HEIGHT, 1} // MTLSize
                };

                // Blit gameboy screen data into metal texture
                [texture replaceRegion:region mipmapLevel:0 withBytes:debugger.screenData bytesPerRow:4*MegaBoyDebugger::GB_SCREEN_WIDTH];

                ImGui::Image((void*)(intptr_t)texture, ImVec2(MegaBoyDebugger::GB_SCREEN_WIDTH*2, MegaBoyDebugger::GB_SCREEN_HEIGHT*2));
                ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
                ImGui::End();
            }

            // Rendering
            ImGui::Render();
            ImGui_ImplMetal_RenderDrawData(ImGui::GetDrawData(), commandBuffer, renderEncoder);

            [renderEncoder popDebugGroup];
            [renderEncoder endEncoding];

            [commandBuffer presentDrawable:drawable];
            [commandBuffer commit];
        }
    }

    // Cleanup
    ImGui_ImplMetal_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
