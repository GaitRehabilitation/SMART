cmake_minimum_required(VERSION 3.8.2)

project(SMART)

# Find includes in the build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Turn on automatic invocation of the MOC
set(CMAKE_AUTOMOC ON)

# There may be a way to tell up front if Qt5 is going to be found, but I haven't found
# a foolproof way to do it yet, so settle for the default error message for now.
#if(NOT CMAKE_PREFIX_PATH AND NOT Qt5Widgets_DIR)
#    message(WARNING "CMAKE_PREFIX_PATH is not defined, so find_package may not work. Set the CMAKE_PREFIX_PATH "
#            "environment variable to the install prefix of Qt 5, either on the command line as "
#            "-DCMAKE_PREFIX_PATH=\"path/to/Qt5/lib/cmake\" or with set(CMAKE_PREFIX_PATH path/to/Qt5/lib/cmake)")
#endif(NOT CMAKE_PREFIX_PATH AND NOT Qt5Widgets_DIR)

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
  message(${Qt5Widgets_INCLUDE_DIRS}/QtZlib)
endif (MSVC)


add_subdirectory(./3rdparty)
include_directories(./3rdparty/quazip/quazip)

# Generate code from ui files
qt5_wrap_ui(UI_HEADERS ${UIS})

# Generate rules for building source files from the resources
qt5_add_resources(QRCS ${RESOURCES})

# Tell CMake to create the helloworld executable
add_executable(SMART ${UI_HEADERS} ${QRCS} ${SOURCES} ${HEADERS} ${PLATFORM_SOURCE} ${PLATFORM_HEADERS} )


if (MSVC)
 add_custom_command(
    TARGET SMART POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:Qt5::Widgets>
		$<TARGET_FILE:Qt5::Core>
		$<TARGET_FILE:Qt5::Gui>
		$<TARGET_FILE:Qt5::Quick>
		$<TARGET_FILE:Qt5::PrintSupport>
        $<TARGET_FILE_DIR:SMART>
)

 target_link_libraries(SMART wsock32 ws2_32)
endif(MSVC)

# Add the Qt5 Widgets for linking
target_link_libraries(SMART Qt5::Gui Qt5::Widgets Qt5::Core Qt5::Quick Qt5::PrintSupport)

if (UNIX)
    target_link_libraries(SMART Qt5::Bluetooth)
endif (UNIX)

include_directories(./3rdparty/qmsgpack/src)

target_link_libraries(SMART ${QUAZIP_LIB_TARGET_NAME})
target_link_libraries(SMART extern_metawear)
target_link_libraries(SMART qmsgpack)