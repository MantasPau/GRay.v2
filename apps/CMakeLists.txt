add_executable(GRay main.cpp)
target_compile_features(GRay PRIVATE cxx_std_11)
target_link_libraries(GRay PRIVATE GRayV2Lib)

add_executable(GRayFinal01 final01.cpp)
target_compile_features(GRayFinal01 PRIVATE cxx_std_11)
target_link_libraries(GRayFinal01 PRIVATE GRayV2Lib)
