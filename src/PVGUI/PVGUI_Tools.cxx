// Copyright (C) 2010-2014  CEA/DEN, EDF R&D
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
//  File   : PVGUI_Tools.cxx
//  Author : Vitaly Smetannikov
//  Module : PARAVIS

#include "PVGUI_Tools.h"
#include "CAM_Module.h"
#include "SALOME_LifeCycleCORBA.hxx"
#include "SalomeApp_Application.h"
#include "SalomeApp_Study.h"
#include <stdexcept>

using namespace std;

//=============================================================================
namespace PARAVIS
{
  //------------------------------------------------------------
  PARAVIS_Gen_i* GetParavisGen(const CAM_Module* theModule)
  {
    static PARAVIS_Gen_i* aGen = NULL;
    if(!aGen){
      SALOME_LifeCycleCORBA aLCC(SalomeApp_Application::namingService());
      Engines::EngineComponent_var aComponent = aLCC.FindOrLoad_Component("FactoryServer","PARAVIS");
      PARAVIS_Gen_var aPARAVIS = PARAVIS_Gen::_narrow(aComponent);
      if(!CORBA::is_nil(aPARAVIS)){
        aGen = PARAVIS_Gen_i::GetParavisGenImpl();
      }
    }
    if(!aGen)
      throw std::runtime_error("Can not create PARAVIS_Gen");
    return aGen;
  }

  //------------------------------------------------------------
  _PTR(Study) GetCStudy(const CAM_Module* theModule)
  {
    if (theModule && theModule->application()) {
      SalomeApp_Study* activeStudy = 
	dynamic_cast<SalomeApp_Study*>(theModule->application()->activeStudy());
      if (activeStudy) {
	return activeStudy->studyDS();
      }
    }
    
    return _PTR(Study)();
  }
}
