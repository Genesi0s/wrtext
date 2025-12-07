#include "example.h"

void
add_textArea(GtkWidget *win)
{
	// Creates text area
	GtkWidget *textArea = gtk_text_view_new();
	gtk_text_view_new();

	// Adds text area to
	// windowaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
	gtk_container_add(GTK_CONTAINER(win), textArea);
}