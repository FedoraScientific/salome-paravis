// Copyright (C) 2014  CEA/DEN, EDF R&D
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

#ifndef __ElliplseBuilderFilter_h
#define __ElliplseBuilderFilter_h

#include <vtkMultiBlockDataSetAlgorithm.h>
#include <list>

class vtkDataObjectTreeIterator;
class vtkStringArray;

/**
 * Description of class: TODO
*/
class vtkEllipseBuilderFilter : public vtkMultiBlockDataSetAlgorithm
{
public:
	/// Returns pointer on a new instance of the class
	static vtkEllipseBuilderFilter* New();

	vtkTypeMacro(vtkEllipseBuilderFilter, vtkMultiBlockDataSetAlgorithm);

	/// Prints current state of the objects
	virtual void PrintSelf(ostream& os, vtkIndent indent);

	/// Set and Get methods for scale factor
	vtkSetMacro(ScaleFactor, double);
	vtkGetMacro(ScaleFactor, double);
	
        virtual vtkStringArray* GetFieldList();

        /// Set and Get methods for field of real part
	vtkSetStringMacro(RealField);
	vtkGetStringMacro(RealField);
        
        /// Set and Get methods for field of imag part
	vtkSetStringMacro(ImagField);
	vtkGetStringMacro(ImagField);

        /// Set and Get methods for start angle
	vtkSetMacro(StartAngle, double);
	vtkGetMacro(StartAngle, double);

        /// Set and Get methods for end angle
	vtkSetMacro(EndAngle, double);
	vtkGetMacro(EndAngle, double);

        /// Set and Get methods for resolution
	vtkSetMacro(Resolution, int);
	vtkGetMacro(Resolution, int);
        
        /// Set and Get methods for axis
	vtkSetMacro(Axis, int);
	vtkGetMacro(Axis, int);

protected:
	/// Constructor
	vtkEllipseBuilderFilter();

	/// Destructor
	virtual ~vtkEllipseBuilderFilter();
        
	/// A method which is called on filtering data
	virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *);
	virtual int RequestInformation(vtkInformation *, vtkInformationVector **, vtkInformationVector *);
        	
        vtkStringArray* FieldList;
        
	double ScaleFactor;
	double StartAngle;
	double EndAngle;
	int Resolution;
        int Axis;
	char* RealField;
	char* ImagField;

private:
	/// Methods for copy of the filter: Not implemented
	vtkEllipseBuilderFilter(const vtkEllipseBuilderFilter&); // Not implemented
	void operator=(const vtkEllipseBuilderFilter&); // Not implemented
};

#endif
