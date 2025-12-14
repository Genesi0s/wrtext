
#include "gui_edit.h"
#include "editor_file.h"
#include "gui_edit_menu.h"
#include "logger.h"
#include <gtk/gtk.h>
#include <stdlib.h>
#include <string.h>

/*!
	@brief Structure defining a list of files and mapping each one to a page number
*/
typedef struct {
	editor_file **files;
	editor_file_id *ids;
	unsigned long length;
	GtkWidget **page_widget;
} editor_file_list;

/*!
	@brief Container of all the files currently opened in the editor
*/
editor_file_list file_list;

/*!
	@brief GTK widget that holds all of the opened files
*/
GtkWidget *notebook;

void
log_file_list()
{
	log_info(__FILE__, "FILE LIST");
	log_info(__FILE__, "Size: %lu", file_list.length);

	for(int i = 0; i < file_list.length; i++) {
		log_info(__FILE__, "\tfile index: %d", i);
		log_info(__FILE__, "\tfile: %p", file_list.files[i]);
		log_info(__FILE__, "\tid: %lu", file_list.ids[i]);
		log_info(__FILE__, "\t");
	}
}

GtkWidget *
gui_edit_init(GtkApplication *app)
{

	// Initialize opened files structure
	file_list.files = NULL;
	file_list.ids = NULL;
	file_list.page_widget = NULL;
	file_list.length = 0;

	// Initialize window and box
	GtkWidget *window;
	window = gtk_application_window_new(app);
	gtk_window_set_title(GTK_WINDOW(window), VER);
	gtk_window_set_default_size(GTK_WINDOW(window), 400, 400);

	g_signal_connect(window, "destroy", G_CALLBACK(gui_edit_cleanup), NULL);

	GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	gtk_container_add(GTK_CONTAINER(window), vbox);

	// Initialize menubar
	GtkWidget *menubar = gui_edit_menu_init(app);

	gtk_box_pack_start(GTK_BOX(vbox), menubar, FALSE, FALSE, 0);

	notebook = gtk_notebook_new();

	// Create a group, needed for reordering
	gtk_notebook_set_group_name(GTK_NOTEBOOK(notebook), "notebookgroup");

	gtk_box_pack_start(GTK_BOX(vbox), notebook, TRUE, TRUE, 0);

	// Shows window
	gtk_widget_show_all(window);

	return window;
}

int
gui_edit_close_file(editor_file_id id)
{

	// Finds file from editor file id
	int file_index = -1;
	for(int i = 0; i < file_list.length; i++) {
		if(file_list.ids[i] == id) {
			file_index = i;
			break;
		}
	}

	if(file_index == -1) {
		log_err(__FILE__, "ID %lu not found in file list", id);
		return -1;
	}
	log_err(__FILE__, "ID %lu is at position %d", id, file_index);

	// Removes the page associated to that file
	gtk_notebook_remove_page(
		(GtkNotebook *)notebook,
		gtk_notebook_page_num((GtkNotebook *)notebook, file_list.page_widget[file_index]));

	// Free file memory

	editor_file_delete(file_list.files[file_index]);

	if(file_list.length == 1) {
		free(file_list.files);
		free(file_list.page_widget);
		free(file_list.ids);
	} else {

		// Removes file from file list, this is done by shifting all files that come after by one.
		// It might seem slow but it's actually fast with a worst-case time of O(n)
		// Plus it allows for insertion with a time of O(1)

		// Shifts
		for(int i = file_index; i < file_list.length - 1; i++) {
			file_list.files[i] = file_list.files[i + 1];
			file_list.page_widget[i] = file_list.page_widget[i + 1];
			file_list.ids[i] = file_list.ids[i + 1];
		}

		// Frees memory
		file_list.files = realloc(file_list.files, sizeof(editor_file *) * (file_list.length - 1));
		file_list.page_widget
			= realloc(file_list.page_widget, sizeof(GtkWidget *) * (file_list.length - 1));
		file_list.ids = realloc(file_list.ids, sizeof(editor_file_id) * (file_list.length - 1));
	}
	file_list.length--;

	log_info(__FILE__, "closing file \"%lu\"", id);
	log_file_list();

	return 0;
}

void
on_click_file_close(GtkButton *button, gpointer data)
{
	if(gui_edit_close_file(*(editor_file_id *)data) == -1) {
		// error
	}

	// This pointer was allocated at the time of opening the file
	free(data);
}

editor_file_id
gui_edit_add_file(editor_file *f)
{

	static editor_file_id last_assigned = 0;

	// If no file was open, allocate new memory for the array
	if(file_list.length == 0) {
		file_list.files = malloc(sizeof(editor_file *));
		file_list.page_widget = malloc(sizeof(GtkWidget *));
		file_list.ids = malloc(sizeof(editor_file_id));
	} else {
		file_list.files = realloc(file_list.files, sizeof(editor_file *) * (file_list.length + 1));
		file_list.page_widget
			= realloc(file_list.page_widget, sizeof(GtkWidget *) * (file_list.length + 1));
		file_list.ids = realloc(file_list.ids, sizeof(editor_file_id) * (file_list.length + 1));
	}

	file_list.files[file_list.length] = f; // Copy pointer to file added

	// Create new notebook page

	// Create text area with file contents
	GtkWidget *text_area = gtk_text_view_new();

	// Create widgets of the page title
	GtkWidget *title_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 2);
	GtkWidget *title_name = gtk_label_new(f->file_name);
	GtkWidget *title_close = gtk_button_new_from_icon_name("window-close", GTK_ICON_SIZE_MENU);

	// Put them all in one box
	gtk_box_pack_start(GTK_BOX(title_box), title_name, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(title_box), title_close, FALSE, FALSE, 0);

	// Create page with the widgets
	gtk_notebook_append_page(GTK_NOTEBOOK(notebook), text_area, title_box);
	file_list.page_widget[file_list.length] = text_area;

	// Makes page reordable
	gtk_notebook_set_tab_reorderable(GTK_NOTEBOOK(notebook), text_area, TRUE);

	// Connect function to the button that closes the file
	editor_file_id *fid = malloc(sizeof(editor_file_id));

	// Assigns new file id
	last_assigned++;
	*fid = last_assigned;
	file_list.ids[file_list.length] = *fid;

	g_signal_connect(title_close, "clicked", G_CALLBACK(on_click_file_close), fid);

	// Show new widgets
	gtk_widget_show(title_box);
	gtk_widget_show(title_name);
	gtk_widget_show(title_close);
	gtk_widget_show(text_area);

	file_list.length++; // Increment size counter of the list

	log_info(__FILE__, "added file \"%lu\"", *fid);
	log_file_list();

	return *fid;
}

void
gui_edit_cleanup()
{
	if(file_list.files != NULL)
		free(file_list.files);
	if(file_list.page_widget != NULL)
		free(file_list.page_widget);
}

void
gui_edit_add_random_file()
{
	static int called = 0;
	// TEST
	editor_file *fa = malloc(sizeof(editor_file));
	fa->file_name = malloc(30);
	strcpy(fa->file_name, "test0.txt");
	fa->file_name[4] = '0' + (char)called;

	fa->contents = malloc(3);
	fa->size = 3;

	fa->contents[0] = '0' + (char)called;
	fa->contents[1] = '0' + (char)called;
	fa->contents[2] = '0' + (char)called;

	gui_edit_add_file(fa);

	called++;
}