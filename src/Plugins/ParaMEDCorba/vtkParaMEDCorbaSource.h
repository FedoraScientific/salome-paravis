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


#ifndef __vtkParaMEDCorbaSource_h
#define __vtkParaMEDCorbaSource_h

#include "ParaMEDMEM2VTK_defines.hxx"
#include "vtkUnstructuredGridAlgorithm.h"
#include <vtksys/stl/vector>

class vtkDoubleArray;
class vtkUnstructuredGrid;

namespace ParaMEDMEM2VTK
{
  class MEDCouplingMultiFieldsFetcher;
}

class vtkParaMEDCorbaSource : public vtkAlgorithm //
{
public:
  vtkTypeMacro(vtkParaMEDCorbaSource, vtkAlgorithm)
  ParaMEDMEM2VTK_EXPORT static vtkParaMEDCorbaSource* New();
  virtual void PrintSelf( ostream& os, vtkIndent indent );
  const char *GetIORCorba();
  void SetIORCorba(char *ior);
  void SetBufferingPolicy(int pol);
  int GetBufferingPolicy();
protected:
  vtkParaMEDCorbaSource();
  virtual ~vtkParaMEDCorbaSource();
  int FillOutputPortInformation(int vtkNotUsed(port), vtkInformation* info);
  int ProcessRequest(vtkInformation* request, vtkInformationVector** inputVector, vtkInformationVector* outputVector);
  //virtual int RequestUpdateExtent( vtkInformation* request, vtkInformationVector** inInfo, vtkInformationVector* outInfo );
  virtual int RequestInformation( vtkInformation* request, vtkInformationVector** inInfo, vtkInformationVector* outInfo );
  virtual int RequestData( vtkInformation* request, vtkInformationVector** inInfo, vtkInformationVector* outInfo );
  //BTX
  int TotalNumberOfPieces;
  int NumberOfPieces;
  int UpdatePiece;
  int GhostLevel;
  int StartPiece;
  int EndPiece;
  int BufferingPolicy;
  vtkUnstructuredGrid *MyDataSet;
  vtksys_stl::vector<char> IOR;
  ParaMEDMEM2VTK::MEDCouplingMultiFieldsFetcher *mfieldsFetcher;
  //not clean but to avoid to include CORBA.h in this *.h
  static void *Orb;
  //ETX
 private:
  /*void traduceMedMeshToUnstructuredDataset(vtkUnstructuredGrid *ret, int nbOfCells, int nbOfNodes, 
					   int nbOfTypes, const int *types, const int *nbOfEltsPerTypes, const int *nodalConnectivity,
					   const double *coords, int spaceDim);
  vtkDoubleArray *buildDataArrayFromMedCoords(const double *coords, int nbOfNodes, int spaceDim);
  void reorganizeCellConnectivity(int vtkType, int nbOfNodes, const int *medConn, int *vtkConn);
  int transformMedGeomEltType2VtkType(int typeMed, int *nbOfNodesPerCell);*/
  //
  vtkParaMEDCorbaSource( const vtkParaMEDCorbaSource& ); // Not implemented.
  void operator = ( const vtkParaMEDCorbaSource& ); // Not implemented.
};

#endif // __vtkParaMEDCorbaSource_h

