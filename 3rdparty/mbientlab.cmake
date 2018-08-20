file(GLOB_RECURSE Mbient_SOURCE mbientlab/src/metawear/**/*.cpp)
file(GLOB_RECURSE Mbient_HEADERS mbientlab/src/metawear/**/*.h)

add_library(extern_metawear ${Mbient_SOURCE} ${Mbient_HEADERS})
target_include_directories(extern_metawear PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/mbientlab/src)