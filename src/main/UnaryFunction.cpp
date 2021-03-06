/*CXXR $Id$
 *CXXR
 *CXXR This file is part of CXXR, a project to refactor the R interpreter
 *CXXR into C++.  It may consist in whole or in part of program code and
 *CXXR documentation taken from the R project itself, incorporated into
 *CXXR CXXR (and possibly MODIFIED) under the terms of the GNU General Public
 *CXXR Licence.
 *CXXR 
 *CXXR CXXR is Copyright (C) 2008-14 Andrew R. Runnalls, subject to such other
 *CXXR copyrights and copyright restrictions as may be stated below.
 *CXXR 
 *CXXR CXXR is not part of the R project, and bugs and other issues should
 *CXXR not be reported via r-bugs or other R project channels; instead refer
 *CXXR to the CXXR website.
 *CXXR */

/*
 *  R : A Computer Language for Statistical Data Analysis
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2.1 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, a copy is available at
 *  http://www.r-project.org/Licenses/
 */

/** @file UnaryFunction.cpp
 *
 * @brief Implementation of VectorOps::UnaryFunction and related functions.
 */

#include "CXXR/UnaryFunction.hpp"

#include "CXXR/Symbol.h"

using namespace CXXR;
using namespace VectorOps;

void CopyLayoutAttributes::copyAttributes(VectorBase* to,
					  const VectorBase* from)
{
    RObject* names = from->getAttribute(NamesSymbol);
    if (names)
	to->setAttribute(NamesSymbol, names);
    RObject* dim = from->getAttribute(DimSymbol);
    if (dim) {
	to->setAttribute(DimSymbol, dim);
	RObject* dimnames = from->getAttribute(DimNamesSymbol);
	if (dimnames)
	    to->setAttribute(DimNamesSymbol, dimnames);
    }
}
