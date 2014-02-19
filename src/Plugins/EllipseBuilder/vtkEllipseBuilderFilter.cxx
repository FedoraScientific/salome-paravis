// Copyright (C) 2010-2014  CEA/DEN, EDF R&D
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

#include "vtkEllipseBuilderFilter.h"

#include <vtkInformation.h>
#include <vtkInformationVector.h>
#include <vtkMultiBlockDataSet.h>
#include <vtkDataObjectTreeIterator.h>
#include <vtkDataSet.h>
#include <vtkPoints.h>
#include <vtkPointData.h>
#include <vtkDataArray.h>
#include <vtkStringArray.h>
#include <vtkUnstructuredGrid.h>
#include <vtkPolyVertex.h>
#include <vtkMath.h>

#include <complex>
#include <algorithm>
#include <vector>
#include <cmath>

using namespace std;

//------------------------------------------------------------------------------
bool isStringInList(const list<string>& aList, const string& theName)
{
  list<string>::const_iterator anIter;
  for (anIter = aList.begin(); anIter != aList.end(); anIter++)
    {
      // Compares the values of the string.
      if ( (*anIter).compare(theName) == 0)
	return true;
    }
  return false;
}

//------------------------------------------------------------------------------
void add2List(const list<string>& theSource, list<string>& theDestination)
{
  list<string>::const_iterator anIter;
  for (anIter = theSource.begin(); anIter != theSource.end(); anIter++)
    {
      // Add the item to the list if it does not exist
      if (!isStringInList(theDestination, *anIter))
	theDestination.push_back(*anIter);
    }
}

//------------------------------------------------------------------------------
double maximumElement(vector< complex<double> >& theList)
{
  double aMaximum;
  vector<double> tmpList;
  vector< complex<double> >::iterator it;
  for (it = theList.begin(); it != theList.end(); ++it)
    {
      tmpList.push_back(((*it)*conj(*it)).real());
    }
  aMaximum = *max_element(tmpList.begin(), tmpList.end());
  return aMaximum;
}

//------------------------------------------------------------------------------
list<std::string> GetListOfFields(vtkDataObject* theObject)
{
  list<string> aList;
  
  if (theObject->IsA("vtkDataSet"))
    {
      vtkDataSet* aDataSet = vtkDataSet::SafeDownCast(theObject);
      vtkPointData* aPntData = aDataSet->GetPointData();
      
      // Add fields name on points
      int aNbArrays = aPntData->GetNumberOfArrays();
      for (int i = 0; i<aNbArrays; i++)
        {
	  if( vtkDataArray* anArray = aPntData->GetArray(i) ) {
	    //Data array should contains at least 3 components (1 - DX, 2 - DY, 3 - DZ)
	    if( anArray->GetNumberOfComponents() >= 3 ) {
	      const char* aName = aPntData->GetArrayName(i);
	      aList.push_back(aName);
	    }
	  }
        }
    }
  return aList;
}

//------------------------------------------------------------------------------
vtkStandardNewMacro(vtkEllipseBuilderFilter);

//------------------------------------------------------------------------------
vtkEllipseBuilderFilter::vtkEllipseBuilderFilter() : vtkMultiBlockDataSetAlgorithm()
{
  this->RealField = NULL;
  this->ImagField = NULL;
  this->FieldList = vtkStringArray::New();
}

//------------------------------------------------------------------------------
vtkEllipseBuilderFilter::~vtkEllipseBuilderFilter()
{
  this->SetRealField(NULL);
  this->SetRealField(NULL);
  this->FieldList->Delete();
}

//------------------------------------------------------------------------------
int vtkEllipseBuilderFilter::RequestData(vtkInformation* vtkNotUsed(request),
                                         vtkInformationVector** theInputVector,
                                         vtkInformationVector* theOutputVector)
{
  int aResolution = this->Resolution;
  if(aResolution <=0 )
    return 0;
  if( this->ScaleFactor == 0.0 )
    return 0;

  int anAxis = this->Axis;
  
  // Get the info objects
  vtkMultiBlockDataSet* anInputMDSet = vtkMultiBlockDataSet::GetData(theInputVector[0], 0);
  vtkMultiBlockDataSet* anOutputMDSet = vtkMultiBlockDataSet::GetData(theOutputVector, 0);
  
  double a, b, z_point;
  vector< complex<double> > circle;

  // Points Ellipses
  double range, min, max, delta;
  if(this->EndAngle > this->StartAngle) {
    min = this->StartAngle;
    max = this->EndAngle;
  } else {
    min = this->StartAngle;
    max = this->EndAngle + 360.;
  }
  range = max-min;
  delta = range/(double)aResolution;
  for (double iter = min;  iter < max; iter+=delta)
    {
      a = cos(vtkMath::RadiansFromDegrees(iter));
      b = sin(vtkMath::RadiansFromDegrees(iter));
      complex<double> aVal(a, b);
      circle.push_back(aVal);
    }
  
  //Scale Factor    
  double aScaleFactor;
  aScaleFactor = 1./this->ScaleFactor;  
    
  vtkDataObjectTreeIterator* anIter = anInputMDSet->NewTreeIterator();
  anIter->VisitOnlyLeavesOff();
  bool created = false;
  for (anIter->InitTraversal(); !anIter->IsDoneWithTraversal(); anIter->GoToNextItem())
    {
      vtkDataObject* anInputNode = anInputMDSet->GetDataSet(anIter);
      if (anInputNode->IsA("vtkDataSet"))
        {
	  vtkUnstructuredGrid* aSourceDS = vtkUnstructuredGrid::SafeDownCast(anInputNode);
	  if(!aSourceDS) continue;
	  
	  vtkPointData* aPointData = aSourceDS->GetPointData();
	  if(!aPointData) continue;
          
	  int aNumberPoints = aSourceDS->GetNumberOfPoints();
          
	  vtkDataArray* aRealArray = vtkDataArray::SafeDownCast(aPointData->GetArray(this->RealField));
	  vtkDataArray* anImagArray = vtkDataArray::SafeDownCast(aPointData->GetArray(this->ImagField));
	  
	  if(!aRealArray || !anImagArray) continue;
          
	  int aNumberOfRealComponents = aRealArray->GetNumberOfComponents();
	  int aNumberOfImagComponents = anImagArray->GetNumberOfComponents();	           
	  if (aNumberOfRealComponents >= 3 and aNumberOfImagComponents >= 3)
            {
	      anOutputMDSet->CopyStructure(anInputMDSet);
	      vtkUnstructuredGrid* aCloneDS = aSourceDS->NewInstance();
	      vtkPoints* aClonePoints = vtkPoints::New();
	      aCloneDS->SetPoints(aClonePoints);
	      aClonePoints->Delete();
	      anOutputMDSet->SetDataSet(anIter, aCloneDS);
	      aCloneDS->Delete();	      
	      double rx, ry, ix, iy;
	      created = true;
	      for (int j = 0; j < aNumberPoints; j++)
                {
		  z_point = aSourceDS->GetPoint(j)[2];
		  
		  if (anAxis == 2)      // Z : DX and DY
		    {
		      rx = aRealArray->GetTuple(j)[0];
		      ry = aRealArray->GetTuple(j)[1];
		      ix = anImagArray->GetTuple(j)[0];
		      iy = anImagArray->GetTuple(j)[1];
		    }
		  else if (anAxis == 1) // Y : DX and DZ
		    {
		      rx = aRealArray->GetTuple(j)[0];
		      ry = aRealArray->GetTuple(j)[2];
		      ix = anImagArray->GetTuple(j)[0];
		      iy = anImagArray->GetTuple(j)[2];
                    }
		  else                  // X : DY and DZ
                    {
		      rx = aRealArray->GetTuple(j)[1];
		      ry = aRealArray->GetTuple(j)[2];
		      ix = anImagArray->GetTuple(j)[1];
		      iy = anImagArray->GetTuple(j)[2];
                    }
		  
		  complex<double> x(rx, ix);
		  complex<double> y(ry, iy);		   
		  
		  x = x / aScaleFactor;
		  y = y / aScaleFactor;		    

		  double x_point, y_point;
		  for (int r = 0; r < circle.size(); r++)
		    {
		      x_point = (x*circle[r]).real();
		      y_point = (y*circle[r]).real();
		      vtkIdType anId[1];
		      if (anAxis == 2)
                        anId[0] = aClonePoints->InsertNextPoint(x_point, y_point, z_point);
		      else if (anAxis == 1)
                        anId[0] = aClonePoints->InsertNextPoint(x_point, z_point, y_point);
		      else
			anId[0] = aClonePoints->InsertNextPoint(z_point, x_point, y_point);		      
		      aCloneDS->InsertNextCell(VTK_VERTEX, 1, anId);
		    }		  
                }
            }
	  else
            {
	      continue;
            }
        }
    }
  anIter->Delete();
  if(!created)
    return 0;
  return 1;
}

//------------------------------------------------------------------------------
int vtkEllipseBuilderFilter::RequestInformation(vtkInformation* request,
                                                vtkInformationVector **theInputVector,
                                                vtkInformationVector *theOutputVector)
{
    // Retrieve an instance of vtkMultiBlockDataSet class from an information object.
    vtkMultiBlockDataSet* anInputMDataSet = vtkMultiBlockDataSet::GetData(theInputVector[0], 0);

    list<string> aList;
    vtkDataObjectTreeIterator* anIter = anInputMDataSet->NewTreeIterator();
    anIter->VisitOnlyLeavesOff();
    for (anIter->InitTraversal(); !anIter->IsDoneWithTraversal(); anIter->GoToNextItem())
    {
        vtkDataObject* anInputNode = anInputMDataSet->GetDataSet(anIter);
        if (anInputNode->IsA("vtkDataSet"))
        {
	  list<string> aSubList = GetListOfFields(anInputNode);
	  add2List(aSubList, aList);
        }
    }
    anIter->Delete();
    
    this->FieldList->Reset();
    this->FieldList->SetNumberOfValues(aList.size());
    list<string>::const_iterator anIterName;
    int i = 0;
    for (anIterName = aList.begin(); anIterName != aList.end(); anIterName++)
    {
        this->FieldList->SetValue(i, *anIterName);
        i++;
    }
    
    return this->Superclass::RequestInformation(request, theInputVector, theOutputVector);
}


//------------------------------------------------------------------------------
vtkStringArray* vtkEllipseBuilderFilter::GetFieldList()
{
    return this->FieldList;
}

//------------------------------------------------------------------------------
void vtkEllipseBuilderFilter::PrintSelf(ostream& os, vtkIndent indent)
{
    this->Superclass::PrintSelf(os, indent);
    os << indent << "Real Field : " << this->RealField << endl;
    os << indent << "Imag Field : " << this->ImagField << endl;
    os << indent << "Start Angle : " << this->StartAngle << endl;
    os << indent << "End Angle : " << this->EndAngle << endl;
    os << indent << "Scale Factor : " << this->ScaleFactor << endl;
    os << indent << "Scale Resolution : " << this->Resolution << endl;
    os << indent << "Axis : " << this->Axis << endl;
}

