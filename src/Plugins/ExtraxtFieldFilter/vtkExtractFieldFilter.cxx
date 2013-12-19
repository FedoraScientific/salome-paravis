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
#include <vtkStringArray.h>
#include <vtkCompositeDataToUnstructuredGridFilter.h>


#include <string.h>

using namespace std;



bool isContainsName(const std::list<std::string>& aList, const std::string& theName)
{
	std::list<std::string>::const_iterator aIt;
	for (aIt = aList.begin(); aIt != aList.end(); aIt++) {
		if ((*aIt).compare(theName) == 0)
			return true;
	}
	return false;
}


void appendIfNotExists(const std::list<std::string>& theSrc, std::list<std::string>& theDest)
{
	std::list<std::string>::const_iterator aIt;
	for (aIt = theSrc.begin(); aIt != theSrc.end(); aIt++) {
		if (!isContainsName(theDest, *aIt))
			theDest.push_back(*aIt);
	}
}




vtkStandardNewMacro(vtkExtractFieldFilter);



vtkExtractFieldFilter::vtkExtractFieldFilter()
:vtkMultiBlockDataSetAlgorithm()
{
	this->Field = NULL;
	this->FieldList = vtkStringArray::New();
}


vtkExtractFieldFilter::~vtkExtractFieldFilter()
{
	this->FieldList->Delete();
	if (this->Field)
		delete [] this->Field;
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
	int i;
	std::list<int> toDelList;
	for (i = 0; i < aInput->GetNumberOfBlocks(); i++) {
		this->CopySubTree(i, aOutput, aInput, toDelList);
	}
	std::list<int>::const_reverse_iterator aIt;
	for (aIt = toDelList.rbegin(); aIt != toDelList.rend(); ++aIt)
		aOutput->RemoveBlock(*aIt);
	return 1;
}

//----------------------------------------------------------------------------
int vtkExtractFieldFilter::RequestInformation(vtkInformation* reqInfo,
											  vtkInformationVector **theInputVector,
											  vtkInformationVector *theOutputVector)
{
	// get the info objects
	vtkMultiBlockDataSet* aInput = vtkMultiBlockDataSet::GetData(theInputVector[0], 0);
	vtkMultiBlockDataSet* aOutput = vtkMultiBlockDataSet::GetData(theOutputVector, 0);

	vtkDataObjectTreeIterator* aIter = aInput->NewTreeIterator();
	aIter->VisitOnlyLeavesOff();
	int i = 0;
	std::list<std::string> aList;
	for (aIter->InitTraversal(); !aIter->IsDoneWithTraversal(); aIter->GoToNextItem()) {
		std::list<std::string> aSubList = this->GetListOfFields(aIter, aInput);
		appendIfNotExists(aSubList, aList);
	}
	this->FieldList->SetNumberOfValues(aList.size());
	std::list<std::string>::const_iterator aIt;
	i = 0;
	for (aIt = aList.begin(); aIt != aList.end(); aIt++) {
		this->FieldList->SetValue(i, *aIt);
		i++;
	}

	return this->Superclass::RequestInformation(reqInfo, theInputVector, theOutputVector);
}

//----------------------------------------------------------------------------
void vtkExtractFieldFilter::CopySubTree(int theLoc,
										vtkMultiBlockDataSet* theOutput,
										vtkMultiBlockDataSet* theInput,
										std::list<int>& toDel)
{
	vtkDataObject* aInputNode = theInput->GetBlock(theLoc);
	if (aInputNode->IsA("vtkCompositeDataSet")) {
		vtkMultiBlockDataSet* aCInput = vtkMultiBlockDataSet::SafeDownCast(aInputNode);
		vtkMultiBlockDataSet* aCOutput = vtkMultiBlockDataSet::SafeDownCast(theOutput->GetBlock(theLoc));
		std::list<int> toDelList;
		int i;
		for (i = 0; i < aCInput->GetNumberOfBlocks(); i++) {
			this->CopySubTree(i, aCOutput, aCInput, toDelList);
		}
		std::list<int>::const_reverse_iterator aIt;
		for (aIt = toDelList.rbegin(); aIt != toDelList.rend(); ++aIt)
			aCOutput->RemoveBlock(*aIt);
		if (aCOutput->GetNumberOfBlocks() == 0)
			toDel.push_back(theLoc);
	} else {
		if (IsToCopy(aInputNode)) {
			vtkDataObject* aClone = aInputNode->NewInstance();
			aClone->ShallowCopy(aInputNode);
			theOutput->SetBlock(theLoc, aClone);
			aClone->Delete();
		} else {
			toDel.push_back(theLoc);
		}
	}
}

//----------------------------------------------------------------------------
std::list<std::string> vtkExtractFieldFilter::GetListOfFields(vtkDataObjectTreeIterator* theLoc, vtkMultiBlockDataSet* theInput)
{
	std::list<std::string> aList;
	vtkDataObject* aInputNode = theInput->GetDataSet(theLoc);
	if (!aInputNode->IsA("vtkCompositeDataSet")) {
		std::list<std::string> aSubList = this->GetListOfFields(aInputNode);
		appendIfNotExists(aSubList, aList);
	} else {
		vtkCompositeDataSet* aCInput = vtkCompositeDataSet::SafeDownCast(aInputNode);
		vtkCompositeDataIterator* aIter = aCInput->NewIterator();
		vtkDataObjectTreeIterator* aTreeIter = vtkDataObjectTreeIterator::SafeDownCast(aIter);
		if (aTreeIter) {
			aTreeIter->VisitOnlyLeavesOff();
		}
		for (aIter->InitTraversal(); !aIter->IsDoneWithTraversal(); aIter->GoToNextItem()) {
			vtkDataObject* aCurNode = aIter->GetCurrentDataObject();
			std::list<std::string> aSubList = this->GetListOfFields(aCurNode);
			appendIfNotExists(aSubList, aList);
		}
		aIter->Delete();
	}
	return aList;
}

//----------------------------------------------------------------------------
std::list<std::string> vtkExtractFieldFilter::GetListOfFields(vtkDataObject* theObject) const
{
	std::list<std::string> aList;

	if (theObject->IsA("vtkDataSet")) {
		vtkDataSet* aDataSet = vtkDataSet::SafeDownCast(theObject);
		vtkPointData* aPntData = aDataSet->GetPointData();
		int aNbArrays = aPntData->GetNumberOfArrays();
		for (int i = 0; i < aNbArrays; i++) {
			const char* aName = aPntData->GetArrayName(i);
			aList.push_back(aName);
		}
		vtkCellData* aCellData = aDataSet->GetCellData();
		aNbArrays = aCellData->GetNumberOfArrays();
		for (int i = 0; i < aNbArrays; i++) {
			const char* aName = aCellData->GetArrayName(i);
			aList.push_back(aName);
		}
	}
	return aList;
}


//----------------------------------------------------------------------------
bool vtkExtractFieldFilter::IsToCopy(vtkDataObject* theObject) const
{
	if (this->Field == NULL)
		return true;

	std::list<std::string> aList = this->GetListOfFields(theObject);

	std::list<std::string>::const_iterator aIt;
	std::string aTestStr = this->Field;
	for (aIt = aList.begin(); aIt != aList.end(); ++aIt)
		if (aTestStr.compare(*aIt) == 0)
			return true;

	return false;
}


//----------------------------------------------------------------------------
void vtkExtractFieldFilter::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
  os << indent << "Field name: " << this->Field << endl;
  this->FieldList->PrintSelf(os, indent);
}

