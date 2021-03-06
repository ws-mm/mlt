﻿set( SRCS
        mlt_frame.c
        mlt_version.c
        mlt_geometry.c
        mlt_deque.c
        mlt_property.c
        mlt_properties.c
        mlt_events.c
        mlt_parser.c
        mlt_service.c
        mlt_producer.c
        mlt_multitrack.c
        mlt_playlist.c
        mlt_consumer.c
        mlt_filter.c
        mlt_transition.c
        mlt_field.c
        mlt_tractor.c
        mlt_factory.c
        mlt_repository.c
        mlt_pool.c
        mlt_tokeniser.c
        mlt_profile.c
        mlt_log.c
        mlt_cache.c
        mlt_animation.c
        mlt_slices.c
        )

set(HDRS
        mlt.h
        mlt_types.h
        mlt_defines.h
        mlt_consumer.h
        mlt_version.h
        mlt_factory.h
        mlt_filter.h
        mlt_multitrack.h
        mlt_pool.h
        mlt_properties.h
        mlt_events.h
        mlt_parser.h
        mlt_repository.h
        mlt_tractor.h
        mlt_deque.h
        mlt_field.h
        mlt_frame.h
        mlt_geometry.h
        mlt_playlist.h
        mlt_producer.h
        mlt_property.h
        mlt_service.h
        mlt_transition.h
        mlt_tokeniser.h
        mlt_profile.h
        mlt_log.h
        mlt_cache.h
        mlt_animation.h
        mlt_slices.h
        )

#add_definitions( -DPREFIX_DATA="\"${mltdatadir}\"" -DPREFIX_LIB="\"$(moduledir)\"")
add_definitions(-DMLT_EXPORTS)
if(WIN32)
    set(SRCS ${SRCS}
            ../win32/win32.c
            ../win32/win32.h
            ../win32/win_time.c
            ../win32/win_time.h
            )
    #LDFLAGS += -liconv
endif(WIN32)

add_library(mlt SHARED ${SRCS} ${HDRS} )

if(WIN32)
#    target_include_directories(mlt PRIVATE ${PTHREAD_PATH}/include)
    target_link_libraries( mlt pthread_dll dl)
else()
    target_link_libraries( mlt pthread m dl)
endif(WIN32)

set(Mlt_VERSION 1.0.0)

set_property(TARGET mlt PROPERTY VERSION ${Mlt_VERSION})
set_property(TARGET mlt PROPERTY SOVERSION 1)
set_property(TARGET mlt PROPERTY INTERFACE_mlt_MAJOR_VERSION 1)
set_property(TARGET mlt APPEND PROPERTY  COMPATIBLE_INTERFACE_STRING mlt_MAJOR_VERSION )

# install target lib, dll, so, exe
install(TARGETS mlt EXPORT MltTargets #install target: dll lib exe etc
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  RUNTIME DESTINATION bin
  INCLUDES DESTINATION include
)

# install headers
install(
    DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
    DESTINATION include
    FILES_MATCHING PATTERN "*.h")  #install dir

# install version file
include(CMakePackageConfigHelpers)
set(ConfigPackageLocation lib/cmake/mlt)
write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/MltConfigVersion.cmake"
  VERSION ${Mlt_VERSION}
  COMPATIBILITY AnyNewerVersion
)
install(   #install file
  FILES
    "${CMAKE_CURRENT_BINARY_DIR}/MltConfigVersion.cmake"
  DESTINATION
    ${ConfigPackageLocation}
  COMPONENT
    Devel
)

#install export file 
export(EXPORT MltTargets
  FILE "${CMAKE_CURRENT_BINARY_DIR}/MltTargets.cmake"
  NAMESPACE mlt::
)
install(EXPORT MltTargets
  FILE
    MltTargets.cmake
  NAMESPACE
    mlt::
  DESTINATION
    ${ConfigPackageLocation}
)

#install config file
configure_file(../../cmake/mltConfig.cmake
  "${CMAKE_CURRENT_BINARY_DIR}/MltConfig.cmake"
  COPYONLY
)
install(
  FILES
    "${CMAKE_CURRENT_BINARY_DIR}/MltConfig.cmake"
  DESTINATION
    ${ConfigPackageLocation}
  COMPONENT
    Devel
)



