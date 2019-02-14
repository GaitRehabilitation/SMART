cmake_minimum_required(VERSION 3.8.2)

project(SMART)

# Find includes in the build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Turn on automatic invocation of the MOC
set(CMAKE_AUTOMOC ON)

# Make this a GUI application on Windows
if(WIN32)
  set(CMAKE_WIN32_EXECUTABLE ON)
endif()

include_directories(./)
find_package(Qt5 COMPONENTS Core Widgets Quick PrintSupport Bluetooth REQUIRED)

set (SOURCES
  qcustomplot.cpp
  forms/deviceselectdialog.cpp
  forms/mainwindow.cpp
  forms/sensorpanel.cpp
  forms/profiledialog.cpp
  forms/mbientconfigpanel.cpp

  common/main.cpp
  common/metawearwrapperbase.cpp
  common/util.cpp
  common/BluetoothAddress.cpp)

set(HEADERS
  qcustomplot.h
  forms/deviceselectdialog.h
  forms/sensorpanel.h
  forms/mainwindow.h
  forms/profiledialog.h
  forms/mbientconfigpanel.h

  common/metawearwrapperbase.h
  common/util.h
  common/BaseDiscoveryAgent.h
  common/BluetoothAddress.h)


set ( UIS
    forms/mainwindow.ui
    forms/sensorpanel.ui
    forms/deviceselectdialog.ui
    forms/mbientconfigpanel.ui
    forms/profiledialog.ui
        )

set ( RESOURCES
 qml.qrc
)


if (UNIX)

  # Add a compiler flag
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++17 -pthread")

  include_directories(platform/linux)
  set (PLATFORM_SOURCE
          platform/linux/common/metawearwrapper.cpp
          platform/linux/common/DiscoveryAgent.cpp
  )

  set(PLATFORM_HEADERS
          platform/linux/common/metawearwrapper.h
          platform/linux/common/DiscoveryAgent.h
  )

endif (UNIX)

if (MSVC)
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
	include_directories(platform/windows)

	set (PLATFORM_SOURCE
		platform/windows/common/metawearwrapper.cpp
		platform/windows/common/DiscoveryAgent.cpp
	)

	set(PLATFORM_HEADERS
		platform/windows/common/metawearwrapper.h
		platform/windows/common/DiscoveryAgent.h
	)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /ZW")
	add_compile_options("/std:c++14")
	include_directories(zlib)
endif (MSVC)

# Generate code from ui files
qt5_wrap_ui(UI_HEADERS ${UIS})

# Generate rules for building source files from the resources
qt5_add_resources(QRCS ${RESOURCES})

# Tell CMake to create the helloworld executable
add_executable(SMART ${UI_HEADERS} ${QRCS} ${SOURCES} ${HEADERS} ${PLATFORM_SOURCE} ${PLATFORM_HEADERS} )

add_subdirectory(./3rdparty)
# Add the Qt5 Widgets for linking
target_link_libraries(SMART Qt5::Gui Qt5::Widgets Qt5::Core Qt5::Quick Qt5::PrintSupport)

if (UNIX)
    target_link_libraries(SMART Qt5::Bluetooth)
endif (UNIX)

target_link_libraries(SMART extern_metawear)
target_link_libraries(SMART qmsgpack_extern ) 
target_link_libraries(SMART quazip_extern ) 



if (MSVC)
		get_target_property(_qmake_executable Qt5::qmake IMPORTED_LOCATION)
		get_filename_component(_qt_bin_dir "${_qmake_executable}" DIRECTORY)
		find_program(WINDEPLOYQT_EXECUTABLE windeployqt HINTS "${_qt_bin_dir}")

		add_custom_command(
                    TARGET SMART POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_BINARY_DIR}/windeployqt_stuff
                    COMMAND ${WINDEPLOYQT_EXECUTABLE} --compiler-runtime --dir ${CMAKE_BINARY_DIR}/windeployqt_stuff $<TARGET_FILE:SMART>
		)

		install(
            DIRECTORY ${CMAKE_BINARY_DIR}/windeployqt_stuff/
            DESTINATION .
		)

	target_link_libraries(SMART wsock32 ws2_32)
	
	set(CMAKE_INSTALL_UCRT_LIBRARIES TRUE)
	set(CMAKE_INSTALL_SYSTEM_RUNTIME_DESTINATION ./)
	include(InstallRequiredSystemLibraries)
endif(MSVC)


install(FILES
	$<TARGET_FILE:Qt5::Widgets>
	$<TARGET_FILE:Qt5::Core>
	$<TARGET_FILE:Qt5::Gui>
	$<TARGET_FILE:Qt5::Quick>
	$<TARGET_FILE:Qt5::PrintSupport>
	DESTINATION "."
)

install(TARGETS SMART 
	DESTINATION "."
)

set(CPACK_COMPONENT_APPLICATIONS_DISPLAY_NAME "SMART Application")

if(NOT DEFINED ${CPACK_PACKAGE_VERSION_MAJOR})
	SET(CPACK_PACKAGE_VERSION_MAJOR 0)
endif()

if(NOT DEFINED ${CPACK_PACKAGE_VERSION_MINOR})
	SET(CPACK_PACKAGE_VERSION_MINOR 0)
endif()

if(NOT DEFINED ${CPACK_PACKAGE_VERSION_PATCH})
	SET(CPACK_PACKAGE_VERSION_PATCH 0)
endif()

set(CPACK_GENERATOR NSIS)
set(CPACK_PACKAGE_NAME "SMART")
set(CPACK_PACKAGE_VENDOR "GrailLab")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "A Utility for collecting data from Mbient sensors")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "SMART")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/LICENSE.txt")


set(CPACK_NSIS_EXECUTABLES_DIRECTORY ".")
set(CPACK_PACKAGE_EXECUTABLES "SMART" "SMART")
set(CPACK_CREATE_DESKTOP_LINKS "SMART")
set(CPACK_NSIS_MODIFY_PATH "ON")
set(CPACK_PACKAGE_ICON "${CMAKE_SOURCE_DIR}/icon.ico")


INCLUDE(CPack)
