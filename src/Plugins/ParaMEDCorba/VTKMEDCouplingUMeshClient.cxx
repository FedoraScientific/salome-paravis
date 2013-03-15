// Copyright (C) 2010-2012  CEA/DEN, EDF R&D
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

#include "VTKMEDCouplingUMeshClient.hxx"

#include "vtkPoints.h"
#include "vtkCellArray.h"
#include "vtkDoubleArray.h"
#include "vtkSmartPointer.h"
#include "vtkUnstructuredGrid.h"

#include <set>
#include <vector>
#include <string>
#include <algorithm>

static const int ParaMEDMEM2VTKTypeTraducer[34]={1,3,21,5,9,7,22,34,23,28,-1,-1,-1,-1,10,14,13,-1,12,-1,24,-1,16,27,-1,26,-1,29,-1,-1,25,42,-1,4};

void ParaMEDMEM2VTK::FillMEDCouplingUMeshInstanceFrom(SALOME_MED::MEDCouplingUMeshCorbaInterface_ptr meshPtr, vtkUnstructuredGrid *ret, bool& isPolyh)
{
  meshPtr->Register();
  //
  SALOME_TYPES::ListOfDouble *tinyD;
  SALOME_TYPES::ListOfLong *tinyI;
  SALOME_TYPES::ListOfString *tinyS;
  meshPtr->getTinyInfo(tinyD,tinyI,tinyS);
  //
  int spaceDim=(*tinyI)[1];
  int nbOfNodes=(*tinyI)[2];
  int meshDim=(*tinyI)[5];
  int nbOfCells=(*tinyI)[6];
  int meshLength=(*tinyI)[7];
  std::string name((*tinyS)[0]);
  //std::vector<std::string> compoNames(spaceDim);
  //for(int i=0;i<spaceDim;i++)
  //  compoNames[i]=(*tinyS)[i+4];
  delete tinyD;
  delete tinyI;
  delete tinyS;
  //
  ret->Initialize();
  ret->Allocate(nbOfCells);
  vtkPoints *points=vtkPoints::New();
  vtkDoubleArray *da=vtkDoubleArray::New();
  da->SetNumberOfComponents(3);
  da->SetNumberOfTuples(nbOfNodes);
  double *pts=da->GetPointer(0);
  //
  SALOME_TYPES::ListOfLong *a1Corba;
  SALOME_TYPES::ListOfDouble *a2Corba;
  meshPtr->getSerialisationData(a1Corba,a2Corba);
  if(spaceDim==3)
    {
      int myLgth=a2Corba->length();
      for(int i=0;i<myLgth;i++)
        *pts++=(*a2Corba)[i];
    }
  else
    {
      int offset=3-spaceDim;
      for(int i=0;i<nbOfNodes;i++)
        {
          for(int j=0;j<spaceDim;j++)
            *pts++=(*a2Corba)[spaceDim*i+j];
          std::fill(pts,pts+offset,0.);
          pts+=offset;
        }
    }
  //
  vtkIdType *tmp=new vtkIdType[1000];
  isPolyh=false;
  for(int i=0;i<nbOfCells;i++)
    {
      int pos=(*a1Corba)[i];
      int pos1=(*a1Corba)[i+1];
      int nbOfNodeInCurCell=pos1-pos-1;
      int typeOfCell=(*a1Corba)[pos+nbOfCells+1];
      for(int j=0;j<nbOfNodeInCurCell;j++)
        tmp[j]=(*a1Corba)[pos+1+j+nbOfCells+1];
      int vtkType=ParaMEDMEM2VTKTypeTraducer[typeOfCell];
      if(vtkType!=42)
        ret->InsertNextCell(vtkType,nbOfNodeInCurCell,tmp);
      else
        {//polyhedron
          isPolyh=true;
          std::set<vtkIdType> s(tmp,tmp+nbOfNodeInCurCell);
          vtkSmartPointer<vtkCellArray> faces=vtkSmartPointer<vtkCellArray>::New();
          int nbOfFaces=std::count(tmp,tmp+nbOfNodeInCurCell,-1)+1;
          vtkIdType *work=tmp;
          for(int i=0;i<nbOfFaces;i++)
            {
              vtkIdType *work2=std::find(work,tmp+nbOfNodeInCurCell,-1);
              int nbOfNodesInFace=std::distance(work,work2);
              faces->InsertNextCell(nbOfNodesInFace,work);
              work=work2+1;
            }
          s.erase(-1);
          std::vector<vtkIdType> v(s.begin(),s.end());
          ret->InsertNextCell(VTK_POLYHEDRON,v.size(),&v[0],nbOfFaces,faces->GetPointer());
        }
    }
  delete [] tmp;
  //
  delete a1Corba;
  delete a2Corba;
  //
  ret->SetPoints(points);
  points->Delete();
  points->SetData(da);
  da->Delete();
  //
  meshPtr->UnRegister();
}
