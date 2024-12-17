#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include "Runtime/Core/Public/Core/UICore.h"
#include "Runtime/Core/Public/Platform/UIPlatform.h"
#include "Runtime/Core/Public/Platform/UIWindow.h"
#include "Runtime/Core/Public/Renderer/UIRenderer.h"
#include "Runtime/Core/Public/Renderer/UIShaderManager.h"
#include "Runtime/UI/Public/Controls/UIMenuBar.h"
#include "Runtime/UI/Public/Controls/UIMenu.h"
#include <iostream>
#include <chrono>
#include <thread>

using namespace VGE::Editor;

int main() {
    std::cout << "Starting VersaUI test application..." << std::endl;

    // Initialize platform
    auto& platform = UIPlatform::Get();
    if (!platform.Initialize()) {
        std::cerr << "Failed to initialize platform" << std::endl;
        return -1;
    }
    std::cout << "Platform initialized successfully" << std::endl;

    // Initialize themes
    InitializeThemes();
    std::cout << "Themes initialized successfully" << std::endl;

    // Create window
    UIWindowDesc windowDesc;
    windowDesc.Title = "VersaUI Test";
    windowDesc.Width = 1280;
    windowDesc.Height = 720;
    windowDesc.IsResizable = true;
    
    auto window = platform.CreateWindow(windowDesc);
    if (!window) {
        std::cerr << "Failed to create window" << std::endl;
        return -1;
    }
    std::cout << "Window created successfully" << std::endl;

    // Make OpenGL context current
    if (!platform.MakeContextCurrent(window.get())) {
        std::cerr << "Failed to make OpenGL context current" << std::endl;
        return -1;
    }

    // Initialize GLAD
    if (!gladLoadGL((GLADloadfunc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // Enable VSync
    platform.SetVSync(true);

    // Set up OpenGL state
    glClearColor(0.3f, 0.3f, 0.3f, 1.0f);  // Lighter gray color
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Create renderer
    auto renderer = std::make_unique<UIRenderer>();
    if (!renderer->Initialize()) {
        std::cerr << "Failed to initialize renderer" << std::endl;
        return -1;
    }

    // Create menu bar
    auto menuBar = std::make_shared<UIMenuBar>();
    menuBar->SetPosition(glm::vec2(0.0f, 0.0f));
    menuBar->SetSize(glm::vec2(static_cast<float>(windowDesc.Width), 24.0f));
    
    // Add File menu
    auto fileMenu = menuBar->AddMenu("File");
    fileMenu->AddItem("New", [](){ std::cout << "New file action" << std::endl; })->SetShortcut("Ctrl+N");
    fileMenu->AddItem("Open...", [](){ std::cout << "Open file action" << std::endl; })->SetShortcut("Ctrl+O");
    fileMenu->AddSeparator();
    fileMenu->AddItem("Save", [](){ std::cout << "Save action" << std::endl; })->SetShortcut("Ctrl+S");
    fileMenu->AddItem("Save As...", [](){ std::cout << "Save as action" << std::endl; })->SetShortcut("Ctrl+Shift+S");
    fileMenu->AddSeparator();
    fileMenu->AddItem("Exit", [](){ std::cout << "Exit action" << std::endl; });

    // Add Edit menu
    auto editMenu = menuBar->AddMenu("Edit");
    editMenu->AddItem("Undo", [](){ std::cout << "Undo action" << std::endl; })->SetShortcut("Ctrl+Z");
    editMenu->AddItem("Redo", [](){ std::cout << "Redo action" << std::endl; })->SetShortcut("Ctrl+Y");
    editMenu->AddSeparator();
    editMenu->AddItem("Cut", [](){ std::cout << "Cut action" << std::endl; })->SetShortcut("Ctrl+X");
    editMenu->AddItem("Copy", [](){ std::cout << "Copy action" << std::endl; })->SetShortcut("Ctrl+C");
    editMenu->AddItem("Paste", [](){ std::cout << "Paste action" << std::endl; })->SetShortcut("Ctrl+V");

    // Add View menu
    auto viewMenu = menuBar->AddMenu("View");
    viewMenu->AddCheckItem("Show Toolbar", true, [](){ std::cout << "Toggle toolbar" << std::endl; });
    viewMenu->AddCheckItem("Show Status Bar", true, [](){ std::cout << "Toggle status bar" << std::endl; });
    viewMenu->AddSeparator();
    viewMenu->AddItem("Reset Layout", [](){ std::cout << "Reset layout action" << std::endl; });

    // Add Theme menu
    auto themeMenu = menuBar->AddMenu("Theme");
    themeMenu->AddItem("Dark Theme", [&](){
        UIStyleManager::Get().SetActiveTheme("Dark");
        std::cout << "Switched to Dark theme" << std::endl;
    });
    themeMenu->AddItem("Light Theme", [&](){
        UIStyleManager::Get().SetActiveTheme("Light");
        std::cout << "Switched to Light theme" << std::endl;
    });

    // Set up window callbacks
    window->SetCloseCallback([&]() {
        std::cout << "Window close requested" << std::endl;
        window->Close();
    });

    window->SetResizeCallback([&](int width, int height) {
        std::cout << "Window resized to " << width << "x" << height << std::endl;
        glViewport(0, 0, width, height);
        menuBar->SetSize(glm::vec2(static_cast<float>(width), 24.0f));
    });

    window->SetFocusCallback([](bool focused) {
        std::cout << "Window " << (focused ? "gained" : "lost") << " focus" << std::endl;
    });

    // Add mouse callbacks
    window->SetMouseMoveCallback([&](double xpos, double ypos) {
        menuBar->OnMouseMove(glm::vec2(xpos, ypos));
    });

    window->SetMouseButtonCallback([&](int button, int action, int mods) {
        if (button == GLFW_MOUSE_BUTTON_LEFT) {
            double xpos, ypos;
            glfwGetCursorPos(window->GetNativeHandle(), &xpos, &ypos);
            if (action == GLFW_PRESS) {
                menuBar->OnMouseDown(glm::vec2(xpos, ypos));
            } else if (action == GLFW_RELEASE) {
                menuBar->OnMouseUp(glm::vec2(xpos, ypos));
            }
        }
    });

    // Initial viewport setup
    int width, height;
    glfwGetFramebufferSize(window->GetNativeHandle(), &width, &height);
    renderer->SetViewport(0, 0, width, height);

    // Show window
    window->Show();
    window->Focus();

    std::cout << "\nEntering main loop..." << std::endl;

    auto lastFrameTime = std::chrono::high_resolution_clock::now();

    // Main loop
    while (!window->ShouldClose()) {
        // Calculate delta time
        auto currentFrameTime = std::chrono::high_resolution_clock::now();
        float deltaTime = std::chrono::duration<float>(currentFrameTime - lastFrameTime).count();
        lastFrameTime = currentFrameTime;

        // Poll events
        platform.PollEvents();

        // Clear screen
        glClear(GL_COLOR_BUFFER_BIT);

        // Begin frame
        renderer->BeginFrame();

        // Update UI
        menuBar->OnUpdate(deltaTime);

        // Render UI
        menuBar->OnPaint(*renderer);

        // End frame
        renderer->EndFrame();

        // Swap buffers
        window->SwapBuffers();

        // Cap frame rate to ~60 FPS
        if (deltaTime < 0.016f) {
            std::this_thread::sleep_for(std::chrono::duration<float>(0.016f - deltaTime));
        }
    }

    std::cout << "Application shutting down..." << std::endl;
    return 0;
}


