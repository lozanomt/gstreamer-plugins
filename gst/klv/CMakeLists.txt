set (SOURCES
  gstklv.c
  gstklvinject.c
  gstklvtimestamp.c
  gstklvinspect.c)
    
set (HEADERS
  gstklvinject.h
  gstklvtimestamp.h
  gstklvinspect.h)

include_directories (AFTER
  ${PROJECT_SOURCE_DIR}/common
  ${PROJECT_SOURCE_DIR}/gst-libs/klv
  )

set(libname gstklv)

add_library (${libname} MODULE
  ${SOURCES}
  ${HEADERS})
  
target_link_libraries (${libname}
  ${GLIB2_LIBRARIES}
  ${GOBJECT_LIBRARIES}
  ${GSTREAMER_LIBRARY}
  ${GSTREAMER_BASE_LIBRARY}
  ${GSTREAMER_VIDEO_LIBRARY}
  gstklv-1.0-0)

install(TARGETS ${libname} LIBRARY DESTINATION ${PLUGIN_INSTALL_DIR})