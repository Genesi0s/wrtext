#include "gui_edit_menu.h"
#include "editor_file.h"
#include "gui_edit.h"
#include <gtk/gtk.h>
#include <stdlib.h>
#include <string.h>

GtkWidget *
gui_edit_menu_init(GtkApplication *app)
{

	// Create action
	GSimpleAction *action_randfile = g_simple_action_new("randfile", NULL);
	// Link action to function
	g_action_map_add_action(G_ACTION_MAP(app), G_ACTION(action_randfile));
	g_signal_connect(action_randfile, "activate", G_CALLBACK(gui_edit_add_random_file), NULL);

	// Create the two menus
	GMenu *menu_model = g_menu_new();
	GMenu *menu_file_model = g_menu_new();

	GMenuItem *menu_file_menu = g_menu_item_new("File", NULL);
	GMenuItem *item_filerand
		= g_menu_item_new("FileRand", "app.randfile"); // Link option to action

	g_menu_append_item(menu_file_model, item_filerand);

	// Make one menu the submenu of the other
	g_menu_item_set_submenu(menu_file_menu, G_MENU_MODEL(menu_file_model));

	g_menu_append_item(menu_model, menu_file_menu);

	// Set menubar
	gtk_application_set_menubar(GTK_APPLICATION(app), G_MENU_MODEL(menu_model));

	return NULL;
}