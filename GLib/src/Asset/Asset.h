#pragma once

#include <vector>
#include <memory>
#include <cstdint>

namespace GLib {

    template<typename A>
    struct Handle {
        uint32_t index;
    };

    template<typename A>
    class AssetStorage {
    public:
        Handle<A> Reserve() {
            uint32_t idx = m_Storage.size();
            m_Storage.push_back(nullptr);
            return Handle<A>{ idx };
        }

        void Insert(Handle<A> handle, std::shared_ptr<A> asset) {
            m_Storage[handle.index] = std::move(asset);
        }

        Handle<A> Add(std::shared_ptr<A> asset) {
            Handle<A> handle = Reserve();
            Insert(handle, std::move(asset));
            return handle;
        }

        std::shared_ptr<A> Get(Handle<A> handle) const {
            return m_Storage[handle.index];
        }

        bool IsLoaded(Handle<A> handle) const {
            return handle.index < m_Storage.size() && m_Storage[handle.index] != nullptr;
        }
    private:
        std::vector<std::shared_ptr<A>> m_Storage;
    };

}