
#include <memory>

#include <wrl/client.h>

template<typename T>
using UniquePtr = std::unique_ptr<T>;

template<typename T>
using SharedPtr = std::shared_ptr<T>;

using Microsoft::WRL::ComPtr;

template<typename T, typename... Ts>
auto MakeUnique(Ts&&... InArgs)
{
	return std::make_unique<T>(std::forward<Ts>(InArgs)...);
}

