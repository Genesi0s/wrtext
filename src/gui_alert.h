/*!
	@file gui_alert.h
	@brief gui_alert.h and gui_alert.c contain the code for triggering error messages to show the user
*/
#pragma once
#include <stdarg.h>
#include <gtk/gtk.h>

void gui_alert_init(GtkWidget *parent_window);
void gui_alert_error(const char *format,...);