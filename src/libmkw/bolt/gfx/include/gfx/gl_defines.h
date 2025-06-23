#pragma once

#include "glad/glad.h"

#ifndef NDEBUG
GLenum glCheckError_(const char *file, int line);
#define glCheckError() glCheckError_(__FILE__, __LINE__)
#else
#define glCheckError()
#endif
