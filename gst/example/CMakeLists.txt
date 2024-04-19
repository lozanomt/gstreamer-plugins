set (SOURCES
  gstexample.c)
    
set (HEADERS
  gstexample.h)

include_directories (AFTER
  ${GSTREAMER_INCLUDE_DIR}/..
  ${PROJECT_SOURCE_DIR}/common
  )

set(libname gstexample)

add_library (${libname} MODULE
  ${SOURCES}
  ${HEADERS})

target_link_libraries (${libname}
  ${GLIB2_LIBRARIES}
  ${GOBJECT_LIBRARIES}
  ${GSTREAMER_LIBRARY}
  ${GSTREAMER_BASE_LIBRARY}
  ${GSTREAMER_VIDEO_LIBRARY}
  ${ZLIB_LIBRARIES}
  )

install(TARGETS ${libname} LIBRARY DESTINATION ${PLUGIN_INSTALL_DIR})