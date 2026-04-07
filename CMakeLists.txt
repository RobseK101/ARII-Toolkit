cmake_minimum_required(VERSION 3.16)

project("Autobahn Raser II Toolkit" VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED OFF)

# No external dependencies for now!

add_library(Stringtools INTERFACE)

target_include_directories(Stringtools
	INTERFACE
		${PROJECT_SOURCE_DIR}/modules
)

add_library(ModuleLogging STATIC
	${PROJECT_SOURCE_DIR}/modules/ren/Logging.h
	${PROJECT_SOURCE_DIR}/modules/Logging.cpp
)

target_include_directories(ModuleLogging
	PUBLIC
		${PROJECT_SOURCE_DIR}/modules
)

add_library(ModuleInifile STATIC
	${PROJECT_SOURCE_DIR}/modules/Inifile.h
	${PROJECT_SOURCE_DIR}/modules/AR2Inifile.h
	${PROJECT_SOURCE_DIR}/modules/Inifile.cpp
	${PROJECT_SOURCE_DIR}/modules/AR2Inifile.cpp
)

target_include_directories(ModuleInifile 
	PUBLIC 
		${PROJECT_SOURCE_DIR}/modules
)

target_link_libraries(ModuleInifile
	PUBLIC
		ModuleLogging
)

# Core Lib
add_library(winmm SHARED
	${PROJECT_SOURCE_DIR}/include/Exports.h
	${PROJECT_SOURCE_DIR}/src/winmm.cpp
	${PROJECT_SOURCE_DIR}/src/Exports.cpp
	${PROJECT_SOURCE_DIR}/res/Exports.def
)

target_include_directories(winmm
	PUBLIC
		${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(winmm 
	PRIVATE
		ModuleInifile
)

# Test utilities
add_executable(InifileTest
	${PROJECT_SOURCE_DIR}/src/InifileTest.cpp
)

target_link_libraries(InifileTest
	PRIVATE
		ModuleInifile
)