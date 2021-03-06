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
//  File   : PV_Tools.h
//  Author : Vitaly Smetannikov
//  Module : PARAVIS
//

#ifndef __PV_TOOLS_H__
#define __PV_TOOLS_H__

#include "SALOMEconfig.h"
#include CORBA_SERVER_HEADER(SALOMEDS)
#include CORBA_SERVER_HEADER(SALOMEDS_Attributes)

#include <QStringList>

namespace PARAVIS
{
  class PARAVIS_Base_i;

  SALOMEDS::ListOfFileNames* GetListOfFileNames(const QStringList& theFileNames);


  SALOMEDS::SComponent_var FindOrCreateParaVisComponent(SALOMEDS::Study_ptr theStudyDocument);

  /*!
  * Set/Get flag to restore or not path in paraview state file.
  */
  void SetRestoreParam(SALOMEDS::SComponent_ptr theComponent, bool theRestore);
  bool GetRestoreParam(SALOMEDS::SComponent_ptr theComponent);

  PARAVIS_Base_i* GET_SERVANT(CORBA::Object_ptr aObj);

};

#endif
