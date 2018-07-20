find_program(
  INKSCAPE_EXECUTABLE
  inkscape
  PATHS /usr/bin /usr/local/bin /opt/local/bin
)

if(INKSCAPE_EXECUTABLE)
  set(INKSCAPE_FOUND TRUE)
  message(STATUS "Found Inkscape: ${INKSCAPE_EXECUTABLE}")
  mark_as_advanced(INKSCAPE_FOUND)
endif(INKSCAPE_EXECUTABLE)

# inkscape export