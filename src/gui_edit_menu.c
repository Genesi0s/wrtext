#include "gui_edit_menu.h"
#include "editor_file.h"
#include "gui_edit.h"
#include <gtk/gtk.h>
#include <stdlib.h>
#include <string.h>

GtkWidget *
gui_edit_menu_init(GtkApplication *app)
{

	GtkWidget *menubar = gtk_menu_bar_new();

	GtkWidget *file_menu = gtk_menu_new();
	GtkWidget *file_item = gtk_menu_item_new_with_label("File");
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(file_item), file_menu);

	GtkWidget *random_item = gtk_menu_item_new_with_label("Add random file");
	g_signal_connect(random_item, "activate", G_CALLBACK(gui_edit_add_random_file), app);
	gtk_menu_shell_append(GTK_MENU_SHELL(file_menu), random_item);

	gtk_menu_shell_append(GTK_MENU_SHELL(menubar), file_item);

	return menubar;
}