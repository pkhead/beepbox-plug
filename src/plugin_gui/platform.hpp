#pragma once
#include <pugl/pugl.h>
#include <imgui/imgui.h>

// forward declaration of gfx::WindowData
namespace gfx {
    struct WindowData;
}

namespace platform {
    enum class Key {
        None = 0,
        Space = 32,
        Apostrophe = 39,
        Comma = 44,
        Minus = 45,
        Period = 46,
        Slash = 47,
        Zero = 48,
        One = 49,
        Two = 50,
        Three = 51,
        Four = 52,
        Five = 53,
        Six = 54,
        Seven = 55,
        Eight = 56,
        Nine = 57,
        Semicolon = 59,
        Equal = 61,
        A = 65,
        B = 66,
        C = 67,
        D = 68,
        E = 69,
        F = 70,
        G = 71,
        H = 72,
        I = 73,
        J = 74,
        K = 75,
        L = 76,
        M = 77,
        N = 78,
        O = 79,
        P = 80,
        Q = 81,
        R = 82,
        S = 83,
        T = 84,
        U = 85,
        V = 86,
        W = 87,
        X = 88,
        Y = 89,
        Z = 90,
        LeftBracket = 91,
        Backslash = 92,
        RightBracket = 93,
        GraveAccent = 96,
        World1 = 161,
        World2 = 162,
        Escape = 256,
        Enter = 257,
        Tab = 258,
        Backspace = 259,
        Insert = 260,
        Delete = 261,
        Right = 262,
        Left = 263,
        Down = 264,
        Up = 265,
        PageUp = 266,
        PageDown = 267,
        Home = 268,
        End = 269,
        CapsLock = 280,
        ScrollLock = 281,
        NumLock = 282,
        PrintScreen = 283,
        Pause = 284,
        F1 = 290,
        F2 = 291,
        F3 = 292,
        F4 = 293,
        F5 = 294,
        F6 = 295,
        F7 = 296,
        F8 = 297,
        F9 = 298,
        F10 = 299,
        F11 = 300,
        F12 = 301,
        F13 = 302,
        F14 = 303,
        F15 = 304,
        F16 = 305,
        F17 = 306,
        F18 = 307,
        F19 = 308,
        F20 = 309,
        F21 = 310,
        F22 = 311,
        F23 = 312,
        F24 = 313,
        F25 = 314,
        Kp0 = 320,
        Kp1 = 321,
        Kp2 = 322,
        Kp3 = 323,
        Kp4 = 324,
        Kp5 = 325,
        Kp6 = 326,
        Kp7 = 327,
        Kp8 = 328,
        Kp9 = 329,
        KpDECIMAL = 330,
        KpDIVIDE = 331,
        KpMULTIPLY = 332,
        KpSUBTRACT = 333,
        KpADD = 334,
        KpENTER = 335,
        KpEQUAL = 336,
        LeftShift = 340,
        LeftControl = 341,
        LeftAlt = 342,
        LeftSuper = 343,
        RightShift = 344,
        RightControl = 345,
        RightAlt = 346,
        RightSuper = 347,
        Menu = 348,
    };

    struct Event {
        enum EventType {
            MouseDown,
            MouseUp,
            MouseMove,
            MouseWheel,
            KeyDown,
            KeyUp,

            Realize,
            Unrealize,
            Idle,
        } type;
        
        union {
            int button;
            platform::Key key;
        };

        int x;
        int y;
    };

    struct Window;

    typedef void (*EventHandler)(Event event, Window *platform);
    typedef void (*DrawHandler)(Window *platform);

    void setup();
    void shutdown();

    Window* createWindow(int width, int height, const char *name, EventHandler event, DrawHandler draw);
    void closeWindow(Window *window);

    void setUserdata(Window *window, void *ud);
    void *getUserdata(Window *window);

    int getWidth(Window *window);
    int getHeight(Window *window);

    bool setVisible(Window *window, bool visible);
    bool setTitle(Window *window, const char *title);
    bool setParent(Window *window, void *parentHandle);
    bool setTransientParent(Window *window, void *parentHandle);
    void requestRedraw(Window *window, int extraFrames = 0);

    struct Window {
        PuglView *puglView;
        bool isRealized;

        int width;
        int height;
        platform::EventHandler evCallback;
        platform::DrawHandler drawCallback;

        ImGuiContext *imguiCtx;
        gfx::WindowData *gfxData;

        int redrawCounter;
        void *userdata;
    };
}