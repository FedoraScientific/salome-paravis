
#include "PVGUI_OutputWindowAdapter.h"

#include <vtkObjectFactory.h>

#include <LightApp_Application.h>
#include <LogWindow.h>
#include <SUIT_Session.h>

vtkStandardNewMacro(PVGUI_OutputWindowAdapter);
vtkCxxRevisionMacro(PVGUI_OutputWindowAdapter, "$Revision$");

PVGUI_OutputWindowAdapter::PVGUI_OutputWindowAdapter() :
  TextCount(0),
  ErrorCount(0),
  WarningCount(0),
  GenericWarningCount(0)
{
}

PVGUI_OutputWindowAdapter::~PVGUI_OutputWindowAdapter()
{
}

const unsigned int PVGUI_OutputWindowAdapter::getTextCount()
{
  return this->TextCount;
}

const unsigned int PVGUI_OutputWindowAdapter::getErrorCount()
{
  return this->ErrorCount;
}

const unsigned int PVGUI_OutputWindowAdapter::getWarningCount()
{
  return this->WarningCount;
}

const unsigned int PVGUI_OutputWindowAdapter::getGenericWarningCount()
{
  return this->GenericWarningCount;
}

static LogWindow* getLogWindow()
{
  LogWindow* wnd = 0;
  LightApp_Application* anApp = dynamic_cast<LightApp_Application*>( SUIT_Session::session()->activeApplication() );
  if ( anApp )
    wnd = anApp->logWindow();
  return wnd;
}

void PVGUI_OutputWindowAdapter::DisplayText(const char* text)
{
  ++this->TextCount;
  LogWindow* wnd = getLogWindow();
  if ( wnd )
    wnd->putMessage( text, Qt::darkGreen, LogWindow::DisplayNormal );
}

void PVGUI_OutputWindowAdapter::DisplayErrorText(const char* text)
{
  ++this->ErrorCount;
  LogWindow* wnd = getLogWindow();
  if ( wnd )
    wnd->putMessage( text, Qt::darkRed, LogWindow::DisplayNormal );
}

void PVGUI_OutputWindowAdapter::DisplayWarningText(const char* text)
{
  ++this->WarningCount;
  LogWindow* wnd = getLogWindow();
  if ( wnd )
    wnd->putMessage( text, Qt::black, LogWindow::DisplayNormal );
}

void PVGUI_OutputWindowAdapter::DisplayGenericWarningText(const char* text)
{
  ++this->GenericWarningCount;
  LogWindow* wnd = getLogWindow();
  if ( wnd )
    wnd->putMessage( text, Qt::black, LogWindow::DisplayNormal );
}
