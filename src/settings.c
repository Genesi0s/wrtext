#include "settings.h"
#include "gui_edit.h"
#include <gtk/gtk.h>
#include <string.h>

// Actual application settings, safely encapsulated.
settings_state t;

settings_state
settings_get()
{
	return t;
}

void
settings_set(settings_state *s)
{
	t.darkmode = s->darkmode;
	t.font[0] = 0;
	strcpy(t.font, s->font);
	t.linenums = s->linenums;
	t.textwrap = s->textwrap;
	t.whitespace = s->whitespace;
}

void
settings_apply()
{
	settings_state s = settings_get();
	gui_edit_set_wrap(s.textwrap);

	// dark theme setting
	GtkSettings *gtk_settings = gtk_settings_get_default();
	if(gtk_settings) {
		g_object_set(gtk_settings, "gtk-application-prefer-dark-theme", s.darkmode, NULL);
	}
}