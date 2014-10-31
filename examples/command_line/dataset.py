def create_geometry(out_filename=None):
  import GEOM
  import salome
  from salome.geom import geomBuilder
  geompy = geomBuilder.New(salome.myStudy)

  # create a cloud of points
  points = [
    geompy.MakeVertex(0,0,0),
    geompy.MakeVertex(9,0,0),
    geompy.MakeVertex(0,9,0),
    geompy.MakeVertex(9,9,0),
    geompy.MakeVertex(3,3,1),
    geompy.MakeVertex(6,6,2)]

  # create SmoothingSurface object
  smoothingsurface = geompy.MakeSmoothingSurface(points)
  if out_filename:
    geompy.ExportBREP(smoothingsurface, out_filename)

  # add object in the study
  id_smoothingsurface = geompy.addToStudy(smoothingsurface,"SmoothingSurface")

  return smoothingsurface
#

def create_meshes(geometry, out_filename=None):
  import SMESH
  import salome
  from salome.smesh import smeshBuilder

  smesh = smeshBuilder.New(salome.myStudy)
  Mesh_tri = smesh.Mesh(geometry)

  Regular_1D = Mesh_tri.Segment()
  Max_Size_1 = Regular_1D.MaxSize(1.28993)

  MEFISTO_2D = Mesh_tri.Triangle(algo=smeshBuilder.MEFISTO)
  Nb_Segments_1 = smesh.CreateHypothesis('NumberOfSegments')
  Nb_Segments_1.SetNumberOfSegments( 15 )
  Nb_Segments_1.SetDistrType( 0 )

  Mesh_quad = smesh.Mesh(geometry)
  status = Mesh_quad.AddHypothesis(Nb_Segments_1)
  status = Mesh_quad.AddHypothesis(Regular_1D)

  Quadrangle_2D = Mesh_quad.Quadrangle(algo=smeshBuilder.QUADRANGLE)
  isDone = Mesh_tri.Compute()
  isDone = Mesh_quad.Compute()
  smesh.SetName(Mesh_tri, 'Mesh_tri')
  if out_filename:
    Mesh_tri.ExportMED(out_filename, 0, SMESH.MED_V2_2, 1, None ,1)
  smesh.SetName(Mesh_quad, 'Mesh_quad')
  if out_filename:
    Mesh_quad.ExportMED(out_filename, 0, SMESH.MED_V2_2, 0, None ,1)

  # Set names of Mesh objects
  smesh.SetName(Regular_1D.GetAlgorithm(), 'Regular_1D')
  smesh.SetName(Quadrangle_2D.GetAlgorithm(), 'Quadrangle_2D')
  smesh.SetName(MEFISTO_2D.GetAlgorithm(), 'MEFISTO_2D')
  smesh.SetName(Nb_Segments_1, 'Nb. Segments_1')
  smesh.SetName(Max_Size_1, 'Max Size_1')
  smesh.SetName(Mesh_tri.GetMesh(), 'Mesh_tri')
  smesh.SetName(Mesh_quad.GetMesh(), 'Mesh_quad')

  return Mesh_tri, Mesh_quad
#

from MEDLoader import *

def create_field(mesh_filename, mesh_name, field_name, out_filename=None):
  mesh = MEDLoader.ReadUMeshFromFile(mesh_filename, mesh_name)
  src_field = mesh.fillFromAnalytic(ON_CELLS, 1, "7-sqrt((x-5.)*(x-5.)+(y-5.)*(y-5.)+(z-5.)*(z-5.))")
  src_field.setName(field_name)
  if out_filename:
    MEDLoader.WriteField(out_filename, src_field, True)
  return src_field
#
