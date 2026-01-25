#include "gui_edit_menu.h"
#include "editor_file.h"
#include "file_dialog.h"
#include "fmanager.h"
#include "gui_about.h"
#include "gui_alert.h"
#include "gui_edit.h"
#include "gui_settings.h"
#include "logger.h"
#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief Callback for the "open" action: show open-file dialog.
 *
 * @param action Unused GSimpleAction pointer.
 * @param parameter Unused GVariant pointer.
 * @param user_data Pointer to the GtkApplication passed from the caller.
 */
static void
gui_edit_menu_on_open(GSimpleAction *action, GVariant *parameter, gpointer user_data)
{
	GtkApplication *app = GTK_APPLICATION(user_data);
	GtkWindow *parent = gtk_application_get_active_window(app);

	// Disables parent!
	gtk_widget_set_sensitive(GTK_WIDGET(parent), FALSE);

	char *path = file_dialog_open_file(parent);

	// Enables parent before adding the file so focus can be grabbed correctly.
	// -> correction: before we couldn't type on open/new cerated files
	gtk_widget_set_sensitive(GTK_WIDGET(parent), TRUE);

	if(path) {
		editor_file *ef = fmanager_load(path);
		if(ef) {
			gui_edit_add_file(ef);
			log_info(__FILE__, "Open selected: %s", ef->file_name);
		} else {
			gui_alert_error("Failed to load file %s", path);
			log_err(__FILE__, "Failed to load file: %s", path);
		}
		g_free(path);
	}
}

/**
 * @brief Callback for the "save" action: saves file to its current path.
 */
static void
gui_edit_menu_on_save(GSimpleAction *action, GVariant *parameter, gpointer user_data)
{

	GtkApplication *app = GTK_APPLICATION(user_data);
	GtkWindow *parent = gtk_application_get_active_window(app);

	// Disables parent!
	gtk_widget_set_sensitive(GTK_WIDGET(parent), FALSE);

	editor_file *ef = gui_edit_get_selected_file();

	// Update content
	editor_file_update_content(ef);

	// Save the current file to its current path
	int success = fmanager_save(ef);
	if(success == 0) {
		log_info(__FILE__, "Successfully saved file");
	} else {
		log_err(__FILE__, "Saving of file failed");
	}

	// Enables parent!
	gtk_widget_set_sensitive(GTK_WIDGET(parent), TRUE);
}

/**
 * @brief Callback for the "save_as" action: show save-file dialog and create file.
 *
 * If a path is chosen, an empty file is created (or truncated) so the application
 * can later write into it.
 */
static void
gui_edit_menu_on_save_as(GSimpleAction *action, GVariant *parameter, gpointer user_data)
{
	GtkApplication *app = GTK_APPLICATION(user_data);
	GtkWindow *parent = gtk_application_get_active_window(app);

	// Disables parent!
	gtk_widget_set_sensitive(GTK_WIDGET(parent), FALSE);

	char *path = file_dialog_save_file(parent, NULL);
	if(path) {
		FILE *f = fopen(path, "w");
		if(f)
			fclose(f);
		log_info(__FILE__, "Save selected: %s", path);

		// Implement file saving here
		editor_file *ef = gui_edit_get_selected_file();

		ef->file_path = strdup(path);

		// Update name
		g_free(ef->file_name);
		editor_file_update_name(ef);
		gtk_label_set_text(GTK_LABEL(ef->title_label), ef->file_name);

		// Update content
		editor_file_update_content(ef);
		
		// Save file
		int success = fmanager_save(ef);
		if(success == 0) {
			log_info(__FILE__, "Successfully saved file");
		} else {
			log_err(__FILE__, "Saving of file failed");
		}

		g_free(path);
	}
	// Enables parent!
	gtk_widget_set_sensitive(GTK_WIDGET(parent), TRUE);
}
/**
 * @brief Callback for the "new" action: show new-file dialog.
 *
 * @param action Unused GSimpleAction pointer.
 * @param parameter Unused GVariant pointer.
 * @param user_data Pointer to the GtkApplication passed from the caller.
 */
static void
gui_edit_menu_on_new(GSimpleAction *action, GVariant *parameter, gpointer user_data)
{
	GtkApplication *app = GTK_APPLICATION(user_data);
	GtkWindow *parent = gtk_application_get_active_window(app);

	/* Disable parent while native dialog is running. */
	gtk_widget_set_sensitive(GTK_WIDGET(parent), FALSE);

	char *path = file_dialog_create_file(parent, "untitled.txt");

	gtk_widget_set_sensitive(GTK_WIDGET(parent), TRUE);
	if(path) {
		/* Create or truncate the file before loading into the editor. */
		FILE *f = fopen(path, "w");
		if(f)
			fclose(f);

		editor_file *ef = fmanager_load(path);
		if(ef) {
			gui_edit_add_file(ef);
			log_info(__FILE__, "Create selected: %s", ef->file_name);
		}

		g_free(path);
	}
}

GtkWidget *
gui_edit_menu_init(GtkApplication *app)
{

	// Create action
	GSimpleAction *action_randfile = g_simple_action_new("randfile", NULL);
	GSimpleAction *action_new = g_simple_action_new("new", NULL);
	GSimpleAction *action_open = g_simple_action_new("open", NULL);
	GSimpleAction *action_save_as = g_simple_action_new("save_as", NULL);
	GSimpleAction *action_save = g_simple_action_new("save", NULL);
	GSimpleAction *action_about = g_simple_action_new("about", NULL);
	GSimpleAction *action_settings = g_simple_action_new("settings", NULL);
	// Link action to function
	g_action_map_add_action(G_ACTION_MAP(app), G_ACTION(action_randfile));
	g_action_map_add_action(G_ACTION_MAP(app), G_ACTION(action_new));
	g_action_map_add_action(G_ACTION_MAP(app), G_ACTION(action_open));
	g_action_map_add_action(G_ACTION_MAP(app), G_ACTION(action_save_as));
	g_action_map_add_action(G_ACTION_MAP(app), G_ACTION(action_save));
	g_action_map_add_action(G_ACTION_MAP(app), G_ACTION(action_about));
	g_action_map_add_action(G_ACTION_MAP(app), G_ACTION(action_settings));

	g_signal_connect(action_randfile, "activate", G_CALLBACK(gui_edit_add_random_file), NULL);
	g_signal_connect(action_new, "activate", G_CALLBACK(gui_edit_menu_on_new), app);

	// Open/save actions. Pass the GtkApplication as user_data so callbacks
	// can obtain the active window with gtk_application_get_active_window(). */
	g_signal_connect(action_open, "activate", G_CALLBACK(gui_edit_menu_on_open), app);
	g_signal_connect(action_save_as, "activate", G_CALLBACK(gui_edit_menu_on_save_as), app);
	g_signal_connect(action_save, "activate", G_CALLBACK(gui_edit_menu_on_save), app);
	// Calls the function that activates the about window and passes it the main window
	g_signal_connect(action_about, "activate", G_CALLBACK(gui_about_init),
					 gtk_application_get_active_window(app));
	g_signal_connect(action_settings, "activate", G_CALLBACK(gui_settings_init),
					 gtk_application_get_active_window(app));

	// Create the two menus
	GMenu *menu_model = g_menu_new();	   // main
	GMenu *menu_file_model = g_menu_new(); // file -> rand
	GMenu *menu_help_model = g_menu_new(); // help -> about
	GMenu *menu_edit_model = g_menu_new(); // edit -> settings

	GMenuItem *menu_help_menu = g_menu_item_new("Help", NULL);
	GMenuItem *menu_file_menu = g_menu_item_new("File", NULL);
	GMenuItem *menu_edit_menu = g_menu_item_new("Edit", NULL);

	// Items
	GMenuItem *item_filerand
		= g_menu_item_new("FileRand", "app.randfile"); // Link option to action
	GMenuItem *item_new = g_menu_item_new("New...", "app.new");
	GMenuItem *item_open = g_menu_item_new("Open...", "app.open");
	GMenuItem *item_save_as = g_menu_item_new("Save As...", "app.save_as");
	GMenuItem *item_save = g_menu_item_new("Save...", "app.save");
	GMenuItem *item_about = g_menu_item_new("About", "app.about");
	GMenuItem *item_settings = g_menu_item_new("Settings", "app.settings");

	// Add File submenu and randfile item
	g_menu_append_item(menu_file_model, item_new);
	g_menu_append_item(menu_file_model, item_open);
	g_menu_append_item(menu_file_model, item_save_as);
	g_menu_append_item(menu_file_model, item_save);
	g_menu_append_item(menu_file_model, item_filerand);
	g_menu_item_set_submenu(menu_file_menu, G_MENU_MODEL(menu_file_model));
	g_menu_append_item(menu_model, menu_file_menu);

	// Add Edit submenu and Settings item
	g_menu_append_item(menu_edit_model, item_settings);
	g_menu_item_set_submenu(menu_edit_menu, G_MENU_MODEL(menu_edit_model));
	g_menu_append_item(menu_model, menu_edit_menu);

	// Add Help submenu and About item
	g_menu_append_item(menu_help_model, item_about);
	g_menu_item_set_submenu(menu_help_menu, G_MENU_MODEL(menu_help_model));
	g_menu_append_item(menu_model, menu_help_menu);

	// Set menubar
	gtk_application_set_menubar(GTK_APPLICATION(app), G_MENU_MODEL(menu_model));

	return NULL;
}
