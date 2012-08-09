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

#include "vtkVisuTableReader.h"
#include "TableParser.h"

#include "vtkObjectFactory.h"
#include "vtkTable.h"
#include "vtkInformationVector.h"
#include "vtkInformation.h"
#include "vtkStreamingDemandDrivenPipeline.h"
#include "vtkVariantArray.h"
#include "vtkStringArray.h"
#include "vtkStringToNumeric.h"

#include <vtkstd/stdexcept>
#include <vtksys/ios/sstream>

#include <vector> // STL include
#include <string> // STL include
using namespace std;

vtkCxxRevisionMacro(vtkVisuTableReader, "$Revision$");
vtkStandardNewMacro(vtkVisuTableReader);

vtkVisuTableReader::vtkVisuTableReader():
  FileName(0)
{
  this->SetNumberOfInputPorts(0);
  this->SetNumberOfOutputPorts(1);

  this->FileName = NULL;
  
  this->DetectNumericColumns = true;
  this->FirstStringAsTitles = false;

  this->TableNumber = 0;

  this->ValueDelimiter = 0;
  this->SetValueDelimiter(" ");
  
  this->AvailableTables = vtkStringArray::New();
}

vtkVisuTableReader::~vtkVisuTableReader()
{
  this->SetFileName(0);
  this->SetValueDelimiter(0);
  this->AvailableTables->Delete();
}

int vtkVisuTableReader::CanReadFile(const char* fname)
{
  return 1;
}

void vtkVisuTableReader::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
  os << indent << "FileName: " 
     << (this->FileName ? this->FileName : "(none)") << endl;
  os << indent << "DetectNumericColumns: "
     << (this->DetectNumericColumns? "true" : "false") << endl;
  os << indent << "ValueDelimiter: "
     << (this->ValueDelimiter ? this->ValueDelimiter : "(none)") << endl;
  os << indent << "TableNumber: " << this->TableNumber<< endl;
}

int vtkVisuTableReader::RequestData(vtkInformation*, 
				    vtkInformationVector**, 
				    vtkInformationVector* outputVector)
{
  vtkTable* const output_table = vtkTable::GetData(outputVector);

  try
    {
      vtkInformation* const outInfo = outputVector->GetInformationObject(0);
      if(outInfo->Has(vtkStreamingDemandDrivenPipeline::UPDATE_PIECE_NUMBER()) &&
	 outInfo->Get(vtkStreamingDemandDrivenPipeline::UPDATE_PIECE_NUMBER()) > 0) 
	{
	  return 1;
	}

      // If the filename is not defined
      if(!this->FileName || this->TableNumber < 0) 
	{
	  return 1;
	}

      // Read table with the given number from the file
      Table2D table = GetTable(this->FileName, this->ValueDelimiter,
			       this->TableNumber, this->FirstStringAsTitles);

      // Set table name
      output_table->GetInformation()->Set(vtkDataObject::FIELD_NAME(), 
					  table.myTitle.c_str());
      
      int nbRows = table.myRows.size();
      int nbCols = table.myRows[0].myValues.size();

      for (int col=0; col < nbCols; col++) 
	{
	  vtkStringArray* newCol = vtkStringArray::New();
	  newCol->SetNumberOfValues(nbRows);

	  // Set value
	  for (int row=0; row < nbRows; row++) 
	    {
	      newCol->SetValue(row, table.myRows[row].myValues[col].c_str());
	    }
	    
	  // Set title
	  bool hasUnit = !table.myColumnUnits[col].empty();
	
	  if (table.myColumnTitles[col].empty()) 
	    {
	      vtkstd::stringstream buffer;
	      if (hasUnit) 
		{
		  buffer << col <<" [" << table.myColumnUnits[col].c_str() << "]";
		}
	      else 
		{
		  buffer << col;
		}
	      newCol->SetName(buffer.str().c_str());
	    }
	else 
	  {
	    if (hasUnit) 
	      {
		vtkstd::stringstream buffer;
		buffer << table.myColumnTitles[col].c_str() 
		       <<" [" << table.myColumnUnits[col].c_str() << "]";
		newCol->SetName(buffer.str().c_str());
	      }
	  else 
	    {
	      newCol->SetName(table.myColumnTitles[col].c_str());
	    }
	  }
	
	  output_table->AddColumn(newCol);
	  newCol->Delete();
	}
      
      // Detect numeric columns if needed
      if (this->DetectNumericColumns) 
	{
	  vtkStringToNumeric* convertor = vtkStringToNumeric::New();
	  vtkTable* clone = output_table->NewInstance();
	  clone->ShallowCopy(output_table);
	  convertor->SetInput(clone);
	  convertor->Update();
	  clone->Delete();
	  output_table->ShallowCopy(convertor->GetOutputDataObject(0));
	  convertor->Delete();
	}
    } 
  catch(vtkstd::exception& e) 
    {
      vtkErrorMacro(<< "caught exception: " << e.what() << endl);
      output_table->Initialize();
    }
  catch(...) 
    {
      vtkErrorMacro(<< "caught unknown exception." << endl);
      output_table->Initialize();
    }
  
  return 1;
}

vtkStringArray* vtkVisuTableReader::GetAvailableTables()
{
  this->AvailableTables->Initialize();
  
  vector<string> titles = 
    GetTableNames(this->FileName, this->ValueDelimiter,
		  this->FirstStringAsTitles);
  
  for(int i = 0; i < titles.size(); i++)
    {
      this->AvailableTables->InsertNextValue(titles[i].c_str());
    }
  
  return this->AvailableTables;
}
