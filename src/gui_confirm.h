/*!
	@file gui_confirm.h
	@brief gui_confirm.h and gui_confirm.c contain the code for showing a confirm dialog when the user tries to close a file with unsaved changes or the application.
*/
#pragma once
#include "editor_file.h"
#include <stdarg.h>
#include <gtk/gtk.h>

void gui_confirm_init(GtkWidget *parent_window);

/*!
	@brief This function shows a dialog to the user asking whether to close without changes or cancel the close operation.
	The function itself handles the closing, it can be used when closing a single file or the whole application
	@param ef The ID of the closed file. The application will close this file if the user accepts. If passed -1 it will close the whole application instead
*/
void gui_confirm_show(editor_file_id ef);