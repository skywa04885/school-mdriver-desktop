#include "main.h"

static Driver mdriver0("/dev/ttyUSB1", B9600);

void activate(GtkApplication *app, gpointer udata)
{
  Logger logger("Main", LoggerLevel::DEBUG); 
  logger << "Application starting .." << ENDL;

  logger << "Creating connection to motor-driver 0" << ENDL;
  mdriver0.connect();
  logger << "Connection to motor-driver 0 created" << ENDL;

  new GUI::CPanel(app, &mdriver0);
}


int32_t main(int32_t argc, char **argv)
{
  GtkApplication *app = gtk_application_new("nl.fannst.mdriver_desktop", G_APPLICATION_FLAGS_NONE);
  g_signal_connect(app, "activate", G_CALLBACK(&activate), NULL);
  
  int32_t status = g_application_run(G_APPLICATION(app), argc, argv);
  g_object_unref(app);
  return status;
}