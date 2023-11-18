#include "D3D12/CommandEngine.h"

CommandEngine::CommandEngine(CreationParams InParams)
	: m_Device(std::move(InParams.Device))
	, m_Queue(std::move(InParams.Queue))
	, m_List(std::move(InParams.List))
	, m_Allocators()
{
}

void CommandEngine::Flush()
{
	m_Queue.FlushQueue();
}
