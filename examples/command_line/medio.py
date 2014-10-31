def __select_data(reader, dataname=None):
  if dataname:
    keys = reader.GetProperty("FieldsTreeInfo")[::2]
    # list all the names of arrays that can be seen (including their spatial discretization)
    arr_name_with_dis = [elt.split("/")[-1] for elt in keys]
    # list all the names of arrays (Equal to those in the MED File)
    separator = reader.GetProperty("Separator").GetData()
    arr_name = [elt.split(separator)[0] for elt in arr_name_with_dis]
    for idx in range(len(keys)):
      if arr_name[idx] == dataname:
        reader.AllArrays = keys[idx]
        break

  return reader
#

def get_element_type(reader):
  # Return 'P0', 'P1'...
  separator = reader.GetProperty("Separator").GetData()
  return reader.AllArrays[0].split(separator)[1]
#

def get_element_name(reader):
  separator = reader.GetProperty("Separator").GetData()
  return reader.AllArrays[0].split(separator)[0].split("/")[-1]
#

def load_mesh(med_filename, mesh_name=None):
  import pvsimple
  reader = pvsimple.MEDReader(FileName=med_filename)
  return __select_data(reader, mesh_name)
#

def load_field(med_filename, field_name=None):
  import pvsimple
  reader = pvsimple.MEDReader(FileName=med_filename)
  return __select_data(reader, field_name)
#
