# To be run in SALOME environment (salome shell)
# A SALOME session MUST have been started (salome -t)

import os

from dataset import create_geometry, create_meshes, create_field

out_basename = "smooth_surface"
dir_name = os.path.dirname(os.path.abspath(__file__))
brep_filename = os.path.join(dir_name, out_basename+".brep")
med_filename = os.path.join(dir_name, out_basename+".med")
field_filename = os.path.join(dir_name, out_basename+"_and_field.med")

def generate_data():
  geometry = create_geometry(out_filename=brep_filename)
  mesh_tri, mesh_quad = create_meshes(geometry, out_filename=med_filename)
  field = create_field(med_filename, "Mesh_tri", "field_on_tri_cells", out_filename=field_filename)
#

import salome
salome.salome_init()

generate_data()

from medio import load_mesh, load_field
mesh = load_mesh(med_filename, mesh_name="Mesh_tri")
field = load_field(field_filename, field_name="field_on_tri_cells")

from medviewer import MEDViewer
viewer = MEDViewer(interactive=False)
viewer.display_mesh(mesh, pause=True)

viewer2 = MEDViewer(interactive=True)
viewer2.display_field(field, pause=False)
