// PARAVIS : ParaView wrapper SALOME module
//
// Copyright (C) 2010-2013  CEA/DEN, EDF R&D
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License.
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
// File   : PVGUI_Tools.h
// Author : Vitaly Smetannikov
//

#ifndef PVGUI_Tools_H
#define PVGUI_Tools_H

#include "PARAVIS_Gen_i.hh"

#include "SALOMEDSClient_Study.hxx"

class CAM_Module;

namespace PARAVIS 
{
  
  PARAVIS_Gen_i* GetParavisGen(const CAM_Module* theModule);

  _PTR(Study)    GetCStudy(const CAM_Module* theModule);
};

#endif // PVGUI_Tools_H
