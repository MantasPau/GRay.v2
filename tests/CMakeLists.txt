FetchContent_Declare(
    catch
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG v2.13.6
)
FetchContent_MakeAvailable(catch)

add_executable(testMathLib testMathLib.cpp)
target_compile_features(testMathLib PRIVATE cxx_std_11)
target_link_libraries(testMathLib PRIVATE GRayV2Lib Catch2::Catch2)

add_test(NAME math_lib_test COMMAND testMathLib)