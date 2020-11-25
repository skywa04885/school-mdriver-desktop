#include "main.h"


void activate(GtkApplication *app, gpointer udata)
{
  Logger logger("Main", LoggerLevel::DEBUG); 
  logger << "Application starting .." << ENDL;

  char axis0[64];
  const char *axis0_path = nullptr;
  if (!getenv("AXIS0")) {
    axis0_path = "/dev/ttyUSB0";
    logger << "No AXIS0 env variable specified, defaulting to: " << axis0_path << ENDL;
  } else {
    snprintf(axis0, sizeof (axis0), getenv("AXIS0"));
    axis0_path = axis0;
  }

  Driver *mdriver0 = new Driver (axis0_path, B9600);

  logger << "Creating connection to motor-driver 0, device: " << axis0_path << ENDL;
  mdriver0->connect();
  logger << "Connection to motor-driver 0 created" << ENDL;

  new GUI::CPanel(app, mdriver0);
}


int32_t main(int32_t argc, char **argv)
{
  GtkApplication *app = gtk_application_new("nl.fannst.mdriver_desktop", G_APPLICATION_FLAGS_NONE);
  g_signal_connect(app, "activate", G_CALLBACK(&activate), NULL);
  
  int32_t status = g_application_run(G_APPLICATION(app), argc, argv);
  g_object_unref(app);
  return status;
}
