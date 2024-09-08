#include "Framework/TestDataBufferStructs.h"

namespace stf
{
    std::ostream& operator<<(std::ostream& InOs, const SectionInfoMetaData& In)
    {
        InOs << "SectionId: " << In.SectionId << "\nStringId: " << In.StringId << "\nParentId: " << In.ParentId << "\n";
        return InOs;
    }
}
