set(HEADER_LIST "${GRay_SOURCE_DIR}/include/GRay/math.hpp")

add_library(GRayV2Lib math.cpp)
target_include_directories(GRayV2Lib PUBLIC ../include)
target_compile_features(GRayV2Lib PUBLIC cxx_std_11)

source_group(
    TREE "${PROJECT_SOURCE_DIR}/include"
    PREFIX "Header Files"
    FILES ${HEADER_LIST}
)