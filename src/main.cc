/*
 * Copyright (C) 2002 by Holger Rapp 
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

#include "os.h"
#include "options.h"
#include "errors.h"
#include "output.h"
#include "fileloc.h"
#include "input.h"
#include "cursor.h"
#include "intro.h"
#include "mainmenue.h"
#include "setup.h"
#include "font.h"
#include "ui.h"
#include <stdlib.h>
#include <string.h>
#include <sdl/sdl.h>

#ifdef DEBUG
// This is set for the assert function, to skip over to graphical asserts
int graph_is_init=0; 
#endif

/** g_main function 
 * This is the OS Independant main function.
 * 
 * It makes sure, Options and Commandline is parsed,
 * initializes Graphics and Resource Handler
 *
 * return Exitcode of App
 */
inline int g_main(int argn, char** argc)
{
	static Font_Handler f; // Global instance for the hole game
	static User_Interface ui; // Global instance for the hole game
	static Cursor cur; // This is the global cursor instance, init here for the whole game
	
	// Setup default searchpaths
	setup_searchpaths();
	
	// Handle options
	handle_options(argn, argc);
	
	// Setup font handler and user interface for the use in widelands
	setup_fonthandler();
	setup_ui();
	
	// Until now, no window is created, nothing is started, just initialized.
	// By now, we musn't use the tell_user function any longer!! 
	// Rather, we can now use a user_interface window for critical errors, which
	// terminates the application in a good matter
#ifdef 	DEBUG
	graph_is_init=1;
#endif
	// run intro
	run_intro();

	// run main_menue
	main_menue();

	return RET_OK;
}

// ** unix, win32 console *****************************************************
#if !defined(WIN32) || (defined(WIN32) && defined(_CONSOLE))

int main(int argcount, char** args)
{
	return g_main(argcount, args);
}

// ** win32 gui ***************************************************************
#else // WIN32, !_CONSOLE

static int ParseCommandLine(char* cmdline, char** argv)
{
	char* bufp = cmdline;
	int argc = 0;

	while (*bufp)
	{
		// skip leading whitespace
		while (isspace(*bufp))
			++bufp;
		// skip arg
		if (*bufp == '"')
		{
			++bufp;
			if (*bufp)
			{
				if (argv)
					argv[argc] = bufp;
				++argc;
			}
			// skip
			while (*bufp && (*bufp != '"'))
				++bufp;
		}
		else
		{
			if (*bufp)
			{
				if (argv)
					argv[argc] = bufp;
				++argc;
			}
			// skip
			while (*bufp && ! isspace(*bufp))
				++bufp;
		}
		if (*bufp)
		{
			if (argv)
				*bufp = '\0';
			++bufp;
		}
	}
	if (argv)
		argv[argc] = NULL;
	return argc;
}

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR /*cmdLine*/, int)
{
	// param cmdLine and GetCommandLine() are NOT the same
	char* cmdLine = new char[strlen(GetCommandLine()) + 1];
	strcpy(cmdLine, GetCommandLine());

	char** args = new char*[ParseCommandLine(cmdLine, NULL)];
	int argcount = ParseCommandLine(cmdLine, args);

	int ret =  g_main(argcount, args);

	delete[] args;
	delete[] cmdLine;
	
	return ret;
}

#endif