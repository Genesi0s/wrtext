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
	ef->file_path = strdup(file_path);
	// ef->file_path = filepath; // WARNING: file_path may be freed by the caller of this function
	// which would result in this memory region being invalid
	// Recommended: make a copy of the path and require freeing on closing a file.

	// finds filename
	editor_file_update_name(ef);

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
		fclose(f);
		return -1;
	}
	fclose(f);
	return 0;
}