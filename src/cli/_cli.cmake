add_executable(${PROJECT_NAME}
        "${CMAKE_CURRENT_SOURCE_DIR}/src/cli/bsp2sfm.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/common/Config.hpp")
cs_configure_target(${PROJECT_NAME} LOGO MANIFEST)

target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src/cli" "${CMAKE_CURRENT_SOURCE_DIR}/src/common")

target_link_libraries(${PROJECT_NAME} PRIVATE argparse::argparse sourcepp::bsppp sourcepp::vtfpp)
