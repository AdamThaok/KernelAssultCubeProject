#define GLFW_EXPOSE_NATIVE_WIN32
#pragma once
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#include <Windows.h>
#include <math.h>




void makeWindowTransparent(HWND hwnd) {
    // Set the window to be layered and transparent to mouse clicks
    SetWindowLong(hwnd, GWL_EXSTYLE, GetWindowLong(hwnd, GWL_EXSTYLE) | WS_EX_LAYERED | WS_EX_TRANSPARENT);
}

// Function to create a transparent, click-through GLFW window
GLFWwindow* createOverlayWindow(int width, int height) {
    if (!glfwInit()) {
        return nullptr;
    }

    // Create a transparent, borderless window
    glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, GLFW_TRUE);
    glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
    glfwWindowHint(GLFW_FLOATING, GLFW_TRUE);  // Always on top

    GLFWwindow* window = glfwCreateWindow(width, height, "Overlay", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return nullptr;
    }

    glfwMakeContextCurrent(window);

    // Get the window handle (HWND) to modify its style using WinAPI
    HWND hwnd = glfwGetWin32Window(window);  // Get the HWND of the GLFW window

    // Make the window click-through
    makeWindowTransparent(hwnd);

    return window;
}

// Set the position and size of the overlay window to match the game window
void setOverlayPosition(GLFWwindow* window, RECT rect) {
    glfwSetWindowPos(window, rect.left, rect.top);
    glfwSetWindowSize(window, rect.right - rect.left, rect.bottom - rect.top);
}

// Function to get the game window position using WinAPI
RECT getGameWindowRect(const wchar_t* windowName) {
    HWND hwnd = FindWindow(NULL, windowName);
    RECT rect;
    GetWindowRect(hwnd, &rect);  // Get the game window's position and size
    return rect;
}

// Function to initialize OpenGL settings
void initOpenGL() {
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);  // Set transparent background
}

// Function to draw a circle in the overlay window
void drawCircle(float x, float y, float radius) {
    int segments = 100;
    float angle;

    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(x, y);  // Center of the circle
    for (int i = 0; i <= segments; i++) {
        angle = 2.0f * 3.14159f * float(i) / float(segments);
        glVertex2f(x + cos(angle) * radius, y + sin(angle) * radius);
    }
    glEnd();
}

// Function to render the overlay (draw a circle at screen coordinates)
void render(GLFWwindow* window,float x, float y) {
    // Make the OpenGL context current for the window
    glfwMakeContextCurrent(window);

    // Clear the screen
    glClear(GL_COLOR_BUFFER_BIT);

    // Get screen dimensions
    int screenWidth, screenHeight;
    glfwGetFramebufferSize(window, &screenWidth, &screenHeight);

    // Convert the target coordinates (x, y) to OpenGL coordinates
    float normalizedX = (2.0f * x) / screenWidth - 1.0f;
    float normalizedY = 1.0f - (2.0f * y) / screenHeight;

    // Bottom-center of the screen in OpenGL coordinates
    float bottomCenterX = 0.0f;
    float bottomCenterY = -1.0f;

    // Set the color for the line
    glColor3f(1.0f, 0.0f, 0.0f); // Red color

    // Draw a line from bottom-center to the (x, y) point
    glBegin(GL_LINES);
    glVertex2f(bottomCenterX, bottomCenterY);  // Start point (bottom center)
    glVertex2f(normalizedX, normalizedY);     // End point (target coordinates)
    glEnd();

    // Swap buffers to display the rendered content
    glfwSwapBuffers(window);

    // Poll for events (if necessary)
    glfwPollEvents();
}



// Function to draw a circle using GDI on the screen
void drawCircleOnScreen(int x, int y) {
    // Get the device context (DC) for the entire screen
    HDC hdcScreen = GetDC(NULL);

    // Set the drawing color to white (can be changed)
    HPEN hPen = CreatePen(PS_SOLID, 3, RGB(255, 255, 255));
    SelectObject(hdcScreen, hPen);

    // Draw a circle (x, y, radius)
    Ellipse(hdcScreen, x, y, 300, 300);  // Coordinates and radius of the circle

    // Release the device context
    DeleteObject(hPen);
    ReleaseDC(NULL, hdcScreen);
}