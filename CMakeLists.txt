cmake_minimum_required(VERSION 3.16)

project("Autobahn Raser II Toolkit" VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED OFF)

# No external dependencies for now!

add_library(Stringtools INTERFACE)

target_include_directories(Stringtools
	INTERFACE
		${PROJECT_SOURCE_DIR}/modules
)

add_library(Exceptions INTERFACE)

target_include_directories(Exceptions
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
		Stringtools
)

add_library(WindowsInterface STATIC
	${PROJECT_SOURCE_DIR}/include/WindowsInterface.h
	${PROJECT_SOURCE_DIR}/src/WindowsInterface.cpp
)

target_include_directories(WindowsInterface
	PUBLIC
		${PROJECT_SOURCE_DIR}/include		
)

target_link_libraries(WindowsInterface
	PUBLIC
		Stringtools
		Exceptions
)

# Core Lib
add_library(winmm SHARED
	${PROJECT_SOURCE_DIR}/include/Exports.h
	${PROJECT_SOURCE_DIR}/src/winmm.cpp
	${PROJECT_SOURCE_DIR}/src/Exports.cpp
	${PROJECT_SOURCE_DIR}/res/Exports.def
	${PROJECT_SOURCE_DIR}/include/spel.h
	${PROJECT_SOURCE_DIR}/src/spel.cpp
	${PROJECT_SOURCE_DIR}/include/Monitor.h
	${PROJECT_SOURCE_DIR}/src/Monitor.cpp
)

target_include_directories(winmm
	PUBLIC
		${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(winmm 
	PRIVATE
		ModuleInifile
)

# Autobahn Raser II.exe rewrite
add_executable("ARII_starter" WIN32
	"${PROJECT_SOURCE_DIR}/src/ARII_starter.cpp"
)

target_link_libraries("ARII_starter"
	PRIVATE
		ModuleInifile
		WindowsInterface
)

# Test utilities
add_executable(InifileTest
	${PROJECT_SOURCE_DIR}/src/InifileTest.cpp
)

target_link_libraries(InifileTest
	PRIVATE
		ModuleInifile
)