#pragma once

#include "Utility/MoveOnly.h"
#include "Platform.h"
#include "Utility/Concepts.h"
#include "Utility/Pointer.h"

#include <bit>
#include <span>
#include <vector>

#include <d3d12.h>

class CommandAllocator;
class DescriptorHeap;
class GPUResource;
class PipelineState;
class RootSignature;

template<typename T>
concept RootSigConstantType = std::is_trivial_v<T> && sizeof(T) == 4;

class CommandList : MoveOnly
{
public:

	struct CreationParams
	{
		ComPtr<ID3D12GraphicsCommandList9> List = nullptr;
	};

	CommandList() = default;
	CommandList(CreationParams InParams);

	void Dispatch(const u32 InX, const u32 InY, const u32 InZ);

	template<RootSigConstantType T>
	void SetComputeRoot32BitConstants(const u32 InRootParamIndex, const T InVal, const u32 InOffset)
	{
		const auto val = std::bit_cast<u32>(InVal);
		m_List->SetComputeRoot32BitConstant(InRootParamIndex, val, InOffset);
	}

	template<RootSigConstantType T, u64 InSpanSize>
	void SetComputeRoot32BitConstants(const u32 InRootParamIndex, const std::span<T, InSpanSize> InVals, const u32 InOffset)
	{
		m_List->SetComputeRoot32BitConstants(InRootParamIndex, static_cast<u32>(InVals.size()), InVals.data(), InOffset);
	}

	void SetComputeRootSignature(const RootSignature& InRootSig);

	void SetDescriptorHeaps(const DescriptorHeap& InHeap);

	template<RootSigConstantType T>
	void SetGraphicsRoot32BitConstants(const u32 InRootParamIndex, const T InVal, const u32 InOffset)
	{
		const auto val = std::bit_cast<u32>(InVal);
		m_List->SetGraphicsRoot32BitConstant(InRootParamIndex, val, InOffset);
	}

	template<RootSigConstantType T, u64 InSpanSize>
	void SetGraphicsRoot32BitConstants(const u32 InRootParamIndex, const std::span<T, InSpanSize> InVals, const u32 InOffset)
	{
		m_List->SetGraphicsRoot32BitConstants(InRootParamIndex, static_cast<u32>(InVals.size()), InVals.data(), InOffset);
	}

	void SetGraphicsRootSignature(const RootSignature& InRootSig);
	void SetPipelineState(const PipelineState& InState);

	void SetBufferUAV(GPUResource& InResource);

	void Close();
	void Reset(CommandAllocator& InAllocator);

	D3D12_COMMAND_LIST_TYPE GetType() const;

	ID3D12CommandList* GetRaw() const;
	operator ID3D12CommandList* () const;

private:

	ComPtr<ID3D12GraphicsCommandList9> m_List = nullptr;
};