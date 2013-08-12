SET(VTK_LIBS vtkCommonExecutionModel vtkParallelCore)

vtk_module(vtkMedReader
  DEPENDS
    ${VTK_LIBS}
    vtkRenderingFreeTypeOpenGL
    vtkRenderingOpenGL
    vtkRenderingFreeType
    vtkRenderingCore
    vtkInteractionStyle
    TEST_DEPENDS
    vtkRenderingCore
    vtkTestingRendering
    vtkInteractionStyle
    vtkRenderingFreeTypeOpenGL
  EXCLUDE_FROM_WRAP_HIERARCHY
)
