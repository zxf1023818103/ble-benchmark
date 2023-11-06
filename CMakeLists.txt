cmake_minimum_required(VERSION 3.14)

project(ble-benchmark LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Git REQUIRED)

execute_process(COMMAND ${GIT_EXECUTABLE} log --pretty=format:'%h' -n 1
                OUTPUT_VARIABLE GIT_REV
                WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
                ERROR_QUIET)

if ("${GIT_REV}" STREQUAL "")
  set(GIT_REV "N/A")
  set(GIT_DIFF "")
  set(GIT_TAG "N/A")
  set(GIT_BRANCH "N/A")
else()
  execute_process(
    COMMAND ${GIT_EXECUTABLE} diff --quiet --exit-code
    OUTPUT_VARIABLE GIT_DIFF
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    RESULT_VARIABLE GIT_DIFF_RETURN)
  if (${GIT_DIFF_RETURN} EQUAL 0)
    set(GIT_DIFF "+")
  else()
    set(GIT_DIFF "-")
  endif()

  execute_process(
    COMMAND ${GIT_EXECUTABLE} describe --exact-match --tags
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE GIT_TAG ERROR_QUIET)
  execute_process(
    COMMAND ${GIT_EXECUTABLE} rev-parse --abbrev-ref HEAD
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE GIT_BRANCH)

  string(STRIP "${GIT_REV}" GIT_REV)
  string(SUBSTRING "${GIT_REV}" 1 7 GIT_REV)
  string(STRIP "${GIT_DIFF}" GIT_DIFF)
  string(STRIP "${GIT_TAG}" GIT_TAG)
  string(STRIP "${GIT_BRANCH}" GIT_BRANCH)
endif()

if ("${GIT_TAG}" STREQUAL "")
  set(GIT_TAG "0")
endif()

set(CPACK_PACKAGE_VERSION "${GIT_TAG}${GIT_DIFF}git.${GIT_REV}")

add_compile_definitions(GIT_REV="${GIT_REV}")
add_compile_definitions(GIT_BRANCH="${GIT_BRANCH}")
add_compile_definitions(GIT_TAG="${GIT_TAG}")
add_compile_definitions(VERSION="${CPACK_PACKAGE_VERSION}")

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Bluetooth)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Bluetooth)

add_executable(ble-benchmark
  main.cpp
  blebenchmarkapplication.h blebenchmarkapplication.cpp
)
target_link_libraries(ble-benchmark Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Bluetooth)

install(TARGETS ble-benchmark)

if(WIN32)
  set(CPACK_GENERATOR External)
else()
  set(CPACK_GENERATOR 7Z ZIP)
endif()

set(CPACK_PACKAGE_NAME ble-benchmark)
set(CPACK_PACKAGE_DESCRIPTION "BLE Benchmark")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "BLE Benchmark")
set(CPACK_PACKAGE_VENDOR "Zeng Xiangfei")
set(CPACK_PACKAGE_LICENSE "GPL-3.0")
set(CPACK_PACKAGE_CONTACT "zxf1023818103@gmail.com")

set(CPACK_DEBIAN_PACKAGE_DEPENDS "libqt6bluetooth6, libcap2-bin")
set(CPACK_DEBIAN_PACKAGE_HOMEPAGE "https://github.com/zxf1023818103/ble-benchmark")
set(CPACK_DEBIAN_PACKAGE_CONTROL_EXTRA ${CMAKE_SOURCE_DIR}/postinst)
if (${CMAKE_SYSTEM_PROCESSOR} MATCHES "x86_64")
  set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE "amd64")
elseif(${CMAKE_SYSTEM_PROCESSOR} MATCHES "aarch64")
  set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE "arm64")
elseif(${CMAKE_SYSTEM_PROCESSOR} MATCHES "powerpc64le")
  set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE "ppc64el")
else()
  set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE ${CMAKE_SYSTEM_PROCESSOR})
endif()

set(CPACK_RPM_PACKAGE_ARCHITECTURE ${CMAKE_SYSTEM_PROCESSOR})
set(CPACK_RPM_PACKAGE_LICENSE ${CPACK_PACKAGE_LICENSE})
set(CPACK_RPM_PACKAGE_GROUP "Unspecified")
#set(CPACK_RPM_PACKAGE_REQUIRES "qt6-qtconnectivity, libcap")
#set(CPACK_RPM_PACKAGE_REQUIRES_POST "")
set(CPACK_RPM_SPEC_MORE_DEFINE "%define debug_package %{nil}
  %define __os_install_post %{nil}")
set(CPACK_RPM_PACKAGE_ARCHITECTURE ${CMAKE_SYSTEM_PROCESSOR})
set(CPACK_RPM_PACKAGE_URL ${CPACK_DEBIAN_PACKAGE_HOMEPAGE})
set(CPACK_RPM_POST_INSTALL_SCRIPT_FILE ${CPACK_DEBIAN_PACKAGE_CONTROL_EXTRA})
set(CPACK_RPM_PACKAGE_RELOCATABLE ON)

set(CPACK_PROJECT_CONFIG_FILE ${CMAKE_SOURCE_DIR}/PackageConfig.cmake)

include(CPack)
