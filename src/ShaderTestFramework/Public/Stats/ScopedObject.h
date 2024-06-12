
#include "Utility/Concepts.h"

template<CallableType CallOnDestruction>
class ScopedObject
{
public:
    template<CallableType CallOnConstruction>
    ScopedObject(CallOnConstruction&& OnConstruction, CallOnDestruction OnDestruction)
        : m_OnDestruction(std::move(OnDestruction))
    {
        OnConstruction();
    }

    ~ScopedObject() noexcept
    {
        m_OnDestruction();
    }

    ScopedObject(const ScopedObject&) = delete;
    ScopedObject(ScopedObject&&) = delete;
    ScopedObject& operator=(const ScopedObject&) = delete;
    ScopedObject& operator=(ScopedObject&&) = delete;

    template<typename... Ts>
    static void* operator new(std::size_t, Ts&&...) = delete;

    template<typename... Ts>
    static void* operator new[](std::size_t, Ts&&...) = delete;

private:

    CallOnDestruction m_OnDestruction;
};
