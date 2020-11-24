#ifndef _LIB_GUI_CPANEL_H
#define _LIB_GUI_CPANEL_H

#include "../default.h"
#include "../driver.h"
#include "../Logger.h"

namespace GUI
{
  class CPanel
  {
  public:
    CPanel(GtkApplication *app, Driver *a0);

    ~CPanel();

    GtkWidget *m_Window;
    GtkWidget *m_RootBox, *m_Box_ButtonsMX, *m_Box_ButtonsMR;
    GtkWidget *m_Button_DisableMX, *m_Button_EnableMX, *m_Button_MoveMX, *m_Button_HomeMX;
    GtkWidget *m_Button_DisableMR, *m_Button_EnableMR, *m_Button_MoveMR, *m_Button_HomeMR;
    GtkWidget *m_LabelMX, *m_LabelMR;
    GtkWidget *m_Entry_PosMX, *m_Entry_PosMR;
    
    Logger m_Logger;
    Driver *m_Axis0;
  };
}

#endif
