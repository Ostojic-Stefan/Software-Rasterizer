#pragma once

#include <functional>
#include <variant>
#include "input.hpp"

namespace rnd::event
{
    enum class EventType : uint8_t
    {
        Key, MouseMove, MousePress, Resize, AppQuit
    };

    struct KeyData { input::key_code key; bool pressed; };
    struct MousePressData { input::mouse_btn btn; };
    struct MouseMoveData { math::vec2 pos; };
    // struct ApplicationCloseData { pxl::vec2 pos; };

    struct Event
    {
        EventType type;
        union
        {
            KeyData key;
            MousePressData mouse_press;
            MouseMoveData mouse_move;
        } data;
    };

    using Callback = std::function<void(const Event&)>;

    void registerCallback(Callback cb);
    void trigger_event(const Event& e);
}