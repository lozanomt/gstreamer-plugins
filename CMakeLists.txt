cmake_minimum_required(VERSION "3.0.0")

project(gst-plugins)

option(ENABLE_KLV "Whether to enable KLV support" ON)

set(CMAKE_SHARED_MODULE_PREFIX "lib")
set(CMAKE_SHARED_LIBRARY_PREFIX "lib")
set(PLUGIN_INSTALL_DIR /usr/local/lib/)

# add local cmake modules
list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/modules)

# get git version info
include(GetGitRevisionDescription)
git_describe(GIT_DESCRIBE --dirty)

# values used when registering plugins
add_definitions(-DGST_PACKAGE_VERSION="${GIT_DESCRIBE}")
add_definitions(-DGST_PACKAGE_LICENSE="LGPL")
add_definitions(-DGST_PACKAGE_ORIGIN="Unknown package origin")
add_definitions(-DGST_PACKAGE_NAME="${CMAKE_PROJECT_NAME}")
add_definitions(-DPACKAGE="${CMAKE_PROJECT_NAME} package")

# configure CPack
set(CPACK_GENERATOR "ZIP")
set(CPACK_ARCHIVE_COMPONENT_INSTALL ON)
set(CPACK_COMPONENTS_GROUPING IGNORE)
set(CPACK_COMPONENTS_ALL pdb Unspecified)
set(CPACK_PACKAGE_VERSION ${GIT_DESCRIBE})
if (CMAKE_SIZEOF_VOID_P EQUAL 8)
  set(CPACK_PACKAGE_FILE_NAME "${CMAKE_PROJECT_NAME}-${CPACK_PACKAGE_VERSION}-x86_64")
else ()
  set(CPACK_PACKAGE_FILE_NAME "${CMAKE_PROJECT_NAME}-${CPACK_PACKAGE_VERSION}-x86")
endif ()
set(CPACK_ARCHIVE_PDB_FILE_NAME "${CPACK_PACKAGE_FILE_NAME}-pdbs")
set(CPACK_ARCHIVE_UNSPECIFIED_FILE_NAME "${CPACK_PACKAGE_FILE_NAME}")
include(CPack)

include(MacroLogFeature)

find_package(GStreamer REQUIRED COMPONENTS base)
macro_log_feature(GSTREAMER_FOUND "GStreamer" "Required to build gst-plugins" "http://gstreamer.freedesktop.org/" TRUE "1.2.0")
macro_log_feature(GSTREAMER_BASE_LIBRARY_FOUND "GStreamer base library" "Required to build most plugins" "http://gstreamer.freedesktop.org/" FALSE "1.2.0")

find_package(GStreamerPluginsBase COMPONENTS video)
macro_log_feature(GSTREAMER_VIDEO_LIBRARY_FOUND "GStreamer video library" "Required to build several video plugins" "http://gstreamer.freedesktop.org/" FALSE "1.2.0")

find_package(GLIB2 REQUIRED)
macro_log_feature(GLIB2_FOUND "GLib" "Required to build gst-plugins" "http://www.gtk.org/" TRUE)

find_package(GObject REQUIRED)
macro_log_feature(GOBJECT_FOUND "GObject" "Required to build gst-plugins" "http://www.gtk.org/" TRUE)

# Setup common environment
include_directories(
    .
    ${GSTREAMER_INCLUDE_DIR}
    ${GLIB2_INCLUDE_DIR})

if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  if (WIN32)
    get_filename_component(_PREFIX "${GSTREAMER_PLUGIN_DIR}/../" REALPATH)
  else ()
    set(_PREFIX "/usr/lib")
  endif ()
  set(CMAKE_INSTALL_PREFIX "${_PREFIX}"
      CACHE PATH "Common prefix for all installed files (e.g., plugins will be installed under CMAKE_INSTALL_PREFIX/gstreamer-1.0)" FORCE)
endif(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)

if (NOT LIBRARY_INSTALL_DIR)
  set(LIBRARY_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/"
      CACHE PATH "Location to install libraries (e.g., libgstklv-1.0-0.so/dll)")
endif ()

add_subdirectory(gst-libs)

# Build the plugins
add_subdirectory(gst)

macro_display_feature_log()
