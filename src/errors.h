#ifndef _ERRORS_H_
#define _ERRORS_H_

#define ERROR_BUG "Błąd programu!"

// shows error message and exits with non-0 return value
// it will show only one error message if message2 is NULL
extern void error(const char* message1, const char* message2, const char* file, int line);

#endif
