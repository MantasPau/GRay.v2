add_executable(GRay main.cpp)
target_compile_features(GRay PRIVATE cxx_std_11)
target_link_libraries(GRay PRIVATE GRayV2Lib)
