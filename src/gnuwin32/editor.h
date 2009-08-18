/*CXXR $Id$
 *CXXR
 *CXXR This file is part of CXXR, a project to refactor the R interpreter
 *CXXR into C++.  It may consist in whole or in part of program code and
 *CXXR documentation taken from the R project itself, incorporated into
 *CXXR CXXR (and possibly MODIFIED) under the terms of the GNU General Public
 *CXXR Licence.
 *CXXR 
 *CXXR CXXR is Copyright (C) 2008-9 Andrew R. Runnalls, subject to such other
 *CXXR copyrights and copyright restrictions as may be stated below.
 *CXXR 
 *CXXR CXXR is not part of the R project, and bugs and other issues should
 *CXXR not be reported via r-bugs or other R project channels; instead refer
 *CXXR to the CXXR website.
 *CXXR */

/*
 *  R : A Computer Language for Statistical Data Analysis
 *  file editor.h
 *  Copyright (C) 1999-2008  The R Development Core Team
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, a copy is available at
 *  http://www.r-project.org/Licenses/
 */

void menueditornew(control m);
void menueditoropen(control m);
int editorchecksave(editor c);
void editorsetfont(font f);
int Rgui_Edit(const char *filename, int enc, const char *title, int modal);

#define EDITORMAXTITLE 128
#define MAXNEDITORS 50

struct structEditorData {
    Rboolean file; /* is the editor associated with an existing file */
    char *filename; /* corresponding file, in UTF-8 as from 2.9.0 */
    char *title;    /* window title */
    Rboolean stealconsole;  /* set when using fix() or edit(), so that no events are sent to console until this editor is closed */
    menuitem mcut, mcopy, mdelete, mfind, mreplace,
	mpopcut, mpopcopy, mpopdelete;
    HelpMenuItems hmenu;
    PkgMenuItems pmenu;
};
typedef struct structEditorData *EditorData;
