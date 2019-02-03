file(GLOB_RECURSE Mbient_SOURCE mbientlab/src/metawear/**/*.cpp)
file(GLOB_RECURSE Mbient_HEADERS mbientlab/src/metawear/**/*.h)

add_library(extern_metawear ${Mbient_SOURCE} ${Mbient_HEADERS})
target_include_directories(extern_metawear PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/mbientlab/src)

include(${CMAKE_ROOT}/Modules/ExternalProject.cmake)


ExternalProject_Add(qmsgpack 
		GIT_REPOSITORY https://github.com/romixlab/qmsgpack.git
		PREFIX ${CMAKE_CURRENT_BINARY_DIR}/qmsgpack
		CMAKE_ARGS -DCMAKE_PREFIX_PATH=${CMAKE_PREFIX_PATH} -DCMAKE_INSTALL_PREFIX:PATH=<INSTALL_DIR>)
ExternalProject_Get_property(qmsgpack install_dir)
message("QPATH: ${install_dir}")
target_include_directories(SMART PUBLIC ${install_dir}/include/qmsgpack/)
SET(QMSGPACK_LIB ${install_dir}/lib/qmsgpackd.lib PARENT_SCOPE)
install(FILES
	 ${install_dir}/bin/qmsgpackd.dll
	DESTINATION "."
)

#	include_directories(zlib)
message(${CMAKE_SOURCE_DIR}/zlib)
SET(QUAZIP_LIB_VERSION_SUFFIX 5)
ExternalProject_Add(quazip 
		GIT_REPOSITORY https://github.com/stachenov/quazip.git 
		PREFIX ${CMAKE_CURRENT_BINARY_DIR}/quazip
		CMAKE_ARGS -DZLIB_INCLUDE_DIRS=${CMAKE_SOURCE_DIR}/zlib -DCMAKE_PREFIX_PATH=${CMAKE_PREFIX_PATH} -DCMAKE_INSTALL_PREFIX:PATH=<INSTALL_DIR>)

ExternalProject_Get_property(quazip install_dir)
target_include_directories(SMART PUBLIC ${install_dir}/include/quazip${QUAZIP_LIB_VERSION_SUFFIX}/)
SET(QUAZIP_LIB ${install_dir}/lib/quazip${QUAZIP_LIB_VERSION_SUFFIX}d.lib PARENT_SCOPE)
install(FILES
	 ${install_dir}/lib/quazip${QUAZIP_LIB_VERSION_SUFFIX}d.dll
	DESTINATION "."
)

add_dependencies(SMART quazip)
add_dependencies(SMART qmsgpack)