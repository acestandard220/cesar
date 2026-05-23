#pragma once
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#include "../Core/Types.h"
#include "../Core/Log.h"
#include "../Event/Delegate.h"

#include <print>;

namespace cesar
{
	struct __declspec(dllexport) WindowCreationParams
	{
		Uint32 width;
		Uint32 height;

		const Char* title;
	};

	struct WindowEventInfo {
		HWND hwnd;
		UINT msg;
		WPARAM wparam;
		LPARAM lparam;
		float width;
		float height;
	};

	DECLARE_EVENT(WindowEvent, Window, const WindowEventInfo&);

	class __declspec(dllexport) Window
	{
	public:

		Window(WindowCreationParams window_create_params);
		~Window();

		Bool Run();
		void Shutdown();

		Bool IsActive()const;
		HWND GetNative()const;
		GLFWwindow* GetGLFWwindow();

		Uint32 GetClientWidth()const;
		Uint32 GetClientHeight()const;

		void GetClientDimensions(Uint32& width, Uint32& height)const;

		WindowEvent& GetWindowEvent();

	private:
		inline static Window* self = nullptr;

		GLFWwindow* window;

		Uint32 client_width;
		Uint32 client_height;

		WindowEvent window_event;
	};
}

