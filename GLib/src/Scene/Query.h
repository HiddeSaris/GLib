#pragma once

#include <utility>

#include "entt.hpp"

#include "Scene.h"
#include "Entity.h"

namespace GLib {

    template<typename Range>
    class Query {
    public:
        using IterType = decltype(std::declval<Range&>().begin());

        class Iterator {
        public:
            Iterator(IterType it, Scene* scene) 
                : m_Iterator(it), m_Scene(scene) {}

            Entity operator*() const {
                return m_Scene->CreateEntity(*m_Iterator);
            }

            Iterator& operator++() {
                ++m_Iterator;
                return *this;
            }

            bool operator==(const Iterator& other) const {
                return m_Iterator == other.m_Iterator;
            }

            bool operator!=(const Iterator& other) const {
                return !(*this == other);
            }

        private:
            IterType m_Iterator;
            Scene* m_Scene;
        };

        class EachRange {
        private:
            using EachIterable = decltype(std::declval<Range&>().each());
            using IterType = decltype(std::declval<EachIterable&>().begin());
        public:
            class Iterator {
            public:
                Iterator(IterType iterator, Scene* scene) 
                    : m_Iterator(iterator), m_Scene(scene) {}

                auto operator*() const {
                    auto tuple = *m_Iterator;
                    auto entity = m_Scene->CreateEntity(std::get<0>(tuple));
                    return ReplaceFirst(std::move(tuple), entity);
                }

                Iterator& operator++() {
                    ++m_Iterator;
                    return *this;
                }

                bool operator==(const Iterator& other) const {
                    return m_Iterator == other.m_Iterator;
                }

                bool operator!=(const Iterator& other) const {
                    return !(*this == other);
                }
            private:
                IterType m_Iterator;
                Scene* m_Scene;
            private:
                template<typename Tuple, typename EntityType, size_t... I>
                static auto ReplaceFirstImpl(Tuple&& tuple, EntityType entity, std::index_sequence<I...>) {
                    return std::tuple_cat(
                        std::make_tuple(entity), 
                        std::forward_as_tuple(
                            std::get<I + 1>(std::forward<Tuple>(tuple))...
                        )
                    );
                }

                template<typename Tuple, typename EntityType>
                static auto ReplaceFirst(Tuple&& tuple, EntityType entity) {
                    constexpr size_t N = std::tuple_size_v<std::remove_cv_t<std::remove_reference_t<Tuple>>>;
                    static_assert(N >= 1);
                    return ReplaceFirstImpl(
                        std::forward<Tuple>(tuple),
                        entity,
                        std::make_index_sequence<N - 1>{}
                    );
                }
            };

            EachRange(Range range, Scene* scene) 
                : m_Range(std::move(range)), m_Each(m_Range.each()), m_Scene(scene) {}
            
            Iterator begin() {
                return { m_Each.begin(), m_Scene };
            }

            Iterator end() {
                return { m_Each.end(), m_Scene };
            }

        private:
            Range m_Range;
            EachIterable m_Each;
            Scene* m_Scene;
        };

        Query(Range range, Scene* scene)
            : m_Range(std::move(range)), m_Scene(scene) {}

        template<typename... Components>
        decltype(auto) Get(Entity entity) {
            return m_Range.template get<Components...>(entity.GetID());
        }

        template<typename Func>
        void Each(Func&& func) {
            m_Range.each(
                [this, fn = std::forward<Func>(func)]
                (auto entity, auto&&... components) mutable {
                    std::invoke(
                        fn,
                        m_Scene->CreateEntity(entity),
                        std::forward<decltype(components)>(components)...
                    );
                }
            );
        }

        auto Each() {
            return EachRange(m_Range, m_Scene);
        }

        Iterator begin() {
            return { m_Range.begin(), m_Scene };
        }

        Iterator end() {
            return { m_Range.end(), m_Scene };
        }

        size_t Size() const {
            return m_Range.size();
        }
    private:
        Range m_Range;
        Scene* m_Scene;
    };

}