#include "Framework/TestDataBufferStructs.h"

namespace stf
{
    static_assert(sizeof(HLSLAssertMetaData) == 32,
        "The size of this struct must be the same size as ShaderTestPrivate::HLSLAssertMetaData");

    std::ostream& operator<<(std::ostream& InOs, const SectionInfoMetaData& In)
    {
        InOs << "SectionId: " << In.SectionId << "\nStringId: " << In.StringId << "\nParentId: " << In.ParentId << "\n";
        return InOs;
    }
}
