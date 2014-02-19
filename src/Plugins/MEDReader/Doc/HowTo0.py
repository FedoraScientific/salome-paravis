from paraview.simple import *

myMedReader=MEDReader(FileName="/home/H87074/WORK/TESTS_AMA/TESTS0/ssll117a/ssll117a.med")

# list of all keys

keys=myMedReader.GetProperty("FieldsTreeInfo")[::2]

# list all the names of arrays that can be seen (including their spatial discretization)

arr_name_with_dis=[elt.split("/")[-1] for elt in keys]

# list all the names of arrays (Equal to those in the MED File)

arr_name=[elt.split(myMedReader.GetProperty("Separator").GetData())[0] for elt in arr_name_with_dis]

