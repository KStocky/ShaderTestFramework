[Reference](./ShaderTestFramework.md)

# Setting Up PIX

You can download PIX for free from [here](https://devblogs.microsoft.com/pix/download/). Shader Test Framework is able to take PIX captures of tests. By default the framework will use [WARP](https://learn.microsoft.com/en-us/windows/win32/direct3darticles/directx-warp) to run the tests. This is to ensure that they will run on most hardware. The issue with this is that captures taken using WARP can not be analyzed in PIX out of the box there is an extra step to take.

1) Navigate to the location of where Shader Test Framework's WARP driver lives - If the project was installed using CMake's FetchContent library, this will be located at `build\_deps\<Fetch Content Name for STF>-src\src\ShaderTestFramework\nuget\Microsoft.Direct3D.WARP\<Version>\build\native\amd64` For example if your `FetchContent_Declare` looks like the following:

```cmake
FetchContent_Declare(
  stf
  GIT_REPOSITORY https://github.com/KStocky/ShaderTestFramework
  GIT_TAG main
)
```

and the version of WARP that STF uses is `1.0.8` then the path will be `build\_deps\stf-src\src\ShaderTestFramework\nuget\Microsoft.Direct3D.WARP\1.0.8\build\native\amd64`. If you just clones the repository then after running cmake to build the project, the path will be `src\ShaderTestFramework\nuget\Microsoft.Direct3D.WARP\1.0.8\build\native\amd64`.

2) Copy the dll that was found in step 1, into your PIX installation. The default installation will be something like `C:\Program Files\Microsoft PIX\2312.08`. You will need admininistrator rights to successfully do this.

Once the above steps have been completed you will be able to analyze PIX captures taken using the WARP driver. This process is NOT required if you explicitly tell the framework to use your GPU to run the tests.
