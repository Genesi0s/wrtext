/**
 * @file file_dialog.c
 * @brief Implementations for GTK file dialogs.
 */

#include "file_dialog.h"
#include <gio/gio.h>
#include <stdlib.h>
#include <unistd.h>

// Internal helper for waiting for the native dialog response(sychrone)
struct fd_cbdata {
	GtkFileChooserNative *native;
	gint response;
	gboolean done;
	char *result;
};

// Prevent multiple dialogs: track the currently open native dialog.
static GtkNativeDialog *fd_current = NULL;

static void
fd_on_response(GtkNativeDialog *native, gint response, gpointer user_data)
{
	struct fd_cbdata *d = user_data;

	d->response = response;
	if(response == GTK_RESPONSE_ACCEPT) {
		GFile *file = gtk_file_chooser_get_file(GTK_FILE_CHOOSER(native));
		if(file) {
			char *path = g_file_get_path(file);
			if(path)
				d->result = g_strdup(path);
			g_object_unref(file);
		}
	}

	d->done = TRUE;

	/* Clear the global current dialog pointer. */
	if(fd_current == native)
		fd_current = NULL;
}

/**
 * @brief Show a blocking "Open File" dialog and return the chosen path.
 *
 * The returned string must be freed with g_free() by the caller.
 */
char *
file_dialog_open_file(GtkWindow *parent)
{
	if(fd_current)
		return NULL;

	GtkFileChooserNative *native = gtk_file_chooser_native_new(
		"Open File", parent, GTK_FILE_CHOOSER_ACTION_OPEN, "_Open", "_Cancel");

	struct fd_cbdata cb = { 0 };
	cb.native = native;
	cb.done = FALSE;
	cb.result = NULL;

	/// Current open dialog
	g_signal_connect(native, "response", G_CALLBACK(fd_on_response), &cb);
	fd_current = GTK_NATIVE_DIALOG(native);
	gtk_native_dialog_show(GTK_NATIVE_DIALOG(native));

	// Process events until dialog is done.
	while(!cb.done) {
		if(!g_main_context_pending(g_main_context_default()))
			usleep(1000);
		g_main_context_iteration(g_main_context_default(), FALSE);
	}

	char *result = cb.result;
	g_object_unref(native);
	return result;
}

/**
 * @brief Show a blocking "Save File" dialog and return the chosen path.
 *
 * The returned string must be freed with g_free() by the caller.
 */
char *
file_dialog_save_file(GtkWindow *parent, const char *suggested_name)
{
	// If a dialog is already open, don't open another one.
	if(fd_current)
		return NULL;

	GtkFileChooserNative *native = gtk_file_chooser_native_new(
		"Save File", parent, GTK_FILE_CHOOSER_ACTION_SAVE, "_Save", "_Cancel");

	if(suggested_name)
		gtk_file_chooser_set_current_name(GTK_FILE_CHOOSER(native), suggested_name);

	struct fd_cbdata cb = { 0 };
	cb.native = native;
	cb.done = FALSE;
	cb.result = NULL;

	g_signal_connect(native, "response", G_CALLBACK(fd_on_response), &cb);
	fd_current = GTK_NATIVE_DIALOG(native);
	gtk_native_dialog_show(GTK_NATIVE_DIALOG(native));

	/* Process events until dialog is done. */
	while(!cb.done) {
		if(!g_main_context_pending(g_main_context_default()))
			usleep(1000); /* 1ms sleep */
		g_main_context_iteration(g_main_context_default(), FALSE);
	}

	char *result = cb.result;
	g_object_unref(native);
	return result;
}

/**
 * @brief Show a blocking "Create File" dialog and return the chosen path.
 *
 * The returned string must be freed with g_free() by the caller.
 */
char *
file_dialog_create_file(GtkWindow *parent, const char *suggested_name)
{
	if(fd_current)
		return NULL;

	GtkFileChooserNative *native = gtk_file_chooser_native_new(
		"Create File", parent, GTK_FILE_CHOOSER_ACTION_SAVE, "_Create", "_Cancel");

#if GTK_MAJOR_VERSION < 4
	gtk_file_chooser_set_do_overwrite_confirmation(GTK_FILE_CHOOSER(native), TRUE);
#endif

	if(suggested_name)
		gtk_file_chooser_set_current_name(GTK_FILE_CHOOSER(native), suggested_name);

	struct fd_cbdata cb = { 0 };
	cb.native = native;
	cb.done = FALSE;
	cb.result = NULL;

	g_signal_connect(native, "response", G_CALLBACK(fd_on_response), &cb);
	fd_current = GTK_NATIVE_DIALOG(native);
	gtk_native_dialog_show(GTK_NATIVE_DIALOG(native));

	// Process events until dialog is done
	while(!cb.done) {
		if(!g_main_context_pending(g_main_context_default()))
			usleep(1000);
		g_main_context_iteration(g_main_context_default(), FALSE);
	}

	char *result = cb.result;
	g_object_unref(native);
	return result;
}
