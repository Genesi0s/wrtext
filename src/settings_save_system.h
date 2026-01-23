

/*!
	@file settings_save_system.h
	@brief settings_save_system.h and settings_save_system.c handle operations involved with
	loading and saving the settings. Entries are written in a txt file with .cfg format.

*/

#ifndef SETTINGS_SAVE_SYSTEM
#define SETTINGS_SAVE_SYSTEM

#define SETTINGS_FILE_NAME "settings.txt"

int settings_file_load();

int settings_file_save();

#endif
