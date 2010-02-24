//  Copyright (C) 2007-2008  CEA/DEN, EDF R&D, OPEN CASCADE
//
//  Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
//  CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
//  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
//  VISU VISUGUI : GUI of VISU component
//  File   : PVGUI_Tools.cxx
//  Author : Vitaly Smetannikov
//  Module : PARAVIS
//

#include "PVGUI_Tools.h"
#include "CAM_Module.h"
#include "SALOME_LifeCycleCORBA.hxx"
#include "SalomeApp_Application.h"
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
      Engines::Component_var aComponent = aLCC.FindOrLoad_Component("FactoryServer","PARAVIS");
      PARAVIS_Gen_var aPARAVIS = PARAVIS_Gen::_narrow(aComponent);
      if(!CORBA::is_nil(aPARAVIS)){
        aGen = PARAVIS_Gen_i::GetParavisGenImpl();
      }
    }
    if(!aGen)
      throw std::runtime_error("Can not create PARAVIS_Gen");
    return aGen;
  }

}
