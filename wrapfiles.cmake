ADD_CUSTOM_COMMAND(
 OUTPUT wrapfiles.txt
 COMMAND pvpython ${CMAKE_SOURCE_DIR}/getwrapclasses.py
 DEPENDS ${CMAKE_SOURCE_DIR}/getwrapclasses.py ${PARAVIEW_PYTHON_EXECUTABLE}
)
