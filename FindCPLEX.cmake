# Try to find the CPLEX and CONCERT librairies.
include(FindPackageHandleStandardArgs)
include(CheckLibraryExists)

# Find the path to CPLEX Studio
if (UNIX)
    # Set system architecture
    if (CMAKE_SIZEOF_VOID_P EQUAL 8)
        set(CPLEX_ARCH x86-64)
    else ()
        set(CPLEX_ARCH x86)
    endif ()

    if (APPLE)
        set(CPLEX_STUDIO_DIR /Applications/CPLEX_Studio128)
        set(CPLEX_LIB_PATH_SUFFIX lib/${CPLEX_ARCH}_osx/static_pic)
    else ()
        # set CPLEX_STUDIO_DIR
        # set CPLEX_LIB_PATH_SUFFIX
    endif ()
endif ()

find_package(Threads)

# ----------------------------------------------------------------------------
# CPLEX
# ----------------------------------------------------------------------------

set(CPLEX_DIR ${CPLEX_STUDIO_DIR}/cplex)
# Find the CPLEX include directory.
find_path(CPLEX_INCLUDE_DIR ilcplex/cplex.h PATHS ${CPLEX_DIR}/include)
if (UNIX)
    find_library(CPLEX_LIBRARY NAMES cplex
            PATHS ${CPLEX_DIR} PATH_SUFFIXES ${CPLEX_LIB_PATH_SUFFIX})
    set(CPLEX_LIBRARY_DEBUG ${CPLEX_LIBRARY})
else ()
    error("Windows not supported by FindCPLEX.cmake")
endif()

# Handle the QUIETLY and REQUIRED arguments and set CPLEX_FOUND to TRUE
# if all listed variables are TRUE.
find_package_handle_standard_args(
        CPLEX DEFAULT_MSG CPLEX_LIBRARY CPLEX_LIBRARY_DEBUG CPLEX_INCLUDE_DIR)

if (CPLEX_FOUND AND NOT TARGET cplex-library)
    set(CPLEX_LINK_LIBRARIES ${CMAKE_THREAD_LIBS_INIT})
    check_library_exists(m floor "" HAVE_LIBM)
    if (HAVE_LIBM)
        set(CPLEX_LINK_LIBRARIES ${CPLEX_LINK_LIBRARIES} m)
    endif ()
    add_library(cplex-library STATIC IMPORTED GLOBAL)
    set_target_properties(cplex-library PROPERTIES
            IMPORTED_LOCATION "${CPLEX_LIBRARY}"
            IMPORTED_LOCATION_DEBUG "${CPLEX_LIBRARY_DEBUG}"
            INTERFACE_INCLUDE_DIRECTORIES "${CPLEX_INCLUDE_DIR}"
            INTERFACE_LINK_LIBRARIES "${CPLEX_LINK_LIBRARIES}")
endif ()

# ----------------------------------------------------------------------------
# CONCERT
# ----------------------------------------------------------------------------

set(CPLEX_CONCERT_DIR ${CPLEX_STUDIO_DIR}/concert)

# Find the Concert include directory.
find_path(CPLEX_CONCERT_INCLUDE_DIR ilconcert/ilosys.h
        PATHS ${CPLEX_CONCERT_DIR}/include)

# Find the Concert library.

if (UNIX)
    find_library(CPLEX_CONCERT_LIBRARY NAMES concert
            PATHS ${CPLEX_CONCERT_DIR} PATH_SUFFIXES ${CPLEX_LIB_PATH_SUFFIX})
    set(CPLEX_CONCERT_LIBRARY_DEBUG ${CPLEX_CONCERT_LIBRARY})
else ()
    error("Windows not supported by FindCPLEX.cmake")
endif ()

# Handle the QUIETLY and REQUIRED arguments and set CPLEX_CONCERT_FOUND to
# TRUE if all listed variables are TRUE.
find_package_handle_standard_args(
        CPLEX_CONCERT DEFAULT_MSG CPLEX_CONCERT_LIBRARY CPLEX_CONCERT_LIBRARY_DEBUG
        CPLEX_CONCERT_INCLUDE_DIR)

if (CPLEX_CONCERT_FOUND AND NOT TARGET cplex-concert)
    add_library(cplex-concert STATIC IMPORTED GLOBAL)
    set_target_properties(cplex-concert PROPERTIES
            IMPORTED_LOCATION "${CPLEX_CONCERT_LIBRARY}"
            IMPORTED_LOCATION_DEBUG "${CPLEX_CONCERT_LIBRARY_DEBUG}"
            INTERFACE_COMPILE_DEFINITIONS IL_STD # Require standard compliance.
            INTERFACE_INCLUDE_DIRECTORIES "${CPLEX_CONCERT_INCLUDE_DIR}"
            INTERFACE_LINK_LIBRARIES "${CMAKE_THREAD_LIBS_INIT}")
endif ()

# ----------------------------------------------------------------------------
# IloCplex - depends on CPLEX and Concert
# ----------------------------------------------------------------------------

include(CheckCXXCompilerFlag)
check_cxx_compiler_flag(-Wno-long-long HAVE_WNO_LONG_LONG_FLAG)
if (HAVE_WNO_LONG_LONG_FLAG)
    # Required if -pedantic is used.
    set(CPLEX_ILOCPLEX_DEFINITIONS -Wno-long-long)
endif ()

find_path(CPLEX_ILOCPLEX_INCLUDE_DIR ilcplex/ilocplex.h
        PATHS ${CPLEX_INCLUDE_DIR})

# Find the IloCplex library.
if (UNIX)
    find_library(CPLEX_ILOCPLEX_LIBRARY NAMES ilocplex
            PATHS ${CPLEX_DIR} PATH_SUFFIXES ${CPLEX_LIB_PATH_SUFFIX})
    set(CPLEX_ILOCPLEX_LIBRARY_DEBUG ${CPLEX_ILOCPLEX_LIBRARY})
else ()
    error("Windows not supported by FindCPLEX.cmake")
endif ()

# Handle the QUIETLY and REQUIRED arguments and set CPLEX_ILOCPLEX_FOUND to
# TRUE if all listed variables are TRUE.
find_package_handle_standard_args(
        CPLEX_ILOCPLEX DEFAULT_MSG
        CPLEX_ILOCPLEX_LIBRARY CPLEX_ILOCPLEX_LIBRARY_DEBUG
        CPLEX_ILOCPLEX_INCLUDE_DIR CPLEX_FOUND CPLEX_CONCERT_FOUND)

if (CPLEX_ILOCPLEX_FOUND AND NOT TARGET ilocplex)
    add_library(ilocplex STATIC IMPORTED GLOBAL)
    set_target_properties(ilocplex PROPERTIES
            IMPORTED_LOCATION "${CPLEX_ILOCPLEX_LIBRARY}"
            IMPORTED_LOCATION_DEBUG "${CPLEX_ILOCPLEX_LIBRARY_DEBUG}"
            INTERFACE_INCLUDE_DIRECTORIES "${CPLEX_ILOCPLEX_INCLUDE_DIR}"
            INTERFACE_LINK_LIBRARIES "cplex-concert;cplex-library")
endif ()