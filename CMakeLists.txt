cmake_minimum_required(VERSION 3.16)
project(sample_grid_sort VERSION 0.1.0 LANGUAGES CXX)

if(NOT CMAKE_CXX_STANDARD)
    set(CMAKE_CXX_STANDARD 17)
endif()
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(SGS_BUILD_TESTS      "Build unit tests"        ON)
option(SGS_BUILD_BENCHMARKS "Build benchmark binary"  OFF)
option(SGS_BUILD_EXAMPLES   "Build example programs"  ON)
option(SGS_WARNINGS_AS_ERRORS "Treat warnings as errors" OFF)

add_library(sample_grid_sort INTERFACE)
add_library(sample_grid_sort::sample_grid_sort ALIAS sample_grid_sort)

target_include_directories(sample_grid_sort INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)
target_compile_features(sample_grid_sort INTERFACE cxx_std_17)

if(SGS_WARNINGS_AS_ERRORS)
    if(MSVC)
        target_compile_options(sample_grid_sort INTERFACE /W4 /WX)
    else()
        target_compile_options(sample_grid_sort INTERFACE -Wall -Wextra -Werror)
    endif()
endif()

if(SGS_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

if(SGS_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

if(SGS_BUILD_BENCHMARKS)
    add_subdirectory(benchmarks)
endif()

# -------- install --------
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(TARGETS sample_grid_sort EXPORT sample_grid_sortTargets)
install(EXPORT sample_grid_sortTargets
    NAMESPACE sample_grid_sort::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/sample_grid_sort
)

write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/sample_grid_sortConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)
configure_package_config_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/sample_grid_sortConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/sample_grid_sortConfig.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/sample_grid_sort
)
install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/sample_grid_sortConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/sample_grid_sortConfigVersion.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/sample_grid_sort
)
