#ifndef KT_DBG_DBGDEFS_H_
#define KT_DBG_DBGDEFS_H_

// Debugging utilities that are never part of a release build.

#ifdef _DEBUG
	
// When declaring a single line of debug code.
#define DBG_LINE(code)		code
// When declaring a debug line in an initializer list
#define DBG_INITIALIZER(initializer_line) , initializer_line

#else

// When declaring a debug line in a header
#define DBG_LINE(code)		;
// When declaring a debug line in an initializer list
#define DBG_INITIALIZER(initializer_line)

#endif

#endif
