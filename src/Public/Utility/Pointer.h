#pragma once

#include "Utility/Concepts.h"
#include <memory>

#include <wrl/client.h>

namespace stf
{
    template<typename T>
    using UniquePtr = std::unique_ptr<T>;

    template<typename T>
    using SharedPtr = std::shared_ptr<T>;

    template<typename T>
    using SharedFromThis = std::enable_shared_from_this<T>;

    template<typename T>
    using ComPtr = Microsoft::WRL::ComPtr<T>;

    template<typename T, typename... Ts>
        requires std::constructible_from<T, Ts...>
    auto MakeUnique(Ts&&... InArgs)
    {
        return std::make_unique<T>(std::forward<Ts>(InArgs)...);
    }

    template<typename T, typename... Ts>
        requires std::constructible_from<T, Ts...>
    auto MakeShared(Ts&&... InArgs)
    {
        return std::make_shared<T>(std::forward<Ts>(InArgs)...);
    }
}

