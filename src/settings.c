#include "settings.h"
#include "gui_edit.h"
#include "logger.h"
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
	t.font_family[0] = 0;
	strcpy(t.font_family, s->font_family);
	t.font_size = s->font_size;
	t.linenums = s->linenums;
	t.textwrap = s->textwrap;
	t.whitespace = s->whitespace;
}

void
settings_apply()
{
	settings_state s = settings_get();
	// Apply wrap
	gui_edit_set_wrap(s.textwrap);

	// log_info(__FILE__, "Applying font family [%s] with sizee [%d]\n",
	// s.font_family,s.font_size);

	// Apply font
	GtkCssProvider *provider = gtk_css_provider_new();

	// Construct CSS string
	char css[256];
	snprintf(css, sizeof(css), "textview { font-family: '%s'; font-size: %dpt; }", s.font_family,
			 s.font_size);

	// Load CSS into the provider
	gtk_css_provider_load_from_data(provider, css, -1);

	// Apply provider to the display (affects all widgets of this type)
	gtk_style_context_add_provider_for_display(gdk_display_get_default(),
											   GTK_STYLE_PROVIDER(provider),
											   GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

	g_object_unref(provider);
}