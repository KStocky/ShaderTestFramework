
#include "TestUtilities/Noisy.h"

DefaultLogData DefaultNoisyLogger::LogData;

std::ostream& operator<<(std::ostream& InStream, const DefaultLogData& InData)
{
    InStream << "Num Constructions: " << InData.NumConstructions << "\n";
    InStream << "Num Copy Constructions: " << InData.NumCopyConstructions << "\n";
    InStream << "Num Copy Assignments: " << InData.NumCopyAssignments << "\n";
    InStream << "Num Move Constructions: " << InData.NumMoveConstructions << "\n";
    InStream << "Num Move Assignments: " << InData.NumMoveAssignments << "\n";
    InStream << "Num Destructions: " << InData.NumDestructions << "\n";
    InStream << "Events: Num = " << InData.NumEvents.size() << "\n";

    if (InData.NumEvents.size() > 0)
    {
        InStream << "----------------------------\n";

        for (const auto [key, value] : InData.NumEvents)
        {
            InStream << "Key: " << key << "\tValue: " << value << "\n";
        }

        InStream << "----------------------------\n";
    }

    return InStream;
}
