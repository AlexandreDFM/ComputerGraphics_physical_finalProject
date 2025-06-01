// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently


#pragma once

#define _CRT_SECURE_NO_DEPRECATE 1

#define WIN32_LEAN_AND_MEAN // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#define NOMINMAX

#if defined(_WIN32) || defined(_WIN64)
#include <tchar.h>
#include <windows.h>
#endif

#include <algorithm>
#include <iostream>
#include <malloc.h>
#include <math.h>
#include <memory.h>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>


#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>


// FLTK widgets.
//
// The FLTK source is rife with dubious type conversions (i.e., void* to long), which
// causes a slew of compiler warnings to be generated.  All of the Fl widgets that
// we use are included in the following block, and for this block we temporarily
// supress display of these warnings.

#pragma warning(disable : 4312)
#pragma warning(disable : 4311)
#pragma warning(disable : 4244)
#pragma warning(disable : 4275)

#include <FL/Fl.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Check_Button.H>
#include <FL/Fl_Choice.H>
#include <FL/Fl_Counter.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_File_Chooser.H>
#include <FL/Fl_Float_Input.H>
#include <FL/Fl_Gl_Window.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Int_Input.H>
#include <FL/Fl_Light_Button.H>
#include <FL/Fl_Menu_Bar.H>
#include <FL/Fl_Menu_Button.H>
#include <FL/Fl_Menu_Item.H>
#include <FL/Fl_Multi_Browser.H>
#include <FL/Fl_Multiline_Output.H>
#include <FL/Fl_Output.H>
#include <FL/Fl_Repeat_Button.H>
#include <FL/Fl_Return_Button.H>
#include <FL/Fl_Select_Browser.H>
#include <FL/Fl_Slider.H>
#include <FL/Fl_Tabs.H>
#include <FL/Fl_Value_Slider.H>
#include <FL/Fl_Widget.H>
#include <FL/Fl_Window.H>

#include <FL/Fl_Input.H>
#include <FL/filename.H>
#include <FL/fl_ask.H>
#include <FL/fl_draw.H>
#include <FL/fl_message.H>
#pragma warning(default : 4312)
#pragma warning(default : 4311)
#pragma warning(default : 4244)


// TODO: reference additional headers your program requires here
