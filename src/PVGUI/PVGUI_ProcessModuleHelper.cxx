// File:	PVGUI_ProcessModuleHelper.cxx
// Created:	Tue Nov 11 13:03:19 2008
// Author:	Sergey ANIKIN
//		<san@portrex.nnov.opencascade.com>


#include "PVGUI_ProcessModuleHelper.h"

//#include <pqApplicationCore.h>
#include <vtkObjectFactory.h>
//#include <vtkPVConfig.h>

vtkStandardNewMacro(PVGUI_ProcessModuleHelper);
vtkCxxRevisionMacro(PVGUI_ProcessModuleHelper, "$Revision$");

//-----------------------------------------------------------------------------
PVGUI_ProcessModuleHelper::PVGUI_ProcessModuleHelper()
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
bool PVGUI_ProcessModuleHelper::compareView(const QString& ReferenceImage,
  double Threshold, ostream& Output, const QString& TempDirectory)
{
  // TODO: to be implemented...
  //if(MainWindow* const main_window = qobject_cast<MainWindow*>(this->GetMainWindow()))
  //{
  //  return main_window->compareView(ReferenceImage, Threshold, Output, TempDirectory);
  //}
  
  return false;
}

//-----------------------------------------------------------------------------
void PVGUI_ProcessModuleHelper::showOutputWindow()
{
  // Do nothing here
}

//-----------------------------------------------------------------------------
void PVGUI_ProcessModuleHelper::showWindow()
{
}

//-----------------------------------------------------------------------------
void PVGUI_ProcessModuleHelper::hideWindow()
{
}

//-----------------------------------------------------------------------------
int PVGUI_ProcessModuleHelper::InitializeApplication(int argc, char** argv)
{
  if ( pqProcessModuleGUIHelper::InitializeApplication( argc, argv ) ){
    // TODO: Redirect VTK debug output to SALOME GUI message console ...
    /*this->Implementation->OutputWindow = new pqOutputWindow(0);
    this->Implementation->OutputWindow->setAttribute(Qt::WA_QuitOnClose, false);
    this->Implementation->OutputWindow->connect(this->Implementation->OutputWindowAdapter,
                                                SIGNAL(displayText(const QString&)), SLOT(onDisplayText(const QString&)));
    this->Implementation->OutputWindow->connect(this->Implementation->OutputWindowAdapter,
                                                SIGNAL(displayErrorText(const QString&)), SLOT(onDisplayErrorText(const QString&)));
    this->Implementation->OutputWindow->connect(this->Implementation->OutputWindowAdapter,
                                                SIGNAL(displayWarningText(const QString&)), SLOT(onDisplayWarningText(const QString&)));
    this->Implementation->OutputWindow->connect(this->Implementation->OutputWindowAdapter,
                                                SIGNAL(displayGenericWarningText(const QString&)), SLOT(onDisplayGenericWarningText(const QString&)));
    vtkOutputWindow::SetInstance(Implementation->OutputWindowAdapter);*/
  }

  return 1;
}

//-----------------------------------------------------------------------------
int PVGUI_ProcessModuleHelper::appExec()
{
  return 0;
}

//-----------------------------------------------------------------------------
int PVGUI_ProcessModuleHelper::postAppExec()
{
  return 0;
}

//-----------------------------------------------------------------------------
QWidget* PVGUI_ProcessModuleHelper::CreateMainWindow()
{
  return 0;
}

//-----------------------------------------------------------------------------
void PVGUI_ProcessModuleHelper::ExitApplication()
{
  // Cannot exit here, so do nothing
}
