#pragma once

#include "Framework/AssertionInterface.h"
#include "Utility/Error.h"
#include "Utility/OverloadSet.h"

#include <ostream>
#include <utility>
#include <variant>

namespace stf::assert
{
    template<CTestRunResultsType TResults>
    class Results
    {
    public:

        Results() = default;
        Results(Error InError)
            : m_Result(std::move(InError))
        {
        }
        Results(TResults InResults)
            : m_Result(std::move(InResults))
        {
        }

        operator bool() const
        {
            return std::visit(OverloadSet
                {
                    [](std::monostate) { return false; },
                    [](const TResults& InTestResults) { return InTestResults.Succeeded(); },
                    [](const Error&) { return false; }
                }, m_Result);
        }

        const TResults* GetTestResults() const
        {
            return std::get_if<TResults>(&m_Result);
        }

        const Error* GetTestRunError() const
        {
            return std::get_if<Error>(&m_Result);
        }

        friend std::ostream& operator<<(std::ostream& InOs, const Results& In)
        {
            std::visit(OverloadSet
                {
                    [&InOs](std::monostate) { InOs << "Results not initialized"; },
                    [&InOs](const TResults& InTestResults) { InOs << InTestResults; },
                    [&InOs](const Error& InCompilationError) { InOs << InCompilationError; }
                }, In.m_Result);

            return InOs;
        }

    private:
        std::variant<std::monostate, TResults, Error> m_Result;
    };
}
