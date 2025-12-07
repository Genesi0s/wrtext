#include "example.h"
#include <gtk/gtk.h>

/*!
 * @brief Function called once the application runs
 * @param app Pointer to the GTK application
 * @param user_data Other info
*/
static void
activate(GtkApplication *app, gpointer user_data)
{
	// Creates a GTK window.
	GtkWidget *window;
	window = gtk_application_window_new(app);

	// Calls function that adds a textArea to the window that receives as parameter
	add_textArea(window);

	// Sets window title and size
	gtk_window_set_title(GTK_WINDOW(window), "Example editor");
	gtk_window_set_default_size(GTK_WINDOW(window), 400, 400);

	// Shows window
	gtk_widget_show_all(window);
}

/*!
 * @brief Main function of WRText. (Doxygen test)
 * @param argc Number of arguments passed to the program
 * @param argv List of arguments as strings
*/
int
main(int argc, char **argv)
{
	GtkApplication *app; // Variable that stores the pointer to the application object
	int status;			 // Variable used to save the result of the application

	// Creates the GTK application and puts it in the pointer
	app = gtk_application_new("org.gtk.example", G_APPLICATION_DEFAULT_FLAGS);

	// GTK uses signal, here we make it so that once the "activate" signal is called, the activate
	// function is called.
	g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);

	// The application is run and this triggers the "activate" signal, calling the activate
	// function
	status = g_application_run(G_APPLICATION(app), argc, argv);

	// Just some memory cleanup
	g_object_unref(app);

	return status; // Returns result of execution
}
