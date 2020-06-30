cmake_minimum_required(VERSION 3.16)
project(__cpptemplate_name__
        VERSION 0.0.1
        LANGUAGES CXX
        DESCRIPTION "__cpptemplate_description__"
        )
message(STATUS "${PROJECT_NAME} version: ${PROJECT_VERSION}")

# Options related to weed whacking, testing, and documentation.
option(BUILD_EXECUTABLE "Build executable." ON)
option(BUILD_LIBRARY "Build library." ON)
option(BUILD_DOCUMENTATION "Build documentation." OFF)
option(BUILD_TESTING "Build unit testing." OFF)
option(ENABLE_CXX_WARNINGS "Enable GCC/Clang compatible compile options." OFF)

# Set configuration: Either Debug, Release (default), MinSizeRel, or RelWithDebInfo.
if (NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(default_build_type "Release")
    message(STATUS "Setting build type to '${default_build_type}' as none was specified.")
    set(CMAKE_BUILD_TYPE "${default_build_type}" CACHE
            STRING "Choose the type of build." FORCE)
    # Set the possible values of build type for cmake-gui
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY
            STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif ()

# Set C++17 unless specified otherwise.
if (NOT CMAKE_CXX_STANDARD)
    set(CMAKE_CXX_STANDARD 17)
    message(STATUS "${PROJECT_NAME} using CMAKE_CXX_STANDARD: ${CMAKE_CXX_STANDARD}")
endif ()

# Set compiler warning flags if option is on, and using GCC/Clang.
if (ENABLE_CXX_WARNINGS)
    if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        set(EXTRA_COMPILE_OPTIONS
                -Wall -Wextra -Werror -pedantic -Wcast-align -Wcast-qual
                -Wctor-dtor-privacy -Wdisabled-optimization -Wformat=2
                -Winit-self -Wmissing-include-dirs -Wold-style-cast
                -Woverloaded-virtual -Wredundant-decls -Wshadow
                -Wsign-promo -Wswitch-default -Wundef -Wno-unused
                CACHE STRING "Add extra compile options to the CXX targets")
    endif ()
endif ()

message(STATUS "EXTRA_COMPILE_OPTIONS: ${EXTRA_COMPILE_OPTIONS}")

# Set library headers and sources.
set(lib_headers
        ${PROJECT_NAME}.h
        )
set(lib_sources
        ${PROJECT_NAME}.cc
        )

# Set executable sources
set(exec_sources
        main.cc
        )

# Specify source directory.
list(TRANSFORM lib_headers PREPEND "${PROJECT_NAME}/")
list(TRANSFORM lib_sources PREPEND "${PROJECT_NAME}/")
list(TRANSFORM exec_sources PREPEND "${PROJECT_NAME}/")

# Find dependencies.
find_package(Doxygen)

# Define LIBDIR, INCLUDEDIR, DOCDIR
include(GNUInstallDirs)

# Create library if specified
if (BUILD_LIBRARY)
    # Create shared library.
    add_library(${PROJECT_NAME} SHARED ${lib_sources})
    target_include_directories(${PROJECT_NAME} PUBLIC
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
            $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
            )
    target_compile_options(${PROJECT_NAME} PRIVATE ${EXTRA_COMPILE_OPTIONS})

    # Link libaries
    # target_link_libraries(${PROJECT_NAME})

    # Install shared object.
    install(TARGETS ${PROJECT_NAME}
            EXPORT "${PROJECT_NAME}Targets"
            RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT runtime
            LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT runtime
            ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT development
            )

    # Install public header(s).
    install(FILES ${lib_headers}
            DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}
            COMPONENT development
            )

    # Export to the build tree.
    set(export_file "${PROJECT_BINARY_DIR}/${PROJECT_NAME}Targets.cmake")
    export(TARGETS ${PROJECT_NAME}
            NAMESPACE ${PROJECT_NAME}::
            APPEND FILE ${export_file}
            )

    set(install_cmake_dir "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}")

    install(EXPORT "${PROJECT_NAME}Targets"
            NAMESPACE ${PROJECT_NAME}::
            DESTINATION ${install_cmake_dir}
            )

    install(FILES
            ${CMAKE_CURRENT_BINARY_DIR}/cmake/${PROJECT_NAME}Config.cmake
            ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
            DESTINATION ${install_cmake_dir}
            )

    include(CMakePackageConfigHelpers)

    write_basic_package_version_file(${PROJECT_NAME}ConfigVersion.cmake
            VERSION ${PROJECT_VERSION}
            COMPATIBILITY SameMajorVersion
            )

    # Build tree
    set(TARGETS_FILE ${export_file})
    configure_package_config_file(
            ${CMAKE_CURRENT_SOURCE_DIR}/cmake/${PROJECT_NAME}Config.cmake.in
            ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake
            INSTALL_DESTINATION ${install_cmake_dir}
            PATH_VARS TARGETS_FILE
            NO_CHECK_REQUIRED_COMPONENTS_MACRO
            INSTALL_PREFIX ${CMAKE_CURRENT_BINARY_DIR}
    )

    # Install tree
    set(TARGETS_FILE ${CMAKE_INSTALL_PREFIX}/${install_cmake_dir}/${PROJECT_NAME}Targets.cmake)
    configure_package_config_file(
            ${CMAKE_CURRENT_SOURCE_DIR}/cmake/${PROJECT_NAME}Config.cmake.in
            ${CMAKE_CURRENT_BINARY_DIR}/cmake/${PROJECT_NAME}Config.cmake
            INSTALL_DESTINATION ${install_cmake_dir}
            PATH_VARS TARGETS_FILE
            NO_CHECK_REQUIRED_COMPONENTS_MACRO
    )

    # Add custom target to only install component: runtime (libraries)
    add_custom_target(${PROJECT_NAME}-install-runtime
            ${CMAKE_COMMAND}
            -DCMAKE_INSTALL_COMPONENT=runtime
            -P "${PROJECT_BINARY_DIR}/cmake_install.cmake"
            DEPENDS ${PROJECT_NAME}
            )

    add_dependencies(${PROJECT_NAME}-install-runtime ${PROJECT_NAME})
endif ()

# Create executable if specified.
if (BUILD_EXECUTABLE OR BUILD_LIBRARY)
    set(EXEC_NAME __cpptemplate_exec_name__)
    add_executable(${EXEC_NAME} ${exec_sources})
    target_include_directories(${EXEC_NAME} PUBLIC
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
            $<INSTALL_INTERFACE:${CMAKE_INSTALL_BINDIR}>
            )
    target_compile_options(${EXEC_NAME} PRIVATE ${EXTRA_COMPILE_OPTIONS})

    target_link_libraries(${EXEC_NAME} ${PROJECT_NAME})

    install(TARGETS ${EXEC_NAME}
            DESTINATION ${CMAKE_INSTALL_BINDIR}
            COMPONENT runtime
            )
endif ()

# Build GTest if unit testing enabled.
if (BUILD_TESTING)
    enable_testing()
    set(INSTALL_GTEST OFF)
    set(CMAKE_POLICY_DEFAULT_CMP0077 NEW) # Propagate INSTALL_GTEST=OFF to subproject
    set(GTEST_LIBRARIES gtest gtest_main)
    include(GoogleTest)
    #############################################################################
    # Fetch GTest
    include(FetchContent)

    FetchContent_Declare(
            googletest
            GIT_REPOSITORY https://github.com/google/googletest.git
            GIT_TAG v1.8.x
    )

    FetchContent_GetProperties(googletest)
    if (NOT googletest_POPULATED)
        FetchContent_Populate(googletest)
        add_subdirectory(${googletest_SOURCE_DIR} ${googletest_BINARY_DIR})
    endif ()

    add_subdirectory(${PROJECT_NAME}/test)
endif ()

# Build doxygen documentation if enabled.
if (BUILD_DOCUMENTATION)
    if (NOT DOXYGEN_FOUND)
        message(FATAL_ERROR "Doxygen is needed to build the documentation.")
    endif ()

    # Don't try to generate docs for testing.
    set(DOXYGEN_RECURSIVE NO)

    # Use README.md as main page.
    set(DOXYGEN_USE_MDFILE_AS_MAINPAGE ${CMAKE_CURRENT_SOURCE_DIR}/README.md)

    # Add doxygen sources.
    doxygen_add_docs(
            doc
            ALL
            ${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_NAME}
            ${CMAKE_CURRENT_SOURCE_DIR}/README.md
            ${CMAKE_CURRENT_SOURCE_DIR}/.github/CONTRIBUTING.md
            COMMENT "Generate documentation"
    )

    install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/html
            DESTINATION ${CMAKE_INSTALL_DOCDIR}
            COMPONENT development
            )
endif ()
