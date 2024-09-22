#include "/Test/STF/ShaderTestFramework.hlsli"

namespace SFINAETemplateSpecialization
{
	template<typename T>
	struct A
	{
		static const bool value = false;
	};

	template<>
	struct A<int>
	{
		static const bool value = true;
		static uint Func()
		{
			return 1;
		}
	};

	template<>
	struct A<float>
	{
		static const bool value = true;
		static uint Func()
		{
			return 2;
		}
	};

	template<typename T>
	static typename ttl::enable_if<A<T>::value, uint>::type Apply()
	{
		return A<T>::Func();
	}

	template<typename T>
	static typename ttl::enable_if<!A<T>::value, uint>::type Apply()
	{
		return 0;
	}
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_SomeTypesWithAndWithoutASpecializations_WHEN_ApplyFuncCalledOnThem_THEN_ExpectedResultsReturned(uint3 DispatchThreadId : SV_DispatchThreadID)
{
	ASSERT(AreEqual, 0u, SFINAETemplateSpecialization::Apply<bool>());
	ASSERT(AreEqual, 1u, SFINAETemplateSpecialization::Apply<int>());
	ASSERT(AreEqual, 2u, SFINAETemplateSpecialization::Apply<float>());
}