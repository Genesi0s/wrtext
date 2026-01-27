#include "gui_alert.h"
#include "logger.h"
#include <gtk/gtk.h>
#include <stdio.h>
#include <string.h>

GtkWidget *parent = NULL;

void
gui_alert_init(GtkWidget *parent_window)
{
	parent = parent_window;
}

void
gui_alert_error(const char *format, ...)
{

	if(parent == NULL) {
		log_err(__FILE__, "No parent found for the alert window");
		return;
	}

	char message[128];
	message[0] = '\0';

	va_list args;
	va_start(args, format);
	vsnprintf(message, 128, format, args); // Saves the message
	va_end(args);

	// Creates error window
	GtkWidget *alert_dialog = gtk_window_new();
	gtk_window_set_title(GTK_WINDOW(alert_dialog), "Error");
	gtk_window_set_transient_for(GTK_WINDOW(alert_dialog), GTK_WINDOW(parent));
	gtk_window_set_modal(GTK_WINDOW(alert_dialog), TRUE);
	gtk_window_set_resizable(GTK_WINDOW(alert_dialog), FALSE);

	GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 12);
	gtk_widget_set_margin_start(box, 12);
	gtk_widget_set_margin_end(box, 12);
	gtk_widget_set_margin_top(box, 12);
	gtk_widget_set_margin_bottom(box, 12);
	gtk_window_set_child(GTK_WINDOW(alert_dialog), box);

	GtkWidget *hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
	gtk_box_append(GTK_BOX(box), hbox);
	GtkWidget *image = gtk_image_new_from_icon_name("dialog-error");
	gtk_image_set_icon_size(GTK_IMAGE(image), GTK_ICON_SIZE_LARGE);
	gtk_box_append(GTK_BOX(hbox), image);

	GtkWidget *label = gtk_label_new(message);
	gtk_box_append(GTK_BOX(hbox), label);

	GtkWidget *button = gtk_button_new_with_label("OK");
	gtk_box_append(GTK_BOX(box), button);

	g_signal_connect_swapped(button, "clicked", G_CALLBACK(gtk_window_destroy), alert_dialog);

	gtk_window_present(GTK_WINDOW(alert_dialog));
}