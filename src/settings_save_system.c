#include "settings_save_system.h"
#include "data_directory_system.h"
#include "settings.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef enum { SETTINGS_INT, SETTINGS_STRING } Settings_Type;

typedef struct {
	const char *key;
	void *ptr;
	Settings_Type type;
} Settings_Entry;

// Helper function to remove whitespace
void
trim(char *s)
{
	char *end;
	while(*s == ' ' || *s == '\t') { // removes the spaces at the front of the string
		s++;
	}
	end = s + strlen(s) - 1;
	while(end > s && (*end == ' ' || *end == '\t' || *end == '\n')) {
		*end-- = '\0'; // removes spaces and return from the back
	}
}

// Helper function to remove quotation marks
// Returns -1 if no quotes are found
int
strip_quotes(char *s)
{
	size_t len = strlen(s);
	if(len < 2)
		return -1;

	if((s[0] == '"' && s[len - 1] == '"') || (s[0] == '\'' && s[len - 1] == '\'')) {

		memmove(s, s + 1, len - 2);
		s[len - 2] = '\0';
		return 1;
	} else { // malformed or no quotes
		return -1;
	}
}

// Gets the data from the settings file and sets the applications settings to the data
// Returns -1 for wrong format
// Returns 0 for no file
// Returns 1 for sucess
int
settings_file_load()
{
	char *filename = get_data_path(SETTINGS_FILE_NAME);
	FILE *file = fopen(filename, "r");
	if(!file) {
		return 0;
	}

	settings_state settings = { 0 };

	// This list must be updated if the settings are updated
	// and also the same list in the save function
	Settings_Entry entries[] = { { "font_family", &settings.font_family, SETTINGS_STRING },
								 { "font_size", &settings.font_size, SETTINGS_INT },
								 { "textwrap", &settings.textwrap, SETTINGS_INT },
								 { "whitespace", &settings.whitespace, SETTINGS_INT },
								 { "linenums", &settings.linenums, SETTINGS_INT },
								 { "darkmode", &settings.darkmode, SETTINGS_INT } };
	int entry_count = sizeof(entries) / sizeof(entries[0]);

	char line[256];
	while(fgets(line, sizeof(line), file)) {
		char *divider = strchr(line, ':');
		if(!divider)
			continue; // ignore malformatted line

		*divider = '\0';
		char *key = line;
		char *value = divider + 1;

		trim(key);
		trim(value);

		for(int i = 0; i < entry_count; i++) {
			if(strcmp(key, entries[i].key) == 0) {
				switch(entries[i].type) {
				case SETTINGS_INT:
					*(int *)entries[i].ptr = atoi(value);
					break;
				case SETTINGS_STRING:
					if(strip_quotes(value)) { // if quotes found
						strncpy(entries[i].ptr, value, sizeof(settings.font_family) - 1);
						((char *)entries[i].ptr)[sizeof(settings.font_family) - 1] = '\0';
					}
					// printf("%s", value);
					break;
				}
				break;
			}
		}
	}
	settings_set(&settings);
	fclose(file);
	return 1;
}

// Gets the settings from the application settings and saves in the set file
int
settings_file_save()
{
	char *filename = get_data_path(SETTINGS_FILE_NAME);
	FILE *file = fopen(filename, "w");
	if(!file)
		return 0;

	settings_state settings = settings_get();

	// This list must be updated if the settings are updated
	Settings_Entry entries[] = { { "font_family", &settings.font_family, SETTINGS_STRING },
								 { "font_size", &settings.font_size, SETTINGS_INT },
								 { "textwrap", &settings.textwrap, SETTINGS_INT },
								 { "whitespace", &settings.whitespace, SETTINGS_INT },
								 { "linenums", &settings.linenums, SETTINGS_INT },
								 { "darkmode", &settings.darkmode, SETTINGS_INT } };
	int entry_count = sizeof(entries) / sizeof(entries[0]);

	// write each line to the file
	for(int i = 0; i < entry_count; i++) {
		fprintf(file, "%s: ", entries[i].key);
		switch(entries[i].type) {
		case SETTINGS_INT:
			fprintf(file, "%d", *(int *)entries[i].ptr);
			break;
		case SETTINGS_STRING:
			fprintf(file, "%s", (char *)entries[i].ptr);
			break;
		}
		fprintf(file, "\n");
	}

	fclose(file);
	return 1;
}
