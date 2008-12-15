#ifndef __vtkMySampleFilter_h
#define __vtkMySampleFilter_h

#include <vtkExtractEdges.h>

class vtkMySampleFilter : public vtkExtractEdges
{
public:
  static vtkMySampleFilter* New();
  vtkTypeRevisionMacro(vtkMySampleFilter, vtkExtractEdges);
  void PrintSelf(ostream& os, vtkIndent indent);

protected:
  vtkMySampleFilter();
  ~vtkMySampleFilter();

private:
  vtkMySampleFilter(const vtkMySampleFilter&);  // Not implemented.
  void operator=(const vtkMySampleFilter&);  // Not implemented.
};

#endif
