#include "D3D12/Shader/VirtualShaderDirectoryMappingManager.h"

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

SCENARIO("VirtualShaderDirectoryMappingManagerTests")
{
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
	}

	GIVEN("Manger does have the requested mapping")
	{
		const std::filesystem::path realStem = "/This/Exists.hlsl";
		const std::filesystem::path realRoot = "C:/";

		const std::filesystem::path virtualMapping = "/Virtual";

		const auto addResult = manager.AddMapping({ virtualMapping, realRoot });

		REQUIRE(addResult == VirtualShaderDirectoryMappingManager::EErrorType::Success);

		WHEN("file with virtual root requested")
		{
			std::filesystem::path virtualPath = virtualMapping;
			virtualPath += realStem;
			const auto result = manager.Map(virtualPath);

			THEN("mapping succeeded")
			{
				REQUIRE(result.has_value());

				AND_THEN("result has expected path")
				{
					std::filesystem::path realPath = realRoot;
					realPath += realStem;
					REQUIRE(result.value() == realPath);
				}
			}
		}

		WHEN("file with different virtual root requested")
		{
			std::filesystem::path virtualPath = virtualMapping;
			virtualPath += "Foo/";
			virtualPath += realStem;
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
	}
}
