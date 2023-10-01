add_library(MSUnitTestFramework SHARED IMPORTED)
set_target_properties(MSUnitTestFramework PROPERTIES
  INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_GENERATOR_INSTANCE}/VC/Auxiliary/VS/UnitTest/include"
  IMPORTED_IMPLIB "${CMAKE_GENERATOR_INSTANCE}/VC/Auxiliary/VS/UnitTest/lib/x64/Microsoft.VisualStudio.TestTools.CppUnitTestFramework.lib"
)
set(MSUnitTestFramework_FOUND TRUE)