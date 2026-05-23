// Portions of this file are derived from Adria Renderer (MIT License)
// Copyright (c) <https://github.com/mateeeeeee>
// See ext/Adria/LICENSE for details.
#pragma once
#include "../Core/Types.h"
#include "../Core/Log.h"

#include "Delegate.h"
#include "../Platform/Window.h"
#include <optional>

namespace cesar
{

    enum class Key : Uint32 {
        A, B, C, D, E, F,
        G, H, I, J, K, L,
        M, N, O, P, Q, R,
        S, T, U, V, W, X,
        Y, Z,

        Alpha0, Alpha1, Alpha2, Alpha3,
        Alpha4, Alpha5, Alpha6, Alpha7,
        Alpha8, Alpha9,

        Num0, Num1, Num2, Num3, Num4,
        Num5, Num6, Num7, Num8, Num9,

        F1, F2, F3, F4, F5, F6,
        F7, F8, F9, F10, F11, F12,

        Space,
        Enter,
        Tab,

        Left,
        Up,
        Right,
        Down,

        Esc,
        Shift,
        Control,
        LAlt,
        RAlt,

        MouseLeft, MouseRight, MouseMiddle,

        Gamepad1,
        Gamepad2,
        Gamepad3,
        Gamepad4,

        GamepadLT,
        GamepadLB,
        GamepadRT,
        GamepadRB,


        None
    };

    DECLARE_EVENT(MiddleMouseScrollEvent, Input, int);
    DECLARE_EVENT(RightMouseClickedEvent, Input, uint32_t, uint32_t);
    DECLARE_EVENT(WindowResizeEvent, Input, uint32_t, uint32_t);
    DECLARE_EVENT(ViewportResizeEvent, Input, uint32_t, uint32_t);
    DECLARE_EVENT(F9ClickedEvent, Input, bool);

    struct InputEvent {
        MiddleMouseScrollEvent middleMouseEvent;
        RightMouseClickedEvent rightMouseEvent;
        WindowResizeEvent windowResizeEvent;
        ViewportResizeEvent viewport_resize_event;

        F9ClickedEvent f9ClickedEvent;
    };

    class _declspec(dllexport) Input
    {
    public:
        Input()
            :initialize(false), _window(nullptr), prevMousePosX(0.0f), prevMousePosY(0.0f), mouseWheelDelta(0.0f)
        {
            keys.fill(false);
            prevKeys.fill(false);

            POINT mouseScreenPos{};
            if (GetCursorPos(&mouseScreenPos)) {
                mousePosX = static_cast<float>(mouseScreenPos.x);
                mousePosY = static_cast<float>(mouseScreenPos.y);
            }
        }

        void Initialize(Window* window);

        InputEvent& GetInputEvent();
        void OnWindowEvent(const WindowEventInfo& info);


        void Tick();
        bool IsPressed(int key)const noexcept;

        bool GetKey(Key key)const;
        bool IsKeyDown(Key key)const;
        bool IsKeyUp(Key key)const;
        bool IsKeyHeld(Key key)const;

        void SetMousePosition(float x, float y);
        void SetMouseVisiblity(bool isVisible);

        float GetMousePosX()const;
        float GetMousePosY()const;

        float GetMouseDeltaX()const;
        float GetMouseDeltaY()const;

        float GetMouseWheelData()const;

        void DeferResize(Uint32 width, Uint32 height);

        static Input& Get();

    private:
        Input(const Input&) = delete;
        Input& operator=(const Input&) = delete;
    private:
        InputEvent inputEvent;

        std::array<bool, static_cast<uint32_t>(Key::None)> keys;
        std::array<bool, static_cast<uint32_t>(Key::None)> prevKeys;

        struct DeferedResizeInfo {
            Uint32 w, h;
        };
        std::optional<DeferedResizeInfo> defered_resize;

        float mousePosX;
        float mousePosY;

        float prevMousePosX;
        float prevMousePosY;

        float mouseWheelDelta;

        Window* _window;
        bool initialize;
    };

#define gInput Input::Get()
}