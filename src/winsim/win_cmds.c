/*
 * win_cmds.c
 *
 *  Created on: Jun 14, 2018
 *      Author: Mike
 */
#include <windows.h>
#include <tcl.h>

HWND NameToWindow(Tcl_Interp* interp, const char* name, HWND parent) {
	return NULL;
}
int
Win_BindCmd(
    ClientData clientData,		/* Main window associated with
				 * interpreter. */
    Tcl_Interp *interp,		/* Current interpreter. */
    int argc,			/* Number of arguments. */
    const char **argv)		/* Argument strings. */
{
	HWND hwnd = (HWND) clientData;
	HWND winPtr;
	ClientData object;
    fputs(argv[0], stdout);
    for (int arg = 1; arg<argc; arg++)
    	fprintf(stdout, " %s", argv[arg]);
    fputc('\n', stdout);
#if 0
	if ((argc < 2) || (argc > 4)) {
		Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0],
				" window ?pattern? ?command?\"", (char *) NULL);
		return TCL_ERROR;
	}
	if (argv[1][0] == '.') {
		winPtr =  NameToWindow(interp, argv[1], tkwin);
		if (winPtr == NULL) {
			return TCL_ERROR;
		}
		object = (ClientData) winPtr->pathName;
	} else {
		winPtr = (HWND) clientData;
		object = (ClientData) Tk_GetUid(argv[1]);
	}

	if (argc == 4) {
		int append = 0;
		unsigned long mask;

		if (argv[3][0] == 0) {
			return Tk_DeleteBinding(interp, winPtr->mainPtr->bindingTable,
					object, argv[2]);
		}
		if (argv[3][0] == '+') {
			argv[3]++;
			append = 1;
		}
		mask = Tk_CreateBinding(interp, winPtr->mainPtr->bindingTable, object,
				argv[2], argv[3], append);
		if (mask == 0) {
			return TCL_ERROR;
		}
	} else if (argc == 3) {
		char *command;

		command = Tk_GetBinding(interp, winPtr->mainPtr->bindingTable, object,
				argv[2]);
		if (command == NULL) {
			Tcl_ResetResult(interp);
			return TCL_OK;
		}
		interp->result = command;
	} else {
		Tk_GetAllBindings(interp, winPtr->mainPtr->bindingTable, object);
	}
#endif
	return TCL_OK;
}

int
Win_WmCmd(
    ClientData clientData,		/* Main window associated with
				 * interpreter. */
    Tcl_Interp *interp,		/* Current interpreter. */
    int argc,			/* Number of arguments. */
    const char **argv)		/* Argument strings. */
{
	return TCL_OK;
}
int
Win_OptionCmd(
    ClientData clientData,		/* Main window associated with
				 * interpreter. */
    Tcl_Interp *interp,		/* Current interpreter. */
    int argc,			/* Number of arguments. */
    const char **argv)		/* Argument strings. */
{
	return TCL_OK;
}
static int ItemAsCmd(ClientData clientData, Tcl_Interp* interp,  int argc, const char** argv)
{
	fputs("ItemAsCmd: ", stdout);
	if (clientData) fprintf(stdout, "%s ", clientData);
    fputs(argv[0], stdout);
    for (int arg = 1; arg<argc; arg++)
    	fprintf(stdout, " %s", argv[arg]);
    fputc('\n', stdout);
	return TCL_OK;
}
Tcl_HashTable hs;
void foo()
{
	Tcl_HashKeyType hwndType = {
		TCL_HASH_KEY_TYPE_VERSION,
		TCL_HASH_KEY_RANDOMIZE_HASH,
		NULL,
		NULL,
		NULL,
		NULL
	};
	Tcl_InitCustomHashTable(&hs,TCL_CUSTOM_PTR_KEYS, &hwndType);
}
/*
 *--------------------------------------------------------------
 *
 * Tk_FrameCmd --
 *
 *	This procedure is invoked to process the "frame" and
 *	"toplevel" Tcl commands.  See the user documentation for
 *	details on what it does.
 *
 * Results:
 *	A standard Tcl result.
 *
 * Side effects:
 *	See the user documentation.
 *
 *--------------------------------------------------------------
 */

int
Win_FrameCmd(
    ClientData clientData,	/* Main window associated with
				 * interpreter. */
    Tcl_Interp *interp,		/* Current interpreter. */
    int argc,			/* Number of arguments. */
    const char **argv)		/* Argument strings. */
{
	HWND tkwin = (HWND) clientData;
	HWND new;
#if 0
	register Frame *framePtr;
	Tk_Uid screenUid;
#endif
	const char *className, *screen;
	int src, dst;

    fputs(argv[0], stdout);
    for (int arg = 1; arg<argc; arg++)
    	fprintf(stdout, " %s", argv[arg]);
    fputc('\n', stdout);
	if (argc < 2) {
		Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0],
				" pathName ?options?\"", (char *) NULL);
		return TCL_ERROR;
	}

	/*
	 * The code below is a special workaround that extracts a few key
	 * options from the argument list now, rather than letting
	 * ConfigureFrame do it.  This is necessary because we have
	 * to know the window's screen (if it's top-level) and its
	 * class before creating the window.
	 */

	screen = NULL;
	className = (argv[0][0] == 't') ? "Toplevel" : "Frame";
	for (src = 2, dst = 2; src < argc; src += 2) {
		char c;

		c = argv[src][1];
		if ((c == 'c')
				&& (strncmp(argv[src], "-class", strlen(argv[src])) == 0)) {
			className = argv[src + 1];
		} else if ((argv[0][0] == 't') && (c == 's')
				&& (strncmp(argv[src], "-screen", strlen(argv[src])) == 0)) {
			screen = argv[src + 1];
		} else {
			argv[dst] = argv[src];
			argv[dst + 1] = argv[src + 1];
			dst += 2;
		}
	}
	argc -= src - dst;

#if 0
	/*
	 * Provide a default screen for top-level windows (same as screen
	 * of parent window).
	 */

	if ((argv[0][0] == 't') && (screen == NULL)) {
		screen = "";
	}
	if (screen != NULL) {
		screenUid = Tk_GetUid(screen);
	} else {
		screenUid = NULL;
	}

	/*
	 * Create the window.
	 */

	new = Tk_CreateWindowFromPath(interp, tkwin, argv[1], screenUid);

	if (new == NULL) {
		return TCL_ERROR;
	}

	Tk_SetClass(new, className);
	framePtr = (Frame *) ckalloc(sizeof(Frame));
	framePtr->tkwin = new;
	framePtr->interp = interp;
	framePtr->screenName = screenUid;
	framePtr->border = NULL;
	framePtr->geometry = NULL;
	framePtr->cursor = None;
	framePtr->flags = 0;
	Tk_CreateEventHandler(framePtr->tkwin, ExposureMask | StructureNotifyMask,
			FrameEventProc, (ClientData) framePtr);
	Tcl_CreateCommand(interp, Tk_PathName(framePtr->tkwin), FrameWidgetCmd,
			(ClientData) framePtr, (void (*)()) NULL);

	if (ConfigureFrame(interp, framePtr, argc - 2, argv + 2, 0) != TCL_OK) {
		Tk_DestroyWindow(framePtr->tkwin);
		return TCL_ERROR;
	}
	if (screenUid != NULL) {
		Tk_DoWhenIdle(MapFrame, (ClientData) framePtr);
	}
	interp->result = Tk_PathName(framePtr->tkwin);
#endif
    Tcl_CreateCommand(interp, argv[1], ItemAsCmd, (char*)className, NULL);
	return TCL_OK;
}

/*
 *----------------------------------------------------------------------
 *
 * Tk_WinfoCmd --
 *
 *	This procedure is invoked to process the "winfo" Tcl command.
 *	See the user documentation for details on what it does.
 *
 * Results:
 *	A standard Tcl result.
 *
 * Side effects:
 *	See the user documentation.
 *
 *----------------------------------------------------------------------
 */

int
Win_WinfoCmd(
    ClientData clientData,	/* Main window associated with
				 * interpreter. */
    Tcl_Interp *interp,		/* Current interpreter. */
    int argc,			/* Number of arguments. */
    const char **argv)		/* Argument strings. */
{
	HWND tkwin = (HWND) clientData;
	int length;
	char c, *argName;
	HWND window;
	register HWND *winPtr;

#define SETUP(name) \
    if (argc != 3) {\
	argName = name; \
	goto wrongArgs; \
    } \
    window = Tk_NameToWindow(interp, argv[2], tkwin); \
    if (window == NULL) { \
	return TCL_ERROR; \
    }

	if (argc < 2) {
		Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0],
				" option ?arg?\"", (char *) NULL);
		return TCL_ERROR;
	}
	c = argv[1][0];
	length = strlen(argv[1]);
#if 0
	if ((c == 'a') && (strcmp(argv[1], "atom") == 0)) {
		if (argc != 3) {
			Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0],
					" atom name\"", (char *) NULL);
			return TCL_ERROR;
		}
		sprintf(interp->result, "%d", Tk_InternAtom(tkwin, argv[2]));
	} else if ((c == 'a') && (strncmp(argv[1], "atomname", length) == 0)
			&& (length >= 5)) {
		Atom atom;
		char *name;

		if (argc != 3) {
			Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0],
					" atomname id\"", (char *) NULL);
			return TCL_ERROR;
		}
		if (Tcl_GetInt(interp, argv[2], (int *) &atom) != TCL_OK) {
			return TCL_ERROR;
		}
		name = Tk_GetAtomName(tkwin, atom);
		if (strcmp(name, "?bad atom?") == 0) {
			Tcl_AppendResult(interp, "no atom exists with id \"", argv[2], "\"",
					(char *) NULL);
			return TCL_ERROR;
		}
		interp->result = name;
	} else if ((c == 'c') && (strncmp(argv[1], "children", length) == 0)
			&& (length >= 2)) {
		char *separator, *childName;

		SETUP("children");
		separator = "";
		for (winPtr = ((TkWindow *) window)->childList; winPtr != NULL; winPtr =
				winPtr->nextPtr) {
			childName = Tcl_Merge(1, &winPtr->pathName);
			Tcl_AppendResult(interp, separator, childName, (char *) NULL);
			ckfree(childName);
			separator = " ";
		}
	} else if ((c == 'c') && (strncmp(argv[1], "class", length) == 0)
			&& (length >= 2)) {
		SETUP("class");
		interp->result = Tk_Class(window);
	} else if ((c == 'c') && (strncmp(argv[1], "containing", length) == 0)
			&& (length >= 2)) {
		int rootX, rootY;

		if (argc != 4) {
			Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0],
					" containing rootX rootY\"", (char *) NULL);
			return TCL_ERROR;
		}
		if ((Tk_GetPixels(interp, tkwin, argv[2], &rootX) != TCL_OK)
				|| (Tk_GetPixels(interp, tkwin, argv[3], &rootY) != TCL_OK)) {
			return TCL_ERROR;
		}
		window = Tk_CoordsToWindow(rootX, rootY, tkwin);
		if (window != NULL) {
			interp->result = Tk_PathName(window);
		}
	} else if ((c == 'f') && (strncmp(argv[1], "fpixels", length) == 0)
			&& (length >= 2)) {
		double mm, pixels;

		if (argc != 4) {
			Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0],
					" fpixels window number\"", (char *) NULL);
			return TCL_ERROR;
		}
		window = Tk_NameToWindow(interp, argv[2], tkwin);
		if (window == NULL) {
			return TCL_ERROR;
		}
		if (Tk_GetScreenMM(interp, window, argv[3], &mm) != TCL_OK) {
			return TCL_ERROR;
		}
		pixels = mm * WidthOfScreen(Tk_Screen(window))
				/ WidthMMOfScreen(Tk_Screen(window));
		sprintf(interp->result, "%g", pixels);
	} else if ((c == 'g') && (strncmp(argv[1], "geometry", length) == 0)) {
		SETUP("geometry");
		sprintf(interp->result, "%dx%d+%d+%d", Tk_Width(window),
				Tk_Height(window), Tk_X(window), Tk_Y(window));
	} else if ((c == 'h') && (strncmp(argv[1], "height", length) == 0)) {
		SETUP("height");
		sprintf(interp->result, "%d", Tk_Height(window));
	} else if ((c == 'i') && (strcmp(argv[1], "id") == 0)) {
		SETUP("id");
		sprintf(interp->result, "0x%x", Tk_WindowId(window));
	} else if ((c == 'i') && (strncmp(argv[1], "interps", length) == 0)
			&& (length >= 2)) {
		if (argc != 2) {
			Tcl_AppendResult(interp, "wrong # args:  should be \"", argv[1],
					" interps\"", (char *) NULL);
			return TCL_ERROR;
		}
		return TkGetInterpNames(interp, tkwin);
	} else if ((c == 'i') && (strncmp(argv[1], "ismapped", length) == 0)
			&& (length >= 2)) {
		SETUP("ismapped");
		interp->result = Tk_IsMapped(window) ? "1" : "0";
	} else if ((c == 'n') && (strncmp(argv[1], "name", length) == 0)) {
		SETUP("geometry");
		interp->result = Tk_Name(window);
	} else if ((c == 'p') && (strncmp(argv[1], "parent", length) == 0)) {
		SETUP("geometry");
		winPtr = (TkWindow *) window;
		if (winPtr->parentPtr != NULL) {
			interp->result = winPtr->parentPtr->pathName;
		}
	} else if ((c == 'p') && (strncmp(argv[1], "pathname", length) == 0)
			&& (length >= 2)) {
		Window id;

		if (argc != 3) {
			argName = "pathname";
			goto wrongArgs;
		}
		if (Tcl_GetInt(interp, argv[2], (int *) &id) != TCL_OK) {
			return TCL_ERROR;
		}
		if ((XFindContext(Tk_Display(tkwin), id, tkWindowContext,
				(void *) &window) != 0)
				|| (((TkWindow *) window)->mainPtr
						!= ((TkWindow *) tkwin)->mainPtr)) {
			Tcl_AppendResult(interp, "window id \"", argv[2],
					"\" doesn't exist in this application", (char *) NULL);
			return TCL_ERROR;
		}
		interp->result = Tk_PathName(window);
	} else if ((c == 'p') && (strncmp(argv[1], "pixels", length) == 0)
			&& (length >= 2)) {
		int pixels;

		if (argc != 4) {
			Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0],
					" pixels window number\"", (char *) NULL);
			return TCL_ERROR;
		}
		window = Tk_NameToWindow(interp, argv[2], tkwin);
		if (window == NULL) {
			return TCL_ERROR;
		}
		if (Tk_GetPixels(interp, window, argv[3], &pixels) != TCL_OK) {
			return TCL_ERROR;
		}
		sprintf(interp->result, "%d", pixels);
	} else if ((c == 'r') && (strncmp(argv[1], "reqheight", length) == 0)
			&& (length >= 4)) {
		SETUP("reqheight");
		sprintf(interp->result, "%d", Tk_ReqHeight(window));
	} else if ((c == 'r') && (strncmp(argv[1], "reqwidth", length) == 0)
			&& (length >= 4)) {
		SETUP("reqwidth");
		sprintf(interp->result, "%d", Tk_ReqWidth(window));
	} else if ((c == 'r') && (strcmp(argv[1], "rootx") == 0)) {
		int x, y;

		SETUP("rootx");
		Tk_GetRootCoords(window, &x, &y);
		sprintf(interp->result, "%d", x);
	} else if ((c == 'r') && (strcmp(argv[1], "rooty") == 0)) {
		int x, y;

		SETUP("rooty");
		Tk_GetRootCoords(window, &x, &y);
		sprintf(interp->result, "%d", y);
	} else if ((c == 's') && (strcmp(argv[1], "screen") == 0)) {
		char string[20];

		SETUP("screen");
		sprintf(string, "%d", Tk_ScreenNumber(window));
		Tcl_AppendResult(interp, Tk_DisplayName(window), ".", string,
				(char *) NULL);
	} else if ((c == 's') && (strncmp(argv[1], "screencells", length) == 0)
			&& (length >= 7)) {
		SETUP("screencells");
		sprintf(interp->result, "%d",
				Tk_DefaultVisual(Tk_Screen(window))->map_entries);
	} else if ((c == 's') && (strncmp(argv[1], "screendepth", length) == 0)
			&& (length >= 7)) {
		SETUP("screendepth");
		sprintf(interp->result, "%d", Tk_DefaultDepth(Tk_Screen(window)));
	} else if ((c == 's') && (strncmp(argv[1], "screenheight", length) == 0)
			&& (length >= 7)) {
		SETUP("screenheight");
		sprintf(interp->result, "%d", HeightOfScreen(Tk_Screen(window)));
	} else if ((c == 's') && (strncmp(argv[1], "screenmmheight", length) == 0)
			&& (length >= 9)) {
		SETUP("screenmmheight");
		sprintf(interp->result, "%d", HeightMMOfScreen(Tk_Screen(window)));
	} else if ((c == 's') && (strncmp(argv[1], "screenmmwidth", length) == 0)
			&& (length >= 9)) {
		SETUP("screenmmwidth");
		sprintf(interp->result, "%d", WidthMMOfScreen(Tk_Screen(window)));
	} else
#endif
		if ((c == 's') && (strncmp(argv[1], "screenvisual", length) == 0)
			&& (length >= 7)) {
			Tcl_SetResult(interp, "truecolor", TCL_STATIC);
		}
#if 0
	} else if ((c == 's') && (strncmp(argv[1], "screenwidth", length) == 0)
			&& (length >= 7)) {
		SETUP("screenwidth");
		sprintf(interp->result, "%d", WidthOfScreen(Tk_Screen(window)));
	} else if ((c == 's') && (strcmp(argv[1], "server") == 0)) {
		SETUP("server");
		Tcl_AppendResult(interp, Tk_DisplayName(window), (char *) NULL);
	} else if ((c == 't') && (strncmp(argv[1], "toplevel", length) == 0)) {
		SETUP("toplevel");
		for (winPtr = (TkWindow *) window; !(winPtr->flags & TK_TOP_LEVEL);
				winPtr = winPtr->parentPtr) {
			/* Empty loop body. */
		}
		interp->result = winPtr->pathName;
	} else if ((c == 'w') && (strncmp(argv[1], "width", length) == 0)) {
		SETUP("width");
		sprintf(interp->result, "%d", Tk_Width(window));
	} else if ((c == 'x') && (argv[1][1] == '\0')) {
		SETUP("x");
		sprintf(interp->result, "%d", Tk_X(window));
	} else if ((c == 'y') && (argv[1][1] == '\0')) {
		SETUP("y");
		sprintf(interp->result, "%d", Tk_Y(window));
	} else {
		Tcl_AppendResult(interp, "bad option \"", argv[1],
				"\": must be atom, atomname, children, class, fpixels, geometry, height, ",
				"id, interps, ismapped, name, parent, pathname, ",
				"pixels, reqheight, reqwidth, rootx, rooty, ",
				"screen, screencells, screendepth, screenheight, ",
				"screenmmheight, screenmmwidth, screenvisual, ",
				"screenwidth, toplevel, width, x, or y", (char *) NULL);
		return TCL_ERROR;
	}
#endif
	return TCL_OK;

	wrongArgs: Tcl_AppendResult(interp, "wrong # arguments: must be \"",
			argv[0], " ", argName, " window\"", (char *) NULL);
	return TCL_ERROR;
}

#define TYPE_LABEL		0
#define TYPE_BUTTON		1
#define TYPE_CHECK_BUTTON	2
#define TYPE_RADIO_BUTTON	3

int
Win_ButtonCmd(
    ClientData clientData,	/* Main window associated with
				 * interpreter. */
    Tcl_Interp *interp,		/* Current interpreter. */
    int argc,			/* Number of arguments. */
    const char **argv)		/* Argument strings. */
{
    HWND *butPtr;
    int type;
    HWND tkwin = (HWND) clientData;
    HWND new;

    if (argc < 2) {
	Tcl_AppendResult(interp, "wrong # args:  should be \"",
		argv[0], " pathName ?options?\"", (char *) NULL);
	return TCL_ERROR;
    }

    switch (argv[0][0]) {
	case 'l':
	    type = TYPE_LABEL;
	    break;
	case 'b':
	    type = TYPE_BUTTON;
	    break;
	case 'c':
	    type = TYPE_CHECK_BUTTON;
	    break;
	case 'r':
	    type = TYPE_RADIO_BUTTON;
	    break;
	default:
	    Tcl_AppendResult(interp,
		    "unknown button-creation command \"%.50s\"", argv[0], NULL);
	    return TCL_ERROR;
    }
    Tcl_CreateCommand(interp, argv[1], ItemAsCmd, (char*)argv[0], NULL);

    /*
     * Create the new window.
     */
#if 0

    new = Tk_CreateWindowFromPath(interp, tkwin, argv[1], (char *) NULL);
    if (new == NULL) {
	return TCL_ERROR;
    }

    /*
     * Initialize the data structure for the button.
     */

    butPtr = (Button *) ckalloc(sizeof(Button));
    butPtr->tkwin = new;
    butPtr->interp = interp;
    butPtr->type = type;
    butPtr->text = NULL;
    butPtr->textVarName = NULL;
    butPtr->bitmap = None;
    butPtr->state = tkNormalUid;
    butPtr->normalBorder = NULL;
    butPtr->activeBorder = NULL;
    butPtr->borderWidth = 0;
    butPtr->relief = TK_RELIEF_FLAT;
    butPtr->fontPtr = NULL;
    butPtr->normalFg = NULL;
    butPtr->activeFg = NULL;
    butPtr->disabledFg = NULL;
    butPtr->normalTextGC = None;
    butPtr->activeTextGC = None;
    butPtr->gray = None;
    butPtr->disabledGC = None;
    butPtr->selectorFg = NULL;
    butPtr->selectorGC = None;
    butPtr->selVarName = NULL;
    butPtr->onValue = NULL;
    butPtr->offValue = NULL;
    butPtr->cursor = None;
    butPtr->command = NULL;
    butPtr->flags = 0;
    butPtr->updateTimerToken = 0;

    Tk_SetClass(new, classNames[type]);
//fprintf(stderr, "ButtonWidgetCmd Made %s %s\n", Tk_Class(butPtr->tkwin), Tk_PathName(butPtr->tkwin));
    Tk_CreateEventHandler(butPtr->tkwin, ExposureMask|StructureNotifyMask,
	    ButtonEventProc, (ClientData) butPtr);
    Tcl_CreateCommand(interp, Tk_PathName(butPtr->tkwin), ButtonWidgetCmd,
	    (ClientData) butPtr, (void (*)()) NULL);
    if (ConfigureButton(interp, butPtr, argc-2, argv+2,
	    configFlags[type]) != TCL_OK) {
	Tk_DestroyWindow(butPtr->tkwin);
	return TCL_ERROR;
    }

    interp->result = Tk_PathName(butPtr->tkwin);
#endif
    return TCL_OK;
}

/*
 *--------------------------------------------------------------
 *
 * Tk_MenubuttonCmd --
 *
 *	This procedure is invoked to process the "button", "label",
 *	"radiobutton", and "checkbutton" Tcl commands.  See the
 *	user documentation for details on what it does.
 *
 * Results:
 *	A standard Tcl result.
 *
 * Side effects:
 *	See the user documentation.
 *
 *--------------------------------------------------------------
 */

int
Win_MenubuttonCmd(
	ClientData clientData,	/* Main window associated with
	 	 	 	 	 	 	 * interpreter. */
    Tcl_Interp *interp,		/* Current interpreter. */
    int argc,				/* Number of arguments. */
    const char **argv)		/* Argument strings. */
{
    //register MenuButton *mbPtr;
    HWND tkwin = (HWND) clientData;
    HWND new;

    if (argc < 2) {
	Tcl_AppendResult(interp, "wrong # args:  should be \"",
		argv[0], " pathName ?options?\"", (char *) NULL);
	return TCL_ERROR;
    }

    /*
     * Create the new window.
     */

#if 0
    new = Tk_CreateWindowFromPath(interp, tkwin, argv[1], (char *) NULL);
    if (new == NULL) {
	return TCL_ERROR;
    }
    /*
     * Initialize the data structure for the button.
     */

    mbPtr = (MenuButton *) ckalloc(sizeof(MenuButton));
    mbPtr->tkwin = new;
    mbPtr->interp = interp;
    mbPtr->menuName = NULL;
    mbPtr->varName = NULL;
    mbPtr->text = NULL;
    mbPtr->underline = -1;
    mbPtr->textVarName = NULL;
    mbPtr->bitmap = None;
    mbPtr->state = tkNormalUid;
    mbPtr->normalBorder = NULL;
    mbPtr->activeBorder = NULL;
    mbPtr->borderWidth = 0;
    mbPtr->relief = TK_RELIEF_FLAT;
    mbPtr->fontPtr = NULL;
    mbPtr->normalFg = NULL;
    mbPtr->activeFg = NULL;
    mbPtr->disabledFg = NULL;
    mbPtr->normalTextGC = NULL;
    mbPtr->activeTextGC = NULL;
    mbPtr->gray = None;
    mbPtr->disabledGC = NULL;
    mbPtr->cursor = None;
    mbPtr->flags = 0;

    Tk_SetClass(mbPtr->tkwin, "Menubutton");
    Tk_CreateEventHandler(mbPtr->tkwin, ExposureMask|StructureNotifyMask,
	    MenuButtonEventProc, (ClientData) mbPtr);
    Tcl_CreateCommand(interp, Tk_PathName(mbPtr->tkwin), MenuButtonWidgetCmd,
	    (ClientData) mbPtr, (void (*)()) NULL);
    if (ConfigureMenuButton(interp, mbPtr, argc-2, argv+2, 0) != TCL_OK) {
	Tk_DestroyWindow(mbPtr->tkwin);
	return TCL_ERROR;
    }

    interp->result = Tk_PathName(mbPtr->tkwin);
#endif
    return TCL_OK;
}

/*
 *--------------------------------------------------------------
 *
 * Tk_MenuCmd --
 *
 *	This procedure is invoked to process the "menu" Tcl
 *	command.  See the user documentation for details on
 *	what it does.
 *
 * Results:
 *	A standard Tcl result.
 *
 * Side effects:
 *	See the user documentation.
 *
 *--------------------------------------------------------------
 */

int
Win_MenuCmd(
    ClientData clientData,	/* Main window associated with
				 	 	 	 * interpreter. */
    Tcl_Interp *interp,		/* Current interpreter. */
    int argc,				/* Number of arguments. */
    const char **argv)		/* Argument strings. */
{
    HWND tkwin = (HWND) clientData;
    HWND new;
#if 0
    register Menu *menuPtr;
    XSetWindowAttributes atts;
#endif

    if (argc < 2) {
	Tcl_AppendResult(interp, "wrong # args: should be \"",
		argv[0], " pathName ?options?\"", (char *) NULL);
	return TCL_ERROR;
    }
    fputs(argv[0], stdout);
    for (int arg = 1; arg<argc; arg++)
    	fprintf(stdout, " %s", argv[arg]);
    fputc('\n', stdout);

    /*
     * Create the new window.  Set override-redirect so the window
     * manager won't add a border or argue about placement, and set
     * save-under so that the window can pop up and down without a
     * lot of re-drawing.
     */

#if 0
    new = Tk_CreateWindowFromPath(interp, tkwin, argv[1], "");
    if (new == NULL) {
	return TCL_ERROR;
    }
    atts.override_redirect = True;
    atts.save_under = True;
    Tk_ChangeWindowAttributes(new, CWOverrideRedirect|CWSaveUnder, &atts);

    /*
     * Initialize the data structure for the menu.
     */

    menuPtr = (Menu *) ckalloc(sizeof(Menu));
    menuPtr->tkwin = new;
    menuPtr->interp = interp;
    menuPtr->entries = NULL;
    menuPtr->numEntries = 0;
    menuPtr->active = -1;
    menuPtr->group = NULL;
    menuPtr->border = NULL;
    menuPtr->activeBorder = NULL;
    menuPtr->fontPtr = NULL;
    menuPtr->fg = NULL;
    menuPtr->textGC = None;
    menuPtr->disabledFg = NULL;
    menuPtr->gray = None;
    menuPtr->disabledGC = None;
    menuPtr->activeFg = NULL;
    menuPtr->activeGC = None;
    menuPtr->selectorFg = NULL;
    menuPtr->selectorGC = None;
    menuPtr->cursor = None;
    menuPtr->postedCascade = NULL;
    menuPtr->flags = 0;

    Tk_SetClass(new, "Menu");
    Tk_CreateEventHandler(menuPtr->tkwin, ExposureMask|StructureNotifyMask,
	    MenuEventProc, (ClientData) menuPtr);
    Tcl_CreateCommand(interp, Tk_PathName(menuPtr->tkwin), MenuWidgetCmd,
	    (ClientData) menuPtr, (void (*)()) NULL);
    if (ConfigureMenu(interp, menuPtr, argc-2, argv+2, 0) != TCL_OK) {
	goto error;
    }

    interp->result = Tk_PathName(menuPtr->tkwin);
#else
    Tcl_CreateCommand(interp, argv[1], ItemAsCmd, "Menu", NULL);
#endif
    return TCL_OK;

#if 0
    error:
    Tk_DestroyWindow(menuPtr->tkwin);
    return TCL_ERROR;
#endif
}
/*
 * Pack command, arranges children around the edges of parent.
 * See: https://www.tcl.tk/man/tcl8.4/TkCmd/pack.htm
 */

int Win_PackCmd(ClientData clientData, Tcl_Interp* interp,  int argc, const char** argv)
{
    return TCL_OK;
}

/*
 * Tk canvas command
 * Create a window which contains a list of drawn entities that can
 * be bound to commands (from the UI commamds).
 */
int
Win_CanvasCmd(ClientData clientData, Tcl_Interp* interp,  int argc, const char** argv)
{
    fputs(argv[0], stdout);
    for (int arg = 1; arg<argc; arg++)
    	fprintf(stdout, " %s", argv[arg]);
    fputc('\n', stdout);
    Tcl_CreateCommand(interp, argv[1], ItemAsCmd, "Canvas", NULL);
    return TCL_OK;
}

/*
 * Tk scale command
 * creates a window with a widget that defines a scale
 * https://www.tcl.tk/man/tcl8.4/TkCmd/scale.htm
 */
int
Win_ScaleCmd(ClientData clientData, Tcl_Interp* interp,  int argc, const char** argv)
{
	fputs(argv[0], stdout);
	for (int arg = 1; arg<argc; arg++)
		fprintf(stdout, " %s", argv[arg]);
	fputc('\n', stdout);
	Tcl_CreateCommand(interp, argv[1], ItemAsCmd, "Scale", NULL);
	return TCL_OK;
}
/*
 * Tk scale command
 * creates a window with a widget that defines a scale
 * https://www.tcl.tk/man/tcl8.4/TkCmd/scale.htm
 */
int
Win_ScrollbarCmd(ClientData clientData, Tcl_Interp* interp,  int argc, const char** argv)
{
	fputs(argv[0], stdout);
	for (int arg = 1; arg<argc; arg++)
		fprintf(stdout, " %s", argv[arg]);
	fputc('\n', stdout);
	Tcl_CreateCommand(interp, argv[1], ItemAsCmd, "Scrollbar", NULL);
	return TCL_OK;
}
/*
 * Tk text command
 * creates a window with a widget that contains text
 * https://www.tcl.tk/man/tcl8.4/TkCmd/text.htm
 */
int
Win_TextCmd(ClientData clientData, Tcl_Interp* interp,  int argc, const char** argv)
{
	fputs("Win_TextCmd: ", stdout);
	fputs(argv[0], stdout);
	for (int arg = 1; arg<argc; arg++)
		fprintf(stdout, " %s", argv[arg]);
	fputc('\n', stdout);
	Tcl_CreateCommand(interp, argv[1], ItemAsCmd, "Text", NULL);
	return TCL_OK;
}
static int PlaceCmd(ClientData clientData, Tcl_Interp* interp,  int argc, const char** argv)
{
	fputs("PlaceCmd: ", stdout);
    fputs(argv[0], stdout);
    for (int arg = 1; arg<argc; arg++)
    	fprintf(stdout, " %s", argv[arg]);
    fputc('\n', stdout);
	return TCL_OK;
}

/*
 * Tk place command
 * creates a window with a widget that contains children that are placed as desired
 * https://www.tcl.tk/man/tcl8.4/TkCmd/place.htm
 */
int
Win_PlaceCmd(ClientData clientData, Tcl_Interp* interp,  int argc, const char** argv)
{
	fputs(argv[0], stdout);
	for (int arg = 1; arg<argc; arg++)
		fprintf(stdout, " %s", argv[arg]);
	fputc('\n', stdout);
	//Tcl_CreateCommand(interp, argv[1], PlaceCmd, NULL, NULL);
	return TCL_OK;
}

typedef struct {
    char *name;			/* Name of command. */
    int (*cmdProc) (ClientData clientData, Tcl_Interp *interp,
	    int argc, const char**argv);
				/* Command procedure. */
} WinCmd;

WinCmd commands[] = {
    /*
     * Commands that are part of the intrinsics:
     */

#if 0
    {"after",		Tk_AfterCmd},
#endif
    {"bind",		Win_BindCmd},
#if 0
    {"destroy",		Tk_DestroyCmd},
    {"focus",		Tk_FocusCmd},
    {"grab",		Tk_GrabCmd},
#endif
    {"option",		Win_OptionCmd},
    {"pack",		Win_PackCmd},
    {"place",		Win_PlaceCmd},
#if 0
    {"selection",	Tk_SelectionCmd},
    {"tkwait",		Tk_TkwaitCmd},
    {"update",		Tk_UpdateCmd},
#endif
    {"winfo",		Win_WinfoCmd},
    {"wm",		Win_WmCmd},
#if 0
    {"accept",		Tcp_AcceptCmd},
    {"shutdown",	Tcp_ShutdownCmd},
    {"connect",		Tcp_ConnectCmd},
    {"filehandler",	Tcp_FileHandlerCmd},
#endif
    /*
     * Widget-creation commands.
     */
    {"button",		Win_ButtonCmd},
    {"canvas",		Win_CanvasCmd},
    {"checkbutton",	Win_ButtonCmd},
#if 0
    {"entry",		Tk_EntryCmd},
#endif
    {"frame",		Win_FrameCmd},
    {"label",		Win_ButtonCmd},
#if 0
    {"listbox",		Tk_ListboxCmd},
#endif
    {"menu",		Win_MenuCmd},
    {"menubutton",	Win_MenubuttonCmd},
#if 0
    {"message",		Tk_MessageCmd},
#endif
    {"radiobutton",	Win_ButtonCmd},
    {"scale",		Win_ScaleCmd},
    {"scrollbar",   Win_ScrollbarCmd},
    {"text",		Win_TextCmd},
    {"toplevel",	Win_FrameCmd},
    {(char *) NULL,	(int (*)()) NULL}
};

void bindem(Tcl_Interp* interp, HWND tkwin) {
	WinCmd * cmdPtr;
	for (cmdPtr = commands; cmdPtr->name != NULL; cmdPtr++) {
		Tcl_CreateCommand(interp, cmdPtr->name, cmdPtr->cmdProc,
			(ClientData) tkwin, (Tcl_CmdDeleteProc*) NULL);
	}
}
