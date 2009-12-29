""" 
MEDReader test script. 
Before launching the script necessary MED files has to be placed into ${TESTDIR}/MedData directory 
"""

if not ('servermanager' in dir()):
  from pvsimple import *

import sys
import os
import time

def medread (path, impth):
  print '============================================================'

  if os.path.exists(path):
    filelist = os.listdir(path)
    num = len (filelist)
    
    for i in range(num):
      fname = str(filelist[i])
      if fname.rfind(".med") > -1:
        v = GetRenderView()
        v.UseLight = 1
        v.CameraPosition = [0.0, 0.0, 6.7]
        v.LightSwitch = 0
        v.CameraClippingRange = [4.5, 9.5]
        v.LODThreshold = 5.0
        v.Background = [0., 0., 0.]
        v.CameraParallelScale = 1.8
        
        file_med = MEDReader( FileName=(path + fname))
        
        time.sleep(10)
        SetActiveSource(file_med)
        Show()
        ##Show (file_med, view=v)
        ##Render(view=v)
        Render()
        time.sleep(10)
        WriteImage(filename = (impth +  "medFile_" + str(i+1) + ".png"), view=v, Magnification=2)
        print str(i+1), "Shown file is:  ", fname
        
        Delete(v)
        v1 = CreateRenderView()
        SetActiveView(v1)
        

testdir = os.getenv("TESTDIR")
pvdata = os.getenv("PVDATA")

if __name__ == "__main__":
  medread(path=testdir+"/MedData/", impth=testdir+"/Pic/")
