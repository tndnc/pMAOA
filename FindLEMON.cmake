include(FindPackageHandleStandardArgs)

set(LEMON_DIR lib/lemon)
set(LEMON_LIB_PATH_SUFFIX lib)
find_path(LEMON_INCLUDE_DIR lemon/core.h PATHS ${LEMON_DIR}/include)

find_library(LEMON_LIBRARY NAMES emon PATHS ${LEMON_DIR} PATH_SUFFIXES ${LEMON_LIB_PATH_SUFFIX})
set(LEMON_LIBRARY_DEBUG ${LEMON_LIBRARY})

find_package_handle_standard_args(
        LEMON DEFAULT_MSG LEMON_LIBRARY LEMON_LIBRARY_DEBUG LEMON_INCLUDE_DIR)

if (LEMON_FOUND AND NOT TARGET lemon)
    add_library(lemon-library STATIC IMPORTED GLOBAL)
    set_target_properties(lemon-library PROPERTIES
            IMPORTED_LOCATION "${LEMON_LIBRARY}"
            IMPORTED_LOCATION_DEBUG "${LEMON_LIBRARY_DEBUG}"
            INTERFACE_INCLUDE_DIRECTORIES "${LEMON_INCLUDE_DIR}")
endif()