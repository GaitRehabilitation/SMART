include(ExternalProject)


file(GLOB_RECURSE Mbient_SOURCE mbientlab/src/metawear/**/*.cpp)
file(GLOB_RECURSE Mbient_HEADERS mbientlab/src/metawear/**/*.h)

add_library(extern_metawear ${Mbient_SOURCE} ${Mbient_HEADERS})
target_include_directories(extern_metawear PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/mbientlab/src)


ExternalProject_Add(qmsgpack 
		GIT_REPOSITORY https://github.com/romixlab/qmsgpack.git
		PREFIX ${CMAKE_CURRENT_BINARY_DIR}/qmsgpack
		CMAKE_ARGS -DCMAKE_PREFIX_PATH=${CMAKE_PREFIX_PATH} -DCMAKE_INSTALL_PREFIX:PATH=<INSTALL_DIR>)
ExternalProject_Get_property(qmsgpack install_dir)

add_library(qmsgpack_extern STATIC IMPORTED GLOBAL) 
set_property(TARGET qmsgpack_extern PROPERTY IMPORTED_LOCATION  ${install_dir}/lib/qmsgpackd.lib) 
target_include_directories(qmsgpack_extern INTERFACE ${install_dir}/include/qmsgpack/)
link_directories(qmsgpack_extern INTERFACE ${install_dir}/bin)


install(FILES
	 ${install_dir}/bin/qmsgpackd.dll
	DESTINATION "."
)



SET(QUAZIP_LIB_VERSION_SUFFIX 5)
ExternalProject_Add(quazip 
		GIT_REPOSITORY https://github.com/stachenov/quazip.git 
		PREFIX ${CMAKE_CURRENT_BINARY_DIR}/quazip
		CMAKE_ARGS -DZLIB_INCLUDE_DIRS=${CMAKE_SOURCE_DIR}/zlib -DCMAKE_PREFIX_PATH=${CMAKE_PREFIX_PATH} -DCMAKE_INSTALL_PREFIX:PATH=<INSTALL_DIR>)

ExternalProject_Get_property(quazip install_dir)

add_library( quazip_extern  STATIC  IMPORTED GLOBAL) 
set_property(TARGET quazip_extern PROPERTY IMPORTED_LOCATION ${install_dir}/lib/quazip${QUAZIP_LIB_VERSION_SUFFIX}d.lib ) 
target_include_directories(quazip_extern INTERFACE ${install_dir}/include/quazip${QUAZIP_LIB_VERSION_SUFFIX}/)
link_directories(quazip_extern INTERFACE ${install_dir}/bin)

install(FILES
	 ${install_dir}/lib/quazip${QUAZIP_LIB_VERSION_SUFFIX}d.dll
	DESTINATION "."
)

add_dependencies(SMART quazip)
add_dependencies(SMART qmsgpack)
