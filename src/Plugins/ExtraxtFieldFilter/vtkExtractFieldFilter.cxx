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

#include "vtkExtractFieldFilter.h"

#include <vtkInformation.h>
#include <vtkInformationVector.h>
#include <vtkObjectFactory.h>
#include <vtkMultiBlockDataSet.h>
#include <vtkDataObjectTreeIterator.h>
#include <vtkFieldData.h>
#include <vtkStringArray.h>
#include <vtkDataSet.h>
#include <vtkPointData.h>
#include <vtkCellData.h>
#include <vtkDataArray.h>

#include <string.h>

using namespace std;

vtkStandardNewMacro(vtkExtractFieldFilter);

vtkExtractFieldFilter::vtkExtractFieldFilter()
:vtkMultiBlockDataSetAlgorithm()
{
	this->FieldName = NULL;
}


vtkExtractFieldFilter::~vtkExtractFieldFilter()
{
}

//----------------------------------------------------------------------------
int vtkExtractFieldFilter::RequestData(vtkInformation* vtkNotUsed(request),
									   vtkInformationVector** theInputVector,
									   vtkInformationVector* theOutputVector)
{
	// get the info objects
	vtkMultiBlockDataSet* aInput = vtkMultiBlockDataSet::GetData(theInputVector[0], 0);
	vtkMultiBlockDataSet* aOutput = vtkMultiBlockDataSet::GetData(theOutputVector, 0);

	aOutput->CopyStructure(aInput);

	// Copy selected blocks over to the output.
	vtkDataObjectTreeIterator* aIter = aInput->NewTreeIterator();
	aIter->VisitOnlyLeavesOff();
	for (aIter->InitTraversal(); !aIter->IsDoneWithTraversal(); aIter->GoToNextItem()) {
		this->CopySubTree(aIter, aOutput, aInput);
	}
	aIter->Delete();
	return 1;
}


//----------------------------------------------------------------------------
void vtkExtractFieldFilter::CopySubTree(vtkDataObjectTreeIterator* theLoc,
										vtkMultiBlockDataSet* theOutput,
										vtkMultiBlockDataSet* theInput)
{
	vtkDataObject* aInputNode = theInput->GetDataSet(theLoc);
	if (!aInputNode->IsA("vtkCompositeDataSet")) {
		if (IsToCopy(aInputNode)) {
			vtkDataObject* aClone = aInputNode->NewInstance();
			aClone->ShallowCopy(aInputNode);
			theOutput->SetDataSet(theLoc, aClone);
			aClone->Delete();
		}
	} else {
		vtkCompositeDataSet* aCInput = vtkCompositeDataSet::SafeDownCast(aInputNode);
		vtkCompositeDataSet* aCOutput = vtkCompositeDataSet::SafeDownCast(theOutput->GetDataSet(theLoc));
		vtkCompositeDataIterator* aIter = aCInput->NewIterator();
		vtkDataObjectTreeIterator* aTreeIter = vtkDataObjectTreeIterator::SafeDownCast(aIter);
		if (aTreeIter) {
			aTreeIter->VisitOnlyLeavesOff();
		}
		for (aIter->InitTraversal(); !aIter->IsDoneWithTraversal(); aIter->GoToNextItem()) {
			vtkDataObject* aCurNode = aIter->GetCurrentDataObject();
			if (IsToCopy(aInputNode)) {
				vtkDataObject* aClone = aCurNode->NewInstance();
				aClone->ShallowCopy(aCurNode);
				aCOutput->SetDataSet(aIter, aClone);
				aClone->Delete();
			}
		}
		aIter->Delete();
	}

}


//----------------------------------------------------------------------------
void vtkExtractFieldFilter::GetListOfFields(vtkDataObject* theObject, std::list<std::string>& theList) const
{
	theList.clear();

	if (theObject->IsA("vtkDataSet")) {
		vtkDataSet* aDataSet = vtkDataSet::SafeDownCast(theObject);
		vtkPointData* aPntData = aDataSet->GetPointData();
		int aNbArrays = aPntData->GetNumberOfArrays();
		for (int i = 0; i < aNbArrays; i++) {
			const char* aName = aPntData->GetArrayName(i);
			theList.push_back(aName);
		}
		vtkCellData* aCellData = aDataSet->GetCellData();
		aNbArrays = aCellData->GetNumberOfArrays();
		for (int i = 0; i < aNbArrays; i++) {
			const char* aName = aCellData->GetArrayName(i);
			theList.push_back(aName);
		}
	}

}


//----------------------------------------------------------------------------
bool vtkExtractFieldFilter::IsToCopy(vtkDataObject* theObject) const
{
	if (this->FieldName == NULL)
		return true;

	std::list<std::string> aList;
	GetListOfFields(theObject, aList);

	std::list<std::string>::const_iterator aIt;
	std::string aTestStr = this->FieldName;
	for (aIt = aList.begin(); aIt != aList.end(); ++aIt)
		if (aTestStr.compare(*aIt) == 0)
			return true;

	return false;
}



//----------------------------------------------------------------------------
void vtkExtractFieldFilter::SetInputArrayToProcess(int idx, int port, int connection,
		  	  	  	  	  	  	  	  	  	  	   int fieldAssociation, const char* name)
{
	this->SetFieldName(name);
}


//----------------------------------------------------------------------------
void vtkExtractFieldFilter::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
  os << indent << "Field name: " << FieldName << endl;
}

