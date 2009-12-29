
if not ('servermanager' in dir()):
  from pvsimple import *

import os

def LoadMultipleFiles(FilePath, FilePrefix):

  dualSphereAnimation = XMLPolyDataReader( FileName=[FilePath + FilePrefix + '0' + '.vtp',
                                                                                                     FilePath + FilePrefix + '1' + '.vtp',
                                                                                                     FilePath + FilePrefix + '2' + '.vtp',
                                                                                                     FilePath + FilePrefix + '3' + '.vtp',
                                                                                                     FilePath + FilePrefix + '4' + '.vtp',
                                                                                                     FilePath + FilePrefix + '5' + '.vtp',
                                                                                                     FilePath + FilePrefix + '6' + '.vtp',
                                                                                                     FilePath + FilePrefix + '7' + '.vtp',
                                                                                                     FilePath + FilePrefix + '8' + '.vtp',
                                                                                                     FilePath + FilePrefix + '9' + '.vtp',
                                                                                                     FilePath + FilePrefix + '10' + '.vtp'])

  dualSphereAnimation.PointArrayStatus = ['Normals']
  dualSphereAnimation.CellArrayStatus = ['cellNormals']

  DataRepr = Show()
  DataRepr.EdgeColor = [0.0, 0.0, 0.50000762951094835]
  DataRepr.SelectionCellLabelColor = [0.0, 1.0, 0.0]
  DataRepr.SelectionPointLabelJustification = 'Center'
  DataRepr.SelectionCellLabelJustification = 'Center'
  DataRepr.SelectionLineWidth = 2.0
  DataRepr.SelectionCellLabelFontSize = 24
  DataRepr.SelectionColor = [1.0, 0.0, 1.0]
  DataRepr.SelectionRepresentation = 'Wireframe'

  RenderV = GetRenderView()
  RenderV.CameraViewUp = [0.3643594701432189, -0.53089863872031573, -0.76510705912432175]
  RenderV.CameraPosition = [-1.43673886826885, 0.58420580818614165, -1.2056476292046452]
  RenderV.CameraClippingRange = [1.1902279246461551, 3.3614292621569204]
  RenderV.CameraFocalPoint = [0.24373197555541973, 4.2615454220042359e-08, -1.2218938645261819e-16]
  RenderV.CameraParallelScale = 0.5562421018838376
  RenderV.CenterOfRotation = [0.24373197555541992, 4.2615454276528908e-08, 0.0]
  RenderV.CameraViewUp = [-0.022295256951954404, 0.99334037123472696, -0.11303905693476472]
  RenderV.CameraFocalPoint = [0.24373197555541956, 4.261545421741021e-08, 6.2953480785622502e-17]
  RenderV.CameraClippingRange = [0.8880289667943051, 3.8672572119597652]
  RenderV.CameraPosition = [0.73282076784458949, 0.24745673087950623, 2.078081369909921]

  Render()


testdir = os.getenv("TESTDIR")
pvdata = os.getenv("PVDATA")

if __name__ == "__main__":
  LoadMultipleFiles(FilePath=pvdata+"/Data/dualSphereAnimation/", FilePrefix="dualSphereAnimation_P00T000")
