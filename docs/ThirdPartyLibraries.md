[Reference](./ShaderTestFramework.md)

# Third Party Libraries used by Shader Test Framework

| Library | Notes |
|---------|-------|
| [DirectX 12 AgilitySDK](https://www.nuget.org/packages/Microsoft.Direct3D.D3D12/) | Provides the most up to date D3D12 dlls and headers. [More Info](https://devblogs.microsoft.com/directx/gettingstarted-dx12agility/) |
| [WARP](https://www.nuget.org/packages/Microsoft.Direct3D.WARP) | Software implementation of D3D. Enables STF to run on machines without a GPU (or a without a GPU that supports the features required by STF) [More Info](https://learn.microsoft.com/en-us/windows/win32/direct3darticles/directx-warp) |
| [DXC](https://www.nuget.org/packages/Microsoft.Direct3D.DXC) | HLSL compiler for compiler shaders. [Github Repo](https://github.com/microsoft/DirectXShaderCompiler) |
| [WinPixEventRuntime](https://www.nuget.org/packages/WinPixEventRuntime) | STF uses this library to take programmatic PIX captures to help debug failing shader tests. [More Info](https://devblogs.microsoft.com/pix/winpixeventruntime/) |
| [Catch2](https://github.com/catchorg/Catch2) | The main testing framework used by STF |
| [Tuplet](https://github.com/codeinred/tuplet) | A fast replacement for [`std::tuple`](https://en.cppreference.com/w/cpp/utility/tuple) |
| [float16_t](https://github.com/KStocky/float16_t) | A 16 bit floating point library that is a fork of a fork |
| [setup-ninja](https://github.com/seanmiddleditch/gha-setup-ninja) | Github Action to install ninja on a github actions agents |
| [msvc-dev-cmd](https://github.com/ilammy/msvc-dev-cmd) | Github Action which will set up an MSVC Development environment |
| [Publish Test Results](https://github.com/EnricoMi/publish-unit-test-result-action) | Github action for reporting test results at the end of a github workflow |
| [action-junit-report ](https://github.com/mikepenz/action-junit-report) | Github action for reporting test results at the end of a github workflow |
| [Linkspector](https://github.com/UmbrellaDocs/action-linkspector) | Github action for validating that links aren't broken in documentation |
| [setup-msvc-dev](https://github.com/TheMrMilchmann/setup-msvc-dev) | Github Action which will set up an MSVC Development environment |
| [Bring Your Own Badge](https://github.com/RubbaBoy/BYOB) | Github action for creating custom badges using [Badgen](https://badgen.net/) |
| [Dynamic Badges Action](https://github.com/Schneegans/dynamic-badges-action) | Github action for creating custom badges using [shields.io](https://shields.io/) |
| [dependent-jobs-result-check](https://github.com/lwhiteley/dependent-jobs-result-check) | Github action for aggregating the results of dependent jobs |
