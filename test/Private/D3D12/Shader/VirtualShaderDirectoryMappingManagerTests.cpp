#include "D3D12/Shader/VirtualShaderDirectoryMappingManager.h"

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

SCENARIO("VirtualShaderDirectoryMappingManagerTests")
{
    using namespace stf;
	VirtualShaderDirectoryMappingManager manager;

	GIVEN("Manager does not have requested mapping")
	{
		WHEN("mapping requested")
		{
			const auto result = manager.Map("/This/Does/Not/Exist.hlsl");

			THEN("error returned")
			{
				REQUIRE(!result.has_value());

				AND_THEN("result is that mapping is not found")
				{
					REQUIRE(result.error() == VirtualShaderDirectoryMappingManager::EErrorType::NoMappingFound);
				}
			}
		}

		WHEN("mapping with empty virtual path")
		{
			const auto result = manager.AddMapping({L"", L"C:/Hi"});

			THEN("map addition failed")
			{
				REQUIRE(result == VirtualShaderDirectoryMappingManager::EErrorType::VirtualPathEmpty);
			}
		}

		WHEN("mapping that does not start with slash added")
		{
			const auto result = manager.AddMapping({ L"Virtual", L"C:/Hi" });

			THEN("map addition failed")
			{
				REQUIRE(result == VirtualShaderDirectoryMappingManager::EErrorType::VirtualPathShouldStartWithASlash);
			}
		}

		WHEN("mapping has non absolute real path")
		{
			const auto result = manager.AddMapping({ L"/Virtual", L"../Hi" });

			THEN("map addition failed")
			{
				REQUIRE(result == VirtualShaderDirectoryMappingManager::EErrorType::RealPathMustBeAbsolute);
			}
		}
	}

	GIVEN("Manager does have a mapping")
	{
		const std::filesystem::path realRoot = "C:/";

		const std::filesystem::path virtualMapping = "/Virtual";

		const auto addResult = manager.AddMapping({ virtualMapping, realRoot });

		REQUIRE(addResult == VirtualShaderDirectoryMappingManager::EErrorType::Success);

		WHEN("file with existing virtual mapping requested")
		{
			std::filesystem::path virtualPath = "/Virtual/This/Exists.hlsl";
			const auto result = manager.Map(virtualPath);

			THEN("mapping succeeded")
			{
				const auto error = result.has_value() ? VirtualShaderDirectoryMappingManager::EErrorType::Success : result.error();
				CAPTURE(error);
				REQUIRE(result.has_value());

				AND_THEN("result has expected path")
				{
					std::filesystem::path realPath = realRoot;
					realPath += "This/Exists.hlsl";
					REQUIRE(result.value() == realPath);
				}
			}
		}

		WHEN("file with different virtual mapping requested")
		{
			std::filesystem::path virtualPath = virtualMapping;
			virtualPath += "Foo/";
			virtualPath += "Failed.hlsl";
			const auto result = manager.Map(virtualPath);

			THEN("error returned")
			{
				REQUIRE(!result.has_value());

				AND_THEN("result is that mapping is not found")
				{
					REQUIRE(result.error() == VirtualShaderDirectoryMappingManager::EErrorType::NoMappingFound);
				}
			}
		}

		WHEN("new mapping that starts the same is added")
		{
			std::filesystem::path newVirtualMapping = virtualMapping;
			newVirtualMapping += "Foo";
			const std::filesystem::path newRealRoot = "D:/Cool";

			const auto mappingResult = manager.AddMapping({ newVirtualMapping, newRealRoot });

			THEN("map addition succeeded")
			{
				CAPTURE(mappingResult);
				REQUIRE(mappingResult == VirtualShaderDirectoryMappingManager::EErrorType::Success);

				AND_WHEN("mapping requested with new virtual path")
				{
					std::filesystem::path virtualPath = newVirtualMapping;
					virtualPath += "/Exists.hlsl";
					const auto result = manager.Map(virtualPath);

					THEN("mapping succeeds")
					{
						const auto error = result.has_value() ? VirtualShaderDirectoryMappingManager::EErrorType::Success : result.error();
						CAPTURE(error);
						REQUIRE(result.has_value());

						AND_THEN("result has expected path")
						{
							std::filesystem::path realPath = newRealRoot;
							realPath += "/Exists.hlsl";
							REQUIRE(result.value() == realPath);
						}
					}
				}
			}
		}

		WHEN("new mapping that has same root as previous mapping")
		{
			std::filesystem::path newVirtualMapping = virtualMapping;
			newVirtualMapping += "/Foo";
			const std::filesystem::path newRealRoot = "D:/Cool";

			const auto mappingResult = manager.AddMapping({ newVirtualMapping, newRealRoot });

			THEN("map addition failed")
			{
				CAPTURE(mappingResult);
				REQUIRE(mappingResult == VirtualShaderDirectoryMappingManager::EErrorType::VirtualPathAlreadyExists);
			}
		}
	}
}
