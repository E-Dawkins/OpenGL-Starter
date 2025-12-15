#pragma once

const int MOUSE_INPUT_BUFFER_FRAMES = 2;

class Window
{
protected:
	struct GLFWwindow* mWindow = nullptr;

	int mWidth, mHeight;
	std::string mTitle;

	int mMouseInputBuffer = MOUSE_INPUT_BUFFER_FRAMES;
	glm::dvec2 mLastCursorPos = glm::dvec2(0);

public:
	Window(int _width, int _height, const std::string& _title);
	~Window();

	void SwapBuffers();

	// Setters

	void SetShouldClose(bool _closeState = true);
	void SetCursorInputMode(bool _visibleInWindow, bool _confinedToWindow);

	// Getters

	bool ShouldClose() const;
	bool IsFocused() const;
	bool GetKeyPressed(int _key) const;
	const glm::dvec2 GetMouseDelta();
	int GetWidth() const;
	int GetHeight() const;

protected:
	static void FrameBufferSizeCallback(GLFWwindow* _window, int _width, int _height);

	static void WindowFocusCallback(GLFWwindow* _window, int _focused);
	void OnWindowFocusChanged(int _focused);

	static void CursorPosCallback(GLFWwindow* _window, double _x, double _y);
	void OnCursorPosChanged(double _x, double _y);
};