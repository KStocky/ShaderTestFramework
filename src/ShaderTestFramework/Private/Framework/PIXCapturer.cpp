#include "Framework/PIXCapturer.h"

#include "Utility/Exception.h"

#include <d3d12.h>
#include <filesystem>

#include <WinPixEventRuntime/pix3.h>

PIXCapturer::PIXCapturer(const std::string_view InName, const bool InTakeCapture)
    : m_IsCapturing(InTakeCapture)
{
    if (m_IsCapturing)
    {
        const auto capturePath = std::format(L"{}.wpix", std::filesystem::path{ InName }.c_str());
        const std::filesystem::path captureDir = std::filesystem::current_path() / L"Captures";

        std::filesystem::create_directories(captureDir);

        const std::filesystem::path captureFilePath = captureDir / capturePath;
        PIXCaptureParameters params;
        params.GpuCaptureParameters.FileName = captureFilePath.c_str();
        ThrowIfFailed(PIXBeginCapture(PIX_CAPTURE_GPU, &params));
    }
}

PIXCapturer::~PIXCapturer() noexcept
{
    if (m_IsCapturing)
    {
        ThrowIfFailed(PIXEndCapture(false));
    }
}
