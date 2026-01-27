#include "editor_file.h"
#include "gui_alert.h"
#include "gui_edit.h"
#include "logger.h"
#include <gtk/gtk.h>
#include <stdio.h>
#include <string.h>

GtkWidget *parent2 = NULL;

typedef struct {
	editor_file_id ef;
	GtkWidget *confirm_window;
} confirm_params;

void
gui_confirm_close(confirm_params *p)
{

	gtk_window_close(GTK_WINDOW(p->confirm_window));

	if(p->ef != -1) {
		gui_edit_close_file(p->ef); // closes file
	} else {
		// Closes application
		gtk_window_destroy(GTK_WINDOW(parent2));
	}

	free(p);
}

void
gui_confirm_init(GtkWidget *parent_window)
{
	parent2 = parent_window;
}

void
gui_confirm_show(editor_file_id ef)
{

	if(parent2 == NULL) {
		log_err(__FILE__, "No parent found for the confirm window");
		return;
	}
	// Creates error window
	GtkWidget *confirm_dialog = gtk_window_new();
	gtk_window_set_title(GTK_WINDOW(confirm_dialog), "Warning");
	gtk_window_set_transient_for(GTK_WINDOW(confirm_dialog), GTK_WINDOW(parent2));
	gtk_window_set_modal(GTK_WINDOW(confirm_dialog), TRUE);
	gtk_window_set_resizable(GTK_WINDOW(confirm_dialog), FALSE);

	GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 12);
	gtk_widget_set_margin_start(box, 12);
	gtk_widget_set_margin_end(box, 12);
	gtk_widget_set_margin_top(box, 12);
	gtk_widget_set_margin_bottom(box, 12);
	gtk_window_set_child(GTK_WINDOW(confirm_dialog), box);

	GtkWidget *hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
	gtk_box_append(GTK_BOX(box), hbox);
	GtkWidget *image = gtk_image_new_from_icon_name("dialog-warning");
	gtk_image_set_icon_size(GTK_IMAGE(image), GTK_ICON_SIZE_LARGE);
	gtk_box_append(GTK_BOX(hbox), image);

	GtkWidget *label = gtk_label_new("Are you sure you want to close with unsaved changes?");
	gtk_box_append(GTK_BOX(hbox), label);

	GtkWidget *button_close = gtk_button_new_with_label("Close without saving");
	gtk_box_append(GTK_BOX(box), button_close);

	GtkWidget *button_keep = gtk_button_new_with_label("Go back");
	gtk_box_append(GTK_BOX(box), button_keep);

	g_signal_connect_swapped(button_keep, "clicked", G_CALLBACK(gtk_window_destroy),
							 confirm_dialog);

	confirm_params *p = malloc(sizeof(confirm_params));
	p->ef = ef;
	p->confirm_window = confirm_dialog;

	g_signal_connect_swapped(button_close, "clicked", G_CALLBACK(gui_confirm_close), p);

	gtk_window_present(GTK_WINDOW(confirm_dialog));
}