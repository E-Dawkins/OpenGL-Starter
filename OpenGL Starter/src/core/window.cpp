#include "pch.h"

#include "core/window.h"

Window::Window(int _width, int _height, const std::string& _title)
	: mWidth(_width), mHeight(_height), mTitle(_title)
{
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    mWindow = glfwCreateWindow(_width, _height, _title.c_str(), nullptr, nullptr);
    if (mWindow == nullptr)
    {
        std::cout << "Failed to create GLFW window\n";
        glfwTerminate();
        return;
    }

    glfwMakeContextCurrent(mWindow);

    glfwSetWindowUserPointer(mWindow, this); // in-case we need to reference the window class from a static callback

    glfwSetFramebufferSizeCallback(mWindow, FrameBufferSizeCallback);
    glfwSetWindowFocusCallback(mWindow, WindowFocusCallback);
    glfwSetCursorPosCallback(mWindow, CursorPosCallback);
}

Window::~Window()
{
    glfwDestroyWindow(mWindow);
}

void Window::SwapBuffers()
{
    glfwSwapBuffers(mWindow);
}

void Window::SetShouldClose(bool _closeState)
{
    glfwSetWindowShouldClose(mWindow, _closeState);
}

void Window::SetCursorInputMode(bool _visibleInWindow, bool _confinedToWindow)
{
    if (_visibleInWindow)
    {
        if (_confinedToWindow)
        {
            glfwSetInputMode(mWindow, GLFW_CURSOR, GLFW_CURSOR_CAPTURED);
        }
        else
        {
            glfwSetInputMode(mWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }
    }
    else
    {
        if (_confinedToWindow)
        {
            glfwSetInputMode(mWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        }
        else
        {
            glfwSetInputMode(mWindow, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
        }
    }
}

bool Window::ShouldClose() const
{
    return glfwWindowShouldClose(mWindow);
}

bool Window::IsFocused() const
{
    return glfwGetWindowAttrib(mWindow, GLFW_FOCUSED);
}

bool Window::GetKeyPressed(int _key) const
{
    return glfwGetKey(mWindow, _key) == GLFW_PRESS;
}

const glm::dvec2 Window::GetMouseDelta()
{
    if (!IsFocused())
    {
        return glm::vec2(0);
    }

    glm::dvec2 cursorPos;
    glfwGetCursorPos(mWindow, &cursorPos.x, &cursorPos.y);

    glm::dvec2 delta = cursorPos - mLastCursorPos;
    mLastCursorPos = cursorPos;

    if (mMouseInputBuffer > 0)
    {
        return glm::dvec2(0);
    }

    return delta;
}

int Window::GetWidth() const
{
    return mWidth;
}

int Window::GetHeight() const
{
    return mHeight;
}

void Window::FrameBufferSizeCallback(GLFWwindow* _window, int _width, int _height)
{
    glfwMakeContextCurrent(_window);
    glViewport(0, 0, _width, _height); // resize viewport to match new width & height
}

void Window::WindowFocusCallback(GLFWwindow* _window, int _focused)
{
    Window* instance = static_cast<Window*>(glfwGetWindowUserPointer(_window));
    if (instance)
    {
        instance->OnWindowFocusChanged(_focused);
    }
}

void Window::OnWindowFocusChanged(int /*_focused*/)
{
    mMouseInputBuffer = MOUSE_INPUT_BUFFER_FRAMES;
}

void Window::CursorPosCallback(GLFWwindow* _window, double _x, double _y)
{
    Window* instance = static_cast<Window*>(glfwGetWindowUserPointer(_window));
    if (instance)
    {
        instance->OnCursorPosChanged(_x, _y);
    }
}

void Window::OnCursorPosChanged(double /*_x*/, double /*_y*/)
{
    if (mMouseInputBuffer > 0)
    {
        mMouseInputBuffer--;
    }
}
