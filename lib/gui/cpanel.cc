#include "cpanel.h"

namespace GUI
{
  void cpanel_change_motor_state(CPanel *panel, command_motor_t motor, bool enabled)
  {
    auto *cmd = reinterpret_cast<command_packet_t *>(malloc(sizeof(command_packet_t) + sizeof (uint8_t) + sizeof (uint32_t)));

    cmd->hdr.type = enabled ? COMMAND_TYPE_ENABLE : COMMAND_TYPE_DISABLE;
    cmd->body.size = 1;
    cmd->body.payload[0] = motor;
    panel->m_Axis0->write_command(cmd);

    free(cmd);
  }

  void cpanel_home_motor(CPanel *panel, command_motor_t motor)
  {
    auto *cmd = reinterpret_cast<command_packet_t *>(malloc(sizeof(command_packet_t) + sizeof (uint8_t) + sizeof (uint32_t)));

    cmd->hdr.type = COMMAND_TYPE_EAHOME;
    cmd->body.size = 1;
    cmd->body.payload[0] = motor;
    panel->m_Axis0->write_command(cmd);

    free(cmd);
  }

  void cpanel_move_motor(CPanel *panel, command_motor_t motor, uint32_t pos)
  {
    auto *cmd = reinterpret_cast<command_packet_t *>(malloc(sizeof(command_packet_t) + sizeof (uint8_t) + sizeof (uint32_t)));

    cmd->hdr.type = COMMAND_TYPE_MOVE;
    cmd->body.size = sizeof (uint32_t) + sizeof (uint8_t);
    cmd->body.payload[0] = motor;
    cmd->body.payload[1];
    panel->m_Axis0->write_command(cmd);

    free(cmd);
  }

  void cpanel_disable_mr(GtkButton *button, gpointer udata)
  {
    CPanel *panel = reinterpret_cast<CPanel *>(udata);
    panel->m_Logger << "Disabling MR" << ENDL;
    cpanel_change_motor_state(panel, COMMAND_MOTOR_MR, false);
  }

  void cpanel_disable_mx(GtkButton *button, gpointer udata)
  {
    CPanel *panel = reinterpret_cast<CPanel *>(udata);
    panel->m_Logger << "Disabling MX" << ENDL;
    cpanel_change_motor_state(panel, COMMAND_MOTOR_MX, false);
  }

  void cpanel_enable_mr(GtkButton *button, gpointer udata)
  {
    CPanel *panel = reinterpret_cast<CPanel *>(udata);
    panel->m_Logger << "Enabling MR" << ENDL;
    cpanel_change_motor_state(panel, COMMAND_MOTOR_MR, true);
  }

  void cpanel_enable_mx(GtkButton *button, gpointer udata)
  {
    CPanel *panel = reinterpret_cast<CPanel *>(udata);
    panel->m_Logger << "Enabling MX" << ENDL;
    cpanel_change_motor_state(panel, COMMAND_MOTOR_MX, true);
  }

  void cpanel_home_mx(GtkButton *button, gpointer udata)
  {
    CPanel *panel = reinterpret_cast<CPanel *>(udata);
    panel->m_Logger << "Auto-Homing MX" << ENDL;
    cpanel_home_motor(panel, COMMAND_MOTOR_MX);
  }

  void cpanel_home_mr(GtkButton *button, gpointer udata)
  {
    CPanel *panel = reinterpret_cast<CPanel *>(udata);
    panel->m_Logger << "Auto-Homing MR" << ENDL;
    cpanel_home_motor(panel, COMMAND_MOTOR_MR);
  }

  void cpanel_move_mr(GtkButton *button, gpointer udata)
  {
    CPanel *panel = reinterpret_cast<CPanel *>(udata);

    const char *text = gtk_entry_get_text(GTK_ENTRY(panel->m_Entry_PosMR));
    try {
      uint32_t pos = std::stoi(text);
      panel->m_Logger << "Moving MR to position: " << pos << ENDL;
      cpanel_move_motor(panel, COMMAND_MOTOR_MR, pos);
    } catch (...)
    {
      panel->m_Logger << "std::stoi(text) failed: no number" << ENDL;
      return;
    }
  }

  void cpanel_move_mx(GtkButton *button, gpointer udata)
  {
    CPanel *panel = reinterpret_cast<CPanel *>(udata);

    const char *text = gtk_entry_get_text(GTK_ENTRY(panel->m_Entry_PosMX));
    try {
      uint32_t pos = std::stoi(text);
      panel->m_Logger << "Moving MX to position: " << pos << ENDL;
      cpanel_move_motor(panel, COMMAND_MOTOR_MX, pos);
    } catch (...)
    {
      panel->m_Logger << "std::stoi(text) failed: no number" << ENDL;
      return;
    }
  }

  CPanel::CPanel(GtkApplication *app, Driver *a0):
    m_Logger("CPanel", LoggerLevel::DEBUG), m_Axis0(a0)
  {
    // Creates the application
    this->m_Window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(this->m_Window), "MDriver Desktop");
    gtk_window_set_default_size(GTK_WINDOW(this->m_Window), 700, 300);

    // Creates the box
    this->m_RootBox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);

    // Creates MX stuff
    this->m_Entry_PosMX = gtk_entry_new();
    gtk_entry_set_max_length(GTK_ENTRY(this->m_Entry_PosMX), 10);
    gtk_entry_set_text(GTK_ENTRY(this->m_Entry_PosMX), "0");
    gtk_entry_set_input_purpose(GTK_ENTRY(this->m_Entry_PosMX), GTK_INPUT_PURPOSE_NUMBER);

    this->m_Box_ButtonsMX = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    this->m_LabelMX = gtk_label_new("Axis POS");
    this->m_Button_DisableMX = gtk_button_new_with_label("Disable");
    this->m_Button_EnableMX = gtk_button_new_with_label("Enable");
    this->m_Button_MoveMX = gtk_button_new_with_label("Move");
    this->m_Button_HomeMX = gtk_button_new_with_label("AutoHome");

    g_signal_connect(this->m_Button_DisableMX, "clicked", G_CALLBACK(&cpanel_disable_mx), this);
    g_signal_connect(this->m_Button_EnableMX, "clicked", G_CALLBACK(&cpanel_enable_mx), this);
    g_signal_connect(this->m_Button_HomeMX, "clicked", G_CALLBACK(&cpanel_home_mx), this);
    g_signal_connect(this->m_Button_MoveMX, "clicked", G_CALLBACK(&cpanel_move_mx), this);

    gtk_box_pack_start(GTK_BOX(this->m_Box_ButtonsMX), this->m_LabelMX, false, false, 0);
    gtk_box_pack_start(GTK_BOX(this->m_Box_ButtonsMX), this->m_Button_DisableMX, false, false, 0);
    gtk_box_pack_start(GTK_BOX(this->m_Box_ButtonsMX), this->m_Button_EnableMX, false, false, 0);
    gtk_box_pack_start(GTK_BOX(this->m_Box_ButtonsMX), this->m_Button_HomeMX, false, false, 0);
    gtk_box_pack_start(GTK_BOX(this->m_Box_ButtonsMX), this->m_Entry_PosMX, false, false, 0);
    gtk_box_pack_start(GTK_BOX(this->m_Box_ButtonsMX), this->m_Button_MoveMX, false, false, 0);
    gtk_box_pack_start(GTK_BOX(this->m_RootBox), this->m_Box_ButtonsMX, false, false, 2);

    // Creates MR stuff
    this->m_Entry_PosMR = gtk_entry_new();
    gtk_entry_set_max_length(GTK_ENTRY(this->m_Entry_PosMR), 10);
    gtk_entry_set_text(GTK_ENTRY(this->m_Entry_PosMR), "0");
    gtk_entry_set_input_purpose(GTK_ENTRY(this->m_Entry_PosMR), GTK_INPUT_PURPOSE_NUMBER);

    this->m_Box_ButtonsMR = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    this->m_LabelMR = gtk_label_new("Axis ROT");
    this->m_Button_DisableMR = gtk_button_new_with_label("Disable");
    this->m_Button_EnableMR = gtk_button_new_with_label("Enable");
    this->m_Button_MoveMR = gtk_button_new_with_label("Move");
    this->m_Button_HomeMR = gtk_button_new_with_label("AutoHome");

    g_signal_connect(this->m_Button_DisableMR, "clicked", G_CALLBACK(&cpanel_disable_mr), this);
    g_signal_connect(this->m_Button_EnableMR, "clicked", G_CALLBACK(&cpanel_enable_mr), this);
    g_signal_connect(this->m_Button_HomeMR, "clicked", G_CALLBACK(&cpanel_home_mr), this);
    g_signal_connect(this->m_Button_MoveMR, "clicked", G_CALLBACK(&cpanel_move_mr), this);

    gtk_box_pack_start(GTK_BOX(this->m_Box_ButtonsMR), this->m_LabelMR, false, false, 0);
    gtk_box_pack_start(GTK_BOX(this->m_Box_ButtonsMR), this->m_Button_DisableMR, false, false, 0);
    gtk_box_pack_start(GTK_BOX(this->m_Box_ButtonsMR), this->m_Button_EnableMR, false, false, 0);
    gtk_box_pack_start(GTK_BOX(this->m_Box_ButtonsMR), this->m_Button_HomeMR, false, false, 0);
    gtk_box_pack_start(GTK_BOX(this->m_Box_ButtonsMR), this->m_Entry_PosMR, false, false, 0);
    gtk_box_pack_start(GTK_BOX(this->m_Box_ButtonsMR), this->m_Button_MoveMR, false, false, 0);
    gtk_box_pack_start(GTK_BOX(this->m_RootBox), this->m_Box_ButtonsMR, false, false, 2);


    // Shows everything
    gtk_container_add(GTK_CONTAINER(this->m_Window), this->m_RootBox);
    gtk_widget_show_all(this->m_Window);
  }

  CPanel::~CPanel()
  {}
}
