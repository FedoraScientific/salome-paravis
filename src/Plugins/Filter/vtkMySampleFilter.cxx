#include "vtkMySampleFilter.h"

#include <vtkObjectFactory.h>

vtkCxxRevisionMacro(vtkMySampleFilter, "$Revision$");
vtkStandardNewMacro(vtkMySampleFilter);

//----------------------------------------------------------------------------
vtkMySampleFilter::vtkMySampleFilter()
{
}

//----------------------------------------------------------------------------
vtkMySampleFilter::~vtkMySampleFilter()
{
}

//----------------------------------------------------------------------------
void vtkMySampleFilter::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}

