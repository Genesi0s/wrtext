#include "fmanager.h"
#include "editor_file.h"
#include "logger.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

editor_file *
fmanager_load(char *file_path)
{
	FILE *f;
	// Opens file
	f = fopen(file_path, "rb");

	if(f == 0) {
		log_err(__FILE__, "Error reading %s", file_path);
		return NULL;
	}

	// Finds file size in bytes
	fseek(f, 0, SEEK_END);
	long fsize = ftell(f);
	fseek(f, 0, SEEK_SET);

	editor_file *ef = calloc(1, sizeof(editor_file));
	ef->contents = calloc(sizeof(char), fsize);

	// Reads file
	if(fread(ef->contents, fsize, 1, f) != 1) {
		log_err(__FILE__, "Error reading %s", file_path);
		return NULL;
	}

	fclose(f); // Closes file

	ef->size = fsize;
	ef->file_path = file_path;

	// finds filename
	char *ls = file_path; // last slash
	char *c = file_path;
	int nl = 0;
	while(*c != '\0') {
		c++;
		nl++;
		if(*c == '/') {
			ls = c + 1;
			nl = 0;
		}
	}
	// log_info(__FILE__, "length: %d, name:%s\n",nl,ls);
	ef->file_name = calloc(1, nl + 1);
	strcpy(ef->file_name, ls);

	editor_file_update_lines(ef);
	return ef;
}

int
fmanager_save(editor_file *ef)
{
	FILE *f = fopen(ef->file_path, "wb");
	if(f == 0) {
		log_err(__FILE__, "Error writing to %s", ef->file_path);
		return -1;
	}
	if(fwrite(ef->contents, ef->size, 1, f) != 1) {
		log_err(__FILE__, "Error writing to %s", ef->file_path);
		return -1;
	}
	return 0;
}