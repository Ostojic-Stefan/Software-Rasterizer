#pragma once

#include "types.hpp"
#include "math/vector.hpp"

namespace rnd::input
{
    enum class key_code
    {
        UNKNOWN = 0,

        A = 4,
        B = 5,
        C = 6,
        D = 7,
        E = 8,
        F = 9,
        G = 10,
        H = 11,
        I = 12,
        J = 13,
        K = 14,
        L = 15,
        M = 16,
        N = 17,
        O = 18,
        P = 19,
        Q = 20,
        R = 21,
        S = 22,
        T = 23,
        U = 24,
        V = 25,
        W = 26,
        X = 27,
        Y = 28,
        Z = 29,

        NUM_1 = 30,
        NUM_2 = 31,
        NUM_3 = 32,
        NUM_4 = 33,
        NUM_5 = 34,
        NUM_6 = 35,
        NUM_7 = 36,
        NUM_8 = 37,
        NUM_9 = 38,
        NUM_0 = 39,

        RETURN = 40,
        ESCAPE = 41,
        BACKSPACE = 42,
        TAB = 43,
        SPACE = 44,
        MINUS = 45,
        EQUALS = 46,
        LEFTBRACKET = 47,
        RIGHTBRACKET = 48,
        BACKSLASH = 49,
        NONUSHASH = 50,
        SEMICOLON = 51,
        APOSTROPHE = 52,
        BACKQUOTE = 53,
        COMMA = 54,
        PERIOD = 55,
        SLASH = 56,
        CAPSLOCK = 57,

        F1 = 58,
        F2 = 59,
        F3 = 60,
        F4 = 61,
        F5 = 62,
        F6 = 63,
        F7 = 64,
        F8 = 65,
        F9 = 66,
        F10 = 67,
        F11 = 68,
        F12 = 69,

        PRINTSCREEN = 70,
        SCROLLLOCK = 71,
        PAUSE = 72,

        INSERT = 73,
        HOME = 74,
        PAGEUP = 75,
        DEL = 76,
        END = 77,
        PAGEDOWN = 78,

        RIGHT = 79,
        LEFT = 80,
        DOWN = 81,
        UP = 82,

        NUMLOCKCLEAR = 83,

        KP_DIVIDE = 84,
        KP_MULTIPLY = 85,
        KP_MINUS = 86,
        KP_PLUS = 87,
        KP_ENTER = 88,
        KP_1 = 89,
        KP_2 = 90,
        KP_3 = 91,
        KP_4 = 92,
        KP_5 = 93,
        KP_6 = 94,
        KP_7 = 95,
        KP_8 = 96,
        KP_9 = 97,
        KP_0 = 98,
        KP_PERIOD = 99,

        APPLICATION = 101,
        MUTE = 127,
        VOLUMEUP = 128,
        VOLUMEDOWN = 129,

        LCTRL = 224,
        LSHIFT = 225,
        LALT = 226,
        LGUI = 227,
        RCTRL = 228,
        RSHIFT = 229,
        RALT = 230,
        RGUI = 231,
    };

    void init();
    void shutdown();
    void update();

    rnd::b8 is_key_pressed(key_code key);
    rnd::b8 is_key_down(key_code key);
    rnd::b8 is_key_released(key_code key);
    rnd::b8 is_key_up(key_code key);
    void process_key(key_code code, rnd::b8 is_down);

    enum class mouse_btn
    {
        left,
        middle,
        right
    };

    math::vec2 get_mouse_pos();
    rnd::b8 is_mouse_down(mouse_btn btn);
    rnd::b8 is_mouse_pressed(mouse_btn btn);
    rnd::b8 is_mouse_up(mouse_btn btn);
    rnd::b8 is_mouse_released(mouse_btn btn);
    void process_mouse_move(math::vec2 pos);
    void process_mouse(mouse_btn, rnd::b8 is_down);

    void process_mouse_wheel(rnd::f32 value);
    rnd::f32 get_mouse_wheel_delta();
}
