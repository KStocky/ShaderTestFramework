#include "/Test/Public/ShaderTestFramework.hlsli"

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_TwoCallsToCounter_WHEN_Compared_THEN_AreDifferent(uint3 DispatchThreadId : SV_DispatchThreadID)
{
	int a = __COUNTER__;
	int b = __COUNTER__;
	if (a == 0 && b == 1)
	{
		ShaderTestPrivate::Success();
	}
	else
	{
		ShaderTestPrivate::AddError();
	}
}

struct StaticArrayTest
{
	int Val;
	bool Other;
};

static StaticArrayTest globalArray[100];

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_StaticGlobalArray_WHEN_Inspected_THEN_AllZeroed(uint3 DispatchThreadId : SV_DispatchThreadID)
{
	for (int i = 0; i < 100; ++i)
	{
		STF::AreEqual(globalArray[i].Val, 0);
		STF::IsFalse(globalArray[i].Other);
	}
}


static int CurrentTracker = 0;

struct ScopedTracker
{
	int ID;
	int ParentID;
	bool SectionHasBeenEntered;
	bool EnteredSubSection;
	bool HasSubSectionsRemaining;
	bool HasBeenInitialized;
};

static const int NumTrackers = 32;
static ScopedTracker Trackers[NumTrackers];

bool Init(int InID)
{
	bool result = false;
	Trackers[InID].EnteredSubSection = false;
	if (!Trackers[InID].HasBeenInitialized)
	{
		Trackers[InID].SectionHasBeenEntered = false;
		Trackers[InID].HasSubSectionsRemaining = false;
		Trackers[InID].HasBeenInitialized = true;
		Trackers[InID].ID = InID;
		result = true;
	}
	else
	{
		result = !Trackers[InID].SectionHasBeenEntered || Trackers[InID].HasSubSectionsRemaining;
	}

	if (result && Trackers[InID].ID == 0)
	{
		Trackers[InID].ParentID = -1;
		Trackers[InID].SectionHasBeenEntered = true;
		CurrentTracker = 0;
	}
	return result;
}

bool TryEnterSection(int InID)
{
	const bool needsExecution = Init(InID);
	const bool ourTurn = !Trackers[CurrentTracker].EnteredSubSection;
	if (needsExecution)
	{
		if (ourTurn)
		{
			Trackers[CurrentTracker].EnteredSubSection = true;
			Trackers[InID].ParentID = Trackers[CurrentTracker].ID;
			Trackers[InID].SectionHasBeenEntered = true;
			Trackers[CurrentTracker].HasSubSectionsRemaining = false;
			CurrentTracker = Trackers[InID].ID;
			return true;
		}
		else
		{

			Trackers[CurrentTracker].HasSubSectionsRemaining = true;
		}
	}
	return false;
}

void OnLeave()
{
	CurrentTracker = Trackers[CurrentTracker].ParentID;
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void SectionTest(uint3 DispatchThreadId : SV_DispatchThreadID)
{
	int num = 0;
	const int Section_0Num = 0;
	while (Init(Section_0Num))
	{
		++num;

		const int Section_1Num = 1;
		if (TryEnterSection(Section_1Num))
		{
			++num;

			OnLeave();
		}

		const int Section_2Num = 2;
		if (TryEnterSection(Section_2Num))
		{
			++num;

			const int Section_3Num = 3;
			if (TryEnterSection(Section_3Num))
			{
				++num;
				OnLeave();
			}
			OnLeave();
		}
	}  

	if (num == 5)
	{
		ShaderTestPrivate::Success();
	}
	else
	{
		ShaderTestPrivate::AddError();
	}
}

namespace SizeOfTests
{
	struct A
	{
		int A;
	};

	struct B
	{
		int A;
		int B;
	};
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_TwoDifferentSizedStructs_WHEN_sizeofCalledOn_Them_THEN_CorrectSizeReported(uint3 DispatchThreadId : SV_DispatchThreadID)
{
	const uint expectedA = 4;
	const uint expectedB = 8;
	STF::AreEqual(expectedA, (uint)sizeof(SizeOfTests::A));
	STF::AreEqual(expectedB, (uint)sizeof(SizeOfTests::B));
}
