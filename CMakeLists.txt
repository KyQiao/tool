cmake_minimum_required(VERSION 3.10)
project(Tool VERSION 0.0.1 LANGUAGES CXX)

# set cmake sanitizers
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/externals/sanitizers-cmake/cmake" ${CMAKE_MODULE_PATH})

find_package(Sanitizers)


option(BUILD_SHARED_LIBS "Build shared libs" OFF)
option(TEST "test example" OFF)
option(FUNCTIONS "build functions" ON)
include(GNUInstallDirs)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CXX_FLAGS)
  #release comes with -O3 by default
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel." FORCE)
endif(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CXX_FLAGS)

set(CMAKE_CXX_STANDARD 11)


# # add sanitize option
# set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address")
# set (CMAKE_LINKER_FLAGS_DEBUG "${CMAKE_LINKER_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address")


find_package(OpenMP REQUIRED)
find_package(ZLIB REQUIRED)
find_package(VORO REQUIRED)

#grab all file we need to compile
file(GLOB_RECURSE SOURCE src/*.cpp)
file(GLOB_RECURSE HEADERS include/*.hpp)
install(FILES ${HEADERS} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}) 

add_library(tool ${SOURCE})
include_directories(include)

target_link_libraries(tool PRIVATE OpenMP::OpenMP_CXX ZLIB::ZLIB voro++)
install(TARGETS tool EXPORT TOOL_Targets LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})


install(EXPORT TOOL_Targets FILE TOOLConfig.cmake NAMESPACE TOOL:: DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/TOOL)
include(CMakePackageConfigHelpers)
write_basic_package_version_file("TOOLConfigVersion.cmake" VERSION ${PROJECT_VERSION} COMPATIBILITY ExactVersion)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/TOOLConfigVersion.cmake" DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/TOOL)

IF(TEST)
  enable_testing()
  file(GLOB TestFile test/*.cpp)
  # remove some test that not modified
  file(GLOB NOT_test  test/testQW.cpp 
                      test/3jtest.cpp 
                      test/testBase.cpp 
                      test/testIO.cpp 
                      test/testTable.cpp 
                      test/testSh.cpp
        )
  list(REMOVE_ITEM TestFile ${NOT_test})
  foreach(testSource ${TestFile})  
    string(REGEX REPLACE "^.*/([^/]*)\\.cpp$" "\\1" PROGNAME "${testSource}")
    # message(${PROGNAME})
    add_executable(${PROGNAME} ${testSource})
    include_directories(include/)
    target_link_libraries(${PROGNAME} PRIVATE tool OpenMP::OpenMP_CXX ZLIB::ZLIB voro++)
    add_test(NAME ${PROGNAME} COMMAND ${PROGNAME})
    # add_test(NAME ${PROGNAME} COMMAND qw46 ${CMAKE_CURRENT_SOURCE_DIR}/test/fcc.xyz)
endforeach()

  # file(GLOB EXAMPLES LIST_DIRECTORIES true test/*)
endif(TEST)

# build functions
if(FUNCTIONS)
  file(GLOB FunctionsFile functions/*.cpp)
  # remove some test that not modified
  # file(GLOB NOT_test  test/testQW.cpp 
  #                     test/3jtest.cpp 
  #                     test/testBase.cpp 
  #                     test/testIO.cpp 
  #                     test/testTable.cpp 
  #                     test/testSh.cpp
  #       )
  # list(REMOVE_ITEM FunctionsFile ${NOT_test})
  foreach(functionSource ${FunctionsFile})  
    string(REGEX REPLACE "^.*/([^/]*)\\.cpp$" "\\1" PROGNAME "${functionSource}")
    add_executable(${PROGNAME} ${functionSource})
    add_sanitizers(${PROGNAME})
    include_directories(include/)
    target_link_libraries(${PROGNAME} PRIVATE tool OpenMP::OpenMP_CXX ZLIB::ZLIB voro++)
    install(TARGETS ${PROGNAME} RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
  endforeach()
endif(FUNCTIONS)
