#include "errors.h"

const char *error_to_string(int error)
{
	switch (-error) {
	case ERR_OK:
		return "(not an error)";
	case ERR_SYSTEM:
		return "system (setup) error";
	case ERR_NO_WINDOW:
		return "unable to open window";
	case ERR_NO_FILE:
		return "file not found";
	case ERR_SHADER_INVALID:
		return "shader is invalid";
	}

	return "(unknown error)";
}
