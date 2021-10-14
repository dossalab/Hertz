#ifndef ERRORS_ERRORS_H
#define ERRORS_ERRORS_H

enum error_code {
	ERR_OK = 0,

	ERR_SYSTEM,
	ERR_NO_WINDOW,
	ERR_NO_FILE
};

const char *error_to_string(int error);

#endif
