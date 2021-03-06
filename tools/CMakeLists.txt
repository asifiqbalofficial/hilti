
set(hilti_build "${CMAKE_CURRENT_SOURCE_DIR}/../tools/hilti-build")

find_package(PAPI)
find_package(PAPI)

if (${PAPI_FOUND} STREQUAL "TRUE" )
    set(PAPI "papi")
endif ()

set(PERFTOOLS "")

if ( NOT "${CMAKE_BUILD_TYPE}" STREQUAL "Debug" )
    find_package(GooglePerftools)

    if ("${GOOGLEPERFTOOLS_FOUND}" STREQUAL "TRUE" )
        set(PERFTOOLS "tcmalloc")
    endif ()
endif ()


set(HILTI_LIBS        hilti hilti-jit ${PROJECT_BINARY_DIR}/libhilti/libhilti-rt-dbg.bc ${PROJECT_BINARY_DIR}/libhilti/libhilti-rt-native.a ${LLVM_LIBS} ${LLVM_LDFLAGS} pcap ${PAPI} ${PERFTOOLS})
set(HILTI_LIBS_NO_JIT hilti ${LLVM_LIBS} ${LLVM_LDFLAGS} pcap ${PAPI} ${PERFTOOLS})

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../hilti  ${CMAKE_CURRENT_BINARY_DIR}/../hilti)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../binpac ${CMAKE_CURRENT_BINARY_DIR}/../binpac)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../util)

add_executable(hilti-config hilti-config.cc $<TARGET_OBJECTS:util>)
target_link_libraries(hilti-config ${LLVM_LIBS} ${LLVM_LDFLAGS}) # TODO: Should not be needed.

add_executable(binpac++ binpac++.cc)
target_link_libraries(binpac++ binpacxx ${HILTI_LIBS})
add_dependencies(binpac++ hilti-rt hilti-rt-dbg)

add_executable(hiltic hiltic.cc)
target_link_libraries(hiltic ${HILTI_LIBS})
add_dependencies(hiltic hilti-rt hilti-rt-dbg)

# We build a 2nd version of hiltic that excludes the JIT support. We
# use this version for generating libhilti autogen components. The
# JIT would introduce a cyclic dependcy here.
add_executable(hiltic-nojit hiltic.cc)
target_link_libraries(hiltic-nojit ${HILTI_LIBS_NO_JIT})
set_target_properties(hiltic-nojit PROPERTIES COMPILE_DEFINITIONS "HILTIC_NO_JIT=1")

add_executable(hilti-doc hilti-doc.cc)
target_link_libraries(hilti-doc binpacxx ${HILTI_LIBS})
add_dependencies(hilti-doc hilti-rt hilti-rt-dbg)

add_subdirectory(pac-driver)
add_subdirectory(pac-dump)

add_custom_command(
    OUTPUT  ${CMAKE_CURRENT_BINARY_DIR}/hilti-prof
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/hilti-prof.c hilti-rt-dbg hilti-rt
    COMMAND ${hilti_build}
    ARGS    -o ${CMAKE_CURRENT_BINARY_DIR}/hilti-prof ${CMAKE_CURRENT_SOURCE_DIR}/hilti-prof.c
    )

add_custom_target(hilti-prof ALL DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/hilti-prof hiltic)

