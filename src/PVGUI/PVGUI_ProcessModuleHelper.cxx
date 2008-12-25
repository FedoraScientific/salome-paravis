// File:	PVGUI_ProcessModuleHelper.cxx
// Created:	Tue Nov 11 13:03:19 2008
// Author:	Sergey ANIKIN
//		<san@portrex.nnov.opencascade.com>


#include "PVGUI_Module.h"
#include "PVGUI_ProcessModuleHelper.h"
#include "PVGUI_OutputWindowAdapter.h"

#include <CAM_Application.h>
#include <CAM_Module.h>
#include <SUIT_Session.h>

#include <vtkObjectFactory.h>
#include <vtkOutputWindow.h>
#include <vtkSmartPointer.h>

vtkStandardNewMacro(PVGUI_ProcessModuleHelper);
vtkCxxRevisionMacro(PVGUI_ProcessModuleHelper, "$Revision$");

class PVGUI_ProcessModuleHelper::pqImplementation
{
public:
  pqImplementation() :
    OutputWindowAdapter(vtkSmartPointer<PVGUI_OutputWindowAdapter>::New())
  {}

  ~pqImplementation()
  {}

  //! Displays VTK debug output in SALOME log window
  vtkSmartPointer<PVGUI_OutputWindowAdapter> OutputWindowAdapter;
};

//-----------------------------------------------------------------------------
PVGUI_ProcessModuleHelper::PVGUI_ProcessModuleHelper()
  : Implementation(new pqImplementation())
{
}

//-----------------------------------------------------------------------------
PVGUI_ProcessModuleHelper::~PVGUI_ProcessModuleHelper()
{
}

//-----------------------------------------------------------------------------
void PVGUI_ProcessModuleHelper::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}

//-----------------------------------------------------------------------------
/*!
  \brief Used by ParaView testing capabilities, not usable in SALOME.
 */
bool PVGUI_ProcessModuleHelper::compareView(const QString& ReferenceImage,
  double Threshold, ostream& Output, const QString& TempDirectory)
{
  if ( CAM_Application* anApp = dynamic_cast<CAM_Application*>( SUIT_Session::session()->activeApplication() ) )
    if ( PVGUI_Module* aPVM = dynamic_cast<PVGUI_Module*>( anApp->module("ParaVis") ) )
      return aPVM->compareView( ReferenceImage, Threshold, Output, TempDirectory );
  
  return false;
}

//-----------------------------------------------------------------------------
/*! 
  \brief Reimplemented to suppress default ParaView client behavior. Does nothing.
*/
void PVGUI_ProcessModuleHelper::showOutputWindow()
{
  // Do nothing here
}

//-----------------------------------------------------------------------------
/*! 
  \brief Reimplemented to suppress default ParaView client behavior. Does nothing.
*/
void PVGUI_ProcessModuleHelper::showWindow()
{
}

//-----------------------------------------------------------------------------
/*! 
  \brief Reimplemented to suppress default ParaView client behavior. Does nothing.
*/
void PVGUI_ProcessModuleHelper::hideWindow()
{
}

//-----------------------------------------------------------------------------
/*! 
  \brief Finalize application in order to save server settings.
*/
void PVGUI_ProcessModuleHelper::finalize()
{
  this->FinalizeApplication();
}

//-----------------------------------------------------------------------------
/*! 
  \brief Calls the base implementation and redirects ParaView output to PVGUI_OutputWindowAdapter instance.
  \sa PVGUI_OutputWindowAdapter
*/
int PVGUI_ProcessModuleHelper::InitializeApplication(int argc, char** argv)
{
  if ( pqProcessModuleGUIHelper::InitializeApplication( argc, argv ) ){
    // Redirect VTK debug output to SALOME GUI message console 
    vtkOutputWindow::SetInstance(Implementation->OutputWindowAdapter);
  }

  return 1;
}

//-----------------------------------------------------------------------------
/*! 
  \brief Reimplemented to suppress default ParaView client behavior and not to run Qt event loop. Does nothing.
*/
int PVGUI_ProcessModuleHelper::appExec()
{
  return 0;
}

/*! 
  \brief Reimplemented to suppress default ParaView client behavior and not to exit after the client initalization. Does nothing.
*/
//-----------------------------------------------------------------------------
int PVGUI_ProcessModuleHelper::postAppExec()
{
  return 0;
}

/*! 
  \brief Reimplemented to suppress default ParaView client behavior. Does nothing.
*/
//-----------------------------------------------------------------------------
QWidget* PVGUI_ProcessModuleHelper::CreateMainWindow()
{
  return 0;
}

//-----------------------------------------------------------------------------
/*! 
  \brief Reimplemented to suppress default ParaView client behavior. Does nothing.
*/
void PVGUI_ProcessModuleHelper::ExitApplication()
{
  // Cannot exit here, so do nothing
}
