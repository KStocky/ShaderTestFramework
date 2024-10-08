#pragma once
#include <memory>

#include <wrl/client.h>

namespace stf
{
    template<typename T>
    using UniquePtr = std::unique_ptr<T>;

    template<typename T>
    using SharedPtr = std::shared_ptr<T>;

    template<typename T>
    using ComPtr = Microsoft::WRL::ComPtr<T>;

    template<typename T, typename... Ts>
    auto MakeUnique(Ts&&... InArgs)
    {
        return std::make_unique<T>(std::forward<Ts>(InArgs)...);
    }

    template<typename T, typename... Ts>
    auto MakeShared(Ts&&... InArgs)
    {
        return std::make_shared<T>(std::forward<Ts>(InArgs)...);
    }
}

