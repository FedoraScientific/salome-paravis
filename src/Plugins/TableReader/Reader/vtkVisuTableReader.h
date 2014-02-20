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

#ifndef __vtkVisuTableReader_h_
#define __vtkVisuTableReader_h_

#include "vtkTableAlgorithm.h"

class vtkStringArray;

class VTK_EXPORT vtkVisuTableReader: public vtkTableAlgorithm
{
public:
  static vtkVisuTableReader* New();
  vtkTypeMacro(vtkVisuTableReader, vtkTableAlgorithm)
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Specifies the name of the file
  vtkGetStringMacro(FileName);
  vtkSetStringMacro(FileName);

  // Description:
  // When set to true, the reader will detect numeric columns and create
  // vtkDoubleArray or vtkIntArray for those instead of vtkStringArray.
  vtkSetMacro(DetectNumericColumns, bool);
  vtkGetMacro(DetectNumericColumns, bool);
  vtkBooleanMacro(DetectNumericColumns, bool);

  // Description:
  // When set to true, the reader will interpret the first string as
  // column titles.
  vtkSetMacro(FirstStringAsTitles, bool);
  vtkGetMacro(FirstStringAsTitles, bool);
  vtkBooleanMacro(FirstStringAsTitles, bool);

  // Description:
  // Specifies the number of the table
  vtkSetMacro(TableNumber, int);
  vtkGetMacro(TableNumber, int);

  // Description:
  // Specifies string that will be used to separate values
  vtkSetStringMacro(ValueDelimiter);
  vtkGetStringMacro(ValueDelimiter);

  // Description:
  // Determine whether the given file can be read
  virtual int CanReadFile(const char* fname);
  
  // Description:
  // Returns the available table names.
  virtual vtkStringArray* GetAvailableTables();

protected:
  vtkVisuTableReader();
  ~vtkVisuTableReader();
  
  // Description:
  // This is called by the superclass.
  virtual int RequestData(vtkInformation*, vtkInformationVector**,
      vtkInformationVector*);
  
  // name of the file to read from
  char* FileName;

  // defines whether to detect numeric columns
  bool DetectNumericColumns;

  // defines whether to treat first string as column titles
  bool FirstStringAsTitles;

  // number of the table to read
  int TableNumber;

  // string that will be used to separate values
  char* ValueDelimiter;
  
  // Available table names
  vtkStringArray* AvailableTables;

private:
  vtkVisuTableReader(const vtkVisuTableReader&); // Not implemented.
  void operator=(const vtkVisuTableReader&); // Not implemented.
};

#endif //__vtkVisuTableReader_h_
