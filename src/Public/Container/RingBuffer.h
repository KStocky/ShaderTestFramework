#pragma once

#include "Utility/Expected.h"
#include "Platform.h"
#include <optional>
#include <type_traits>
#include <vector>

namespace stf
{
    template<typename T>
    class RingBuffer
    {
    public:

        enum class Qualifier
        {
            NonConst,
            Const
        };

        enum class Direction
        {
            Forwards,
            Backwards
        };

        enum class EErrorType
        {
            Success,
            EmptyBuffer
        };

        template<typename T>
        using Expected = Expected<T, EErrorType>;

        template<Qualifier Qual, Direction Dir>
        class Iterator
        {

            using reference = typename std::conditional_t<Qual == Qualifier::Const, const T&, T&>;
            using pointer = typename std::conditional_t<Qual == Qualifier::Const, const T*, T*>;
            using buffer_type = typename std::conditional_t<Qual == Qualifier::Const, const RingBuffer*, RingBuffer*>;
        public:

            Iterator() = default;
            Iterator(buffer_type InBuffer, const u64 InIndex)
                : m_Buffer(InBuffer)
                , m_Index(InIndex)
            {}

            Iterator& operator++()
            {
                IncrementIndex();
                return *this;
            }

            Iterator operator++(int)
            {
                Iterator ret = *this;
                IncrementIndex();
                return ret;
            }

            Iterator& operator--()
            {
                DecrementIndex();
                return *this;
            }

            Iterator operator--(int)
            {
                Iterator ret = *this;
                DecrementIndex();
                return ret;
            }

            reference operator*() const
            {
                return m_Buffer->m_Data[m_Index].value();
            }

            pointer operator->() const
            {
                return &m_Buffer->m_Data[m_Index].value();
            }

            friend bool operator==(const Iterator& InA, const Iterator& InB)
            {
                return InA.m_Buffer == InB.m_Buffer && InA.m_Index == InB.m_Index;
            }

            friend bool operator!=(const Iterator& InA, const Iterator& InB)
            {
                return !(InA == InB);
            }

        private:

            void IncrementIndex()
            {
                if constexpr (Dir == Direction::Forwards)
                {
                    m_Index = (m_Index + 1) % m_Buffer->m_Data.size();
                }
                else
                {
                    m_Index = m_Index == 0 ? m_Buffer->m_Data.size() - 1 : m_Index - 1;
                }
            }

            void DecrementIndex()
            {
                if constexpr (Dir == Direction::Forwards)
                {
                    m_Index = m_Index == 0 ? m_Buffer->m_Data.size() - 1 : m_Index - 1;
                }
                else
                {
                    m_Index = (m_Index + 1) % m_Buffer->m_Data.size();
                }
            }

            buffer_type m_Buffer = nullptr;
            u64 m_Index = 0;
        };

        using value_type = T;
        using allocator_type = typename std::vector<T>::allocator_type;
        using size_type = i64;
        using difference_type = i64;
        using reference = T&;
        using const_reference = const T&;
        using pointer = T*;
        using const_pointer = const T*;
        using iterator = Iterator<Qualifier::NonConst, Direction::Forwards>;
        using const_iterator = Iterator<Qualifier::Const, Direction::Forwards>;
        using reverse_iterator = Iterator<Qualifier::NonConst, Direction::Backwards>;
        using const_reverse_iterator = Iterator<Qualifier::Const, Direction::Backwards>;

        RingBuffer()
            : RingBuffer(0)
        {}

        explicit RingBuffer(const u64 InSize)
            : m_Data(InSize + 1)
            , m_HeadIndex(0)
            , m_TailIndex(0)
            , m_Size(0)
        {
        }

        void push_back(const T& In)
        {
            if (m_Size == capacity())
            {
                Resize();
            }

            m_Data[m_TailIndex] = In;
            m_TailIndex = AdvanceIndex(m_TailIndex);

            ++m_Size;
        }

        void push_back(T&& In)
        {
            if (m_Size == capacity())
            {
                Resize();
            }

            m_Data[m_TailIndex] = std::move(In);
            m_TailIndex = AdvanceIndex(m_TailIndex);

            ++m_Size;
        }

        Expected<value_type> pop_front()
        {
            if (empty())
            {
                return Unexpected(EErrorType::EmptyBuffer);
            }

            const auto prevHeadIndex = m_HeadIndex;
            m_HeadIndex = AdvanceIndex(m_HeadIndex);
            --m_Size;

            return std::move(m_Data[prevHeadIndex].value());
        }

        size_type size() const
        {
            return m_Size;
        }

        size_type capacity() const
        {
            return m_Data.size() - 1;
        }

        bool empty() const
        {
            return m_Size == 0;
        }

        auto begin()
        {
            return Iterator<Qualifier::NonConst, Direction::Forwards>(this, m_HeadIndex);
        }

        auto begin() const
        {
            return Iterator<Qualifier::Const, Direction::Forwards>(this, m_HeadIndex);
        }

        auto cbegin() const
        {
            return Iterator<Qualifier::Const, Direction::Forwards>(this, m_HeadIndex);
        }

        auto end()
        {
            return Iterator<Qualifier::NonConst, Direction::Forwards>(this, m_TailIndex);
        }

        auto end() const
        {
            return Iterator<Qualifier::Const, Direction::Forwards>(this, m_TailIndex);
        }

        auto cend() const
        {
            return Iterator<Qualifier::Const, Direction::Forwards>(this, m_TailIndex);
        }

        auto rbegin()
        {
            return Iterator<Qualifier::NonConst, Direction::Backwards>(this, m_HeadIndex);
        }

        auto rbegin() const
        {
            return Iterator<Qualifier::Const, Direction::Backwards>(this, m_HeadIndex);
        }

        auto crbegin() const
        {
            return Iterator<Qualifier::Const, Direction::Backwards>(this, m_HeadIndex);
        }

        auto rend()
        {
            return Iterator<Qualifier::NonConst, Direction::Backwards>(this, m_TailIndex);
        }

        auto rend() const
        {
            return Iterator<Qualifier::Const, Direction::Backwards>(this, m_TailIndex);
        }

        auto crend() const
        {
            return Iterator<Qualifier::Const, Direction::Backwards>(this, m_TailIndex);
        }

        reference front()
        {
            return m_Data[m_HeadIndex].value();
        }

        const_reference front() const
        {
            return m_Data[m_HeadIndex].value();
        }

    private:

        void Resize()
        {
            std::vector<std::optional<T>> buffer;
            buffer.resize(m_Data.size() + 1);

            for (auto index = 0ll; index < m_Size; ++index)
            {
                auto moveIndex = AdvanceIndex(m_HeadIndex, index);
                buffer[index] = std::move(m_Data[moveIndex].value());
            }

            m_Data = std::move(buffer);
            m_HeadIndex = 0;
            m_TailIndex = m_Size;
        }

        u64 AdvanceIndex(const u64 InIndex, const u64 InOffset = 1) const
        {
            return (InIndex + InOffset) % m_Data.size();
        }

        u64 DecrementIndex(const u64 InIndex, const u64 InOffset = 1) const
        {
            const u64 offset = InOffset % m_Data.size();

            return InIndex < offset ? InIndex + (m_Size - offset) : InIndex - offset;
        }

        std::vector<std::optional<T>> m_Data;
        u64 m_HeadIndex = 0;
        u64 m_TailIndex = 0;
        size_type m_Size = 0;
    };
}