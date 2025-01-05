#include "D3D12/CommandEngine.h"

namespace stf
{
    CommandEngine::CommandEngine(ObjectToken InToken, CreationParams InParams)
        : Object(InToken)
        , m_Device(InParams.Device)
        , m_Queue(InParams.Device->CreateCommandQueue(
            {
                .Type = D3D12_COMMAND_LIST_TYPE_DIRECT,
                .Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL,
                .Flags = D3D12_COMMAND_QUEUE_FLAG_NONE,
                .NodeMask = 0
            },
            "Command Engine Direct Queue"))
        , m_List(InParams.Device->CreateCommandList(
            D3D12_COMMAND_LIST_TYPE_DIRECT,
            "Command Engine Direct List"
        ))
        , m_Allocators()
    {
    }

    void CommandEngine::Flush()
    {
        m_Queue->FlushQueue();
    }
}
