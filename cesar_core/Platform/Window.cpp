#include "Window.h";

#include <Tracy/tracy\Tracy.hpp>

#include <functional>


namespace cesar
{
	Window::Window(WindowCreationParams window_create_params)
	{
		ZoneScopedN(__FUNCTION__);

		self = this;
		if (!glfwInit()) {
			LOG_FATAL("Failed to initialize GLFW");
		}

		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

		window = glfwCreateWindow(static_cast<int>(window_create_params.width), static_cast<int>(window_create_params.height), window_create_params.title, 0, 0);
		if (!window) {
			LOG_FATAL("Failed to create window.");
			return;
		}

		glfwSetFramebufferSizeCallback(window, [](GLFWwindow* window, int width, int height)
			{
				WindowEventInfo window_event_info{};
				window_event_info.msg = WM_SIZE;
				window_event_info.width = width;
				window_event_info.height = height;

				window_event_info.wparam = 0;
				window_event_info.lparam = MAKELPARAM(width, height);

				self->window_event.Broadcast(window_event_info);
			});

		glfwSetScrollCallback(window, [](GLFWwindow*, double xoffset, double yoffset)
			{
				WindowEventInfo window_event_info{};
				window_event_info.msg = WM_MOUSEWHEEL;

				short wheel_y = static_cast<short>(yoffset * WHEEL_DELTA);
				short wheel_x = static_cast<short>(xoffset * WHEEL_DELTA);

				window_event_info.wparam = MAKEWPARAM(wheel_x, wheel_y);
				window_event_info.lparam = 0;

				self->window_event.Broadcast(window_event_info);
			});

		glfwSetMouseButtonCallback(window, [](GLFWwindow*, int button, int action, int mods)
			{
				WindowEventInfo window_event_info{};
				window_event_info.msg =
					(action == GLFW_PRESS) ? WM_LBUTTONDOWN :
					(action == GLFW_RELEASE) ? WM_LBUTTONUP : 0;

				// Map GLFW buttons → Win32-style
				if (button == GLFW_MOUSE_BUTTON_RIGHT)
				{
					window_event_info.msg =
						(action == GLFW_PRESS) ? WM_RBUTTONDOWN : WM_RBUTTONUP;
				}
				else if (button == GLFW_MOUSE_BUTTON_MIDDLE)
				{
					window_event_info.msg =
						(action == GLFW_PRESS) ? WM_MBUTTONDOWN : WM_MBUTTONUP;
				}

				window_event_info.wparam = 0;
				window_event_info.lparam = 0;

				self->window_event.Broadcast(window_event_info);
			});

		glfwSetKeyCallback(window, [](GLFWwindow*, int key, int scancode, int action, int mods)
			{
				WindowEventInfo window_event_info{};

				window_event_info.msg =
					(action == GLFW_PRESS) ? WM_KEYDOWN :
					(action == GLFW_RELEASE) ? WM_KEYUP : 0;

				window_event_info.wparam = key;
				window_event_info.lparam = (action == GLFW_REPEAT) ? 1 : 0;

				self->window_event.Broadcast(window_event_info);
			});

		glfwSetCharCallback(window, [](GLFWwindow*, unsigned int c)
			{
				WindowEventInfo window_event_info{};
				window_event_info.msg = WM_CHAR;

				window_event_info.wparam = c;
				window_event_info.lparam = 0;

				self->window_event.Broadcast(window_event_info);
			});

		glfwSetWindowFocusCallback(window, [](GLFWwindow*, int focused)
			{
				WindowEventInfo window_event_info{};
				window_event_info.msg = WM_SETFOCUS;

				window_event_info.wparam = focused;
				window_event_info.lparam = 0;

				self->window_event.Broadcast(window_event_info);
			});
	}

	Window::~Window()
	{

	}

	Bool Window::Run()
	{
		if (!glfwWindowShouldClose(window)) {

			glfwPollEvents();
			glfwSwapBuffers(window);
			return true;
		}
		return false;
	}

	void Window::Shutdown()
	{
		glfwDestroyWindow(window);
		glfwTerminate();
	}

	Bool Window::IsActive()const
	{
		return glfwGetWindowAttrib(window, GLFW_FOCUSED);
	}

	HWND Window::GetNative() const
	{
		return glfwGetWin32Window(window);
	}

	GLFWwindow* Window::GetGLFWwindow()
	{
		return window;
	}

	Uint32 Window::GetClientWidth() const
	{
		int width, height;
		glfwGetFramebufferSize(window, &width, &height);
		return width;
	}

	Uint32 Window::GetClientHeight() const
	{
		int width, height;
		glfwGetFramebufferSize(window, &width, &height);
		return height;
	}

	void Window::GetClientDimensions(Uint32& width, Uint32& height) const
	{
		glfwGetFramebufferSize(window, (int*)&width, (int*)&height);
	}

	WindowEvent& Window::GetWindowEvent()
	{
		return window_event;
	}
}
