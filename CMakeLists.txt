cmake_minimum_required(VERSION 3.16)
project(invport
        VERSION 0.0.1
        LANGUAGES CXX
        DESCRIPTION "C++ investments tracker."
        )
message(STATUS "${PROJECT_NAME} version: ${PROJECT_VERSION}")

# Options related to weed whacking, testing, and documentation.
option(BUILD_DOCUMENTATION "Build documentation." OFF)
option(BUILD_TESTING "Build unit testing." ON)
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

# Set executable sources
set(exec_main ${PROJECT_NAME}/main.cc)

set(exec_sources
        invport.cc
        invport.h
        detail/env.cc
        detail/env.h
        detail/file_serializable.cc
        detail/file_serializable.h
        detail/keychain.cc
        detail/keychain.h
        detail/common.h
        detail/transaction.cc
        detail/transaction.h
        detail/transaction_history.cc
        detail/transaction_history.h
        detail/utils.cc
        detail/utils.h
        detail/vanguard.cc
        detail/vanguard.h
        widget/base.h
        widget/util.h
        widget/key_selector.cc
        widget/key_selector.h
        widget/transaction_creator.cc
        widget/transaction_creator.h
        widget/main_window.cc
        widget/main_window.h
        widget/transactions.cc
        widget/transactions.h
        )

# Specify source directory.
list(TRANSFORM exec_sources PREPEND "${PROJECT_NAME}/")

# Find dependencies.
find_package(PkgConfig)
pkg_check_modules(GTKMM REQUIRED gtkmm-3.0)
find_package(iex REQUIRED)
find_package(spdlog REQUIRED)
find_package(Doxygen)

# Define LIBDIR, INCLUDEDIR, DOCDIR
include(GNUInstallDirs)

# gtkmm3 include dirs
include_directories(SYSTEM ${GTKMM_INCLUDE_DIRS})

# Create executable if specified.
set(EXEC_NAME _${PROJECT_NAME})
add_executable(${EXEC_NAME} ${exec_main} ${exec_sources})
target_include_directories(${EXEC_NAME} PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_BINDIR}>
        )
target_compile_options(${EXEC_NAME} PRIVATE ${EXTRA_COMPILE_OPTIONS})

target_link_libraries(${EXEC_NAME} ${GTKMM_LIBRARIES} iex::iex spdlog::spdlog)

#Install
install(TARGETS ${EXEC_NAME}
        DESTINATION ${CMAKE_INSTALL_BINDIR}/${PROJECT_NAME}
        COMPONENT runtime
        )

install(DIRECTORY
        share/${PROJECT_NAME}
        DESTINATION ${CMAKE_INSTALL_DATADIR})

install(FILES
        LICENSE
        DESTINATION ${CMAKE_INSTALL_DATADIR}/licenses/${PROJECT_NAME})

# Build GTest if unit testing enabled.
if (BUILD_TESTING)
    add_library(invport_lib STATIC ${exec_sources})
    target_include_directories(invport_lib PUBLIC
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
            $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
            )

    target_link_libraries(invport_lib ${GTKMM_LIBRARIES} iex::iex spdlog::spdlog)

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
