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

#ifndef __vtkExtractFieldFilter_h
#define __vtkExtractFieldFilter_h

#include <vtkMultiBlockDataSetAlgorithm.h>
#include <list>
#include <string>

class vtkDataObjectTreeIterator;


/**
 * Implements a class of a filter which extract a support mesh of a data field.
 * It processes MultiBlockDataSet data structure extracting blocks which contain
 * a field with FieldName.
 * If field name is not defined then the filter just copies input data to output data
*/
class vtkExtractFieldFilter : public vtkMultiBlockDataSetAlgorithm
{
public:
	/// Returns pointer on a new instance of the class
	static vtkExtractFieldFilter* New();

	vtkTypeMacro(vtkExtractFieldFilter, vtkMultiBlockDataSetAlgorithm);

	/// Prints current state of the objects
	virtual void PrintSelf(ostream& os, vtkIndent indent);

	/// This method is used for definition of a field name for filtering from GUI
	virtual void SetInputArrayToProcess(int idx, int port, int connection,
		  	  	  	  	  	  	  	  	int fieldAssociation, const char* name);

	/// Set and Get methods for FieldName
	vtkSetStringMacro(FieldName);
	vtkGetStringMacro(FieldName);

protected:
	/// Constructor
	vtkExtractFieldFilter();

	/// Destructor
	virtual ~vtkExtractFieldFilter();

	/// A method which is called on filtering data
	virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *);

	/// Copies a sub-tree defined by a Data Set Block
	void CopySubTree(vtkDataObjectTreeIterator* theLoc, vtkMultiBlockDataSet* theOutput, vtkMultiBlockDataSet* theInput);

	/// Returns a list of strings with names of fields defined in the given Data Object
	void GetListOfFields(vtkDataObject* theObject, std::list<std::string>& theList) const;

	/// Returns True if the given Data Object has to be copied into output
	bool IsToCopy(vtkDataObject* theObject) const;

private:
	/// Methods for copy of the filter: Not implemented
	vtkExtractFieldFilter(const vtkExtractFieldFilter&); // Not implemented
	void operator=(const vtkExtractFieldFilter&); // Not implemented

	char* FieldName;
};

#endif
