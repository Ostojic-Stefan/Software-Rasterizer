#pragma once
#include <array>
#include <cassert>
#include <limits>
#include <optional>
#include <utility>

#include "types.hpp"

namespace rnd
{
    struct resource_handle
    {
        uint16_t idx;
        uint16_t gen;

        // explicit ctor so brace-init always works
        constexpr resource_handle(uint16_t i = kNullIndex, uint16_t g = 0) noexcept
            : idx(i), gen(g) {}

        // make kNullIndex part of the handle itself:
        static constexpr uint16_t kNullIndex = std::numeric_limits<uint16_t>::max();
    };

    template<typename HandleT, uint16_t MaxHandles>
    struct handle_manager
    {
        static constexpr uint16_t kNullIndex = std::numeric_limits<uint16_t>::max();

        handle_manager()
        {
            for (uint16_t i = 0; i < MaxHandles - 1; ++i) {
                _nextFree[i] = i + 1;
                _generation[i] = 0;
            }
            _nextFree[MaxHandles - 1] = kNullIndex;
            _generation[MaxHandles - 1] = 0;
            _head = 0;
        }

        HandleT alloc()
        {
            if (_head == kNullIndex) {
                return { kNullIndex, 0 };
            }
            uint16_t idx = _head;
            _head = _nextFree[idx];
            ++_generation[idx];
            return { idx, _generation[idx] };
        }

        void free(HandleT h) 
        {
            assert(valid(h) && "free(): invalid handle");
            _nextFree[h.idx] = _head;
            _head = h.idx;
            ++_generation[h.idx];
        }

        bool valid(HandleT h) const
        {
            return h.idx < MaxHandles && h.gen == _generation[h.idx];
        }

    private:
        uint16_t _head;
        std::array<uint16_t, MaxHandles> _nextFree;
        std::array<uint16_t, MaxHandles> _generation;
    };

    template <typename ResourceT, rnd::u16 MaxResources>
    struct resource_manager
    {
        using Handle = resource_handle;

        static bool is_null(Handle h) 
        {
            return h.idx == handle_manager<Handle, MaxResources>::kNullIndex;
        }

        resource_manager() = default;

        //Handle alloc(const ResourceT& resource)
        //{
        //    Handle h = _hm.alloc();
        //    if (!is_null())
        //        _resources[h.idx] = resource;
        //    return h;
        //}

        template <typename ...Args>
        Handle emplace(Args&&... args)
        {
            Handle h = _hm.alloc();
            if (!is_null(h))
                _resources[h.idx].emplace(std::forward<Args>(args)...);
            return h;
        }

        void free(Handle h)
        {
            assert(_hm.valid(h) && "invalid handle");
            _resources[h.idx].reset();
            _hm.free(h);
        }

        bool valid(Handle h) const
        {
            return _hm.valid(h);
        }

        ResourceT& get(Handle h) 
        {
            assert(valid(h) && _resources[h.idx].has_value() && "invalid handle");
            return *_resources[h.idx];
        }

        const ResourceT& get(Handle h) const 
        {
            assert(valid(h) && _resources[h.idx].has_value() && "invalid handle");
            return *_resources[h.idx];
        }

        ResourceT* get_ptr(Handle h)
        {
            assert(valid(h) && _resources[h.idx].has_value() && "invalid handle");
            return &*_resources[h.idx];
        }

        const ResourceT* get_ptr(Handle h) const
        {
            assert(valid(h) && _resources[h.idx].has_value() && "invalid handle");
            return &*_resources[h.idx];
        }

        //ResourceT& operator[](Handle h) { return get(h); }
        //const ResourceT& operator[](Handle h) const { return get(h); }

        static constexpr Handle null_handle() 
        {
            return { handle_manager<Handle,MaxResources>::kNullIndex, 0 };
        }
    private:
        handle_manager<Handle, MaxResources>    _hm;
        std::array<std::optional<ResourceT>, MaxResources>     _resources;
    };

}
