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

/** @file PairList.cpp
 *
 * @brief Class CXXR::PairList and associated C interface.
 */

#include "CXXR/PairList.h"

#include <iostream>
#include "localization.h"
#include "R_ext/Error.h"

using namespace std;
using namespace CXXR;

// Force the creation of non-inline embodiments of functions callable
// from C:
namespace CXXR {
    namespace ForceNonInline {
	Rboolean (*BINDING_IS_LOCKEDptr)(SEXP b) = BINDING_IS_LOCKED;
	SEXP (*CAD4Rp)(SEXP e) = CAD4R;
	SEXP (*CADDDRp)(SEXP e) = CADDDR;
	SEXP (*CADDRp)(SEXP e) = CADDR;
	SEXP (*CADRp)(SEXP e) = CADR;
	SEXP (*CDARp)(SEXP e) = CDAR;
	SEXP (*CDDRp)(SEXP e) = CDDR;
	SEXP (*CDRp)(SEXP e) = CDR;
	Rboolean (*IS_ACTIVE_BINDINGptr)(SEXP b) = IS_ACTIVE_BINDING;
	void (*LOCK_BINDINGptr)(SEXP b) = LOCK_BINDING;
	void (*SET_ACTIVE_BINDING_BITptr)(SEXP b) = SET_ACTIVE_BINDING_BIT;
	void (*UNLOCK_BINDINGptr)(SEXP b) = UNLOCK_BINDING;
   }
}

namespace {
    // Used in {,un}packGPBits():
    const unsigned int BINDING_LOCK_MASK = 1<<14;
    const unsigned int ACTIVE_BINDING_MASK = 1<<15;
}

PairList::PairList(const PairList& pattern)
    : ConsCell(pattern, 0)
{
    // Clone the tail:
    PairList* c = this;
    const PairList* pl = pattern.m_tail;
    while (pl) {
	c->m_tail = expose(new PairList(*pl, 0));
	c = c->m_tail;
	pl = pl->m_tail;
    }
}

// Non-inlined so it can get put in .text.hot :
PairList::~PairList()
{}

PairList* PairList::clone() const
{
    return expose(new PairList(*this));
}

PairList* PairList::make(size_t sz) throw (std::bad_alloc)
{
    PairList* ans = 0;
    while (sz--)
	ans = cons(0, ans);
    return ans;
}

unsigned int PairList::packGPBits() const
{
    unsigned int ans = ConsCell::packGPBits();
    if (m_binding_locked)
	ans |= BINDING_LOCK_MASK;
    if (m_active_binding)
	ans |= ACTIVE_BINDING_MASK;
    return ans;
}

const char* PairList::typeName() const
{
    return staticTypeName();
}

void PairList::unpackGPBits(unsigned int gpbits)
{
    ConsCell::unpackGPBits(gpbits);
    m_binding_locked = ((gpbits & BINDING_LOCK_MASK) != 0);
    m_active_binding = ((gpbits & ACTIVE_BINDING_MASK) != 0);
}

BOOST_CLASS_EXPORT_IMPLEMENT(CXXR::PairList)

// ***** C interface *****

SEXP Rf_allocList(unsigned int n)
{
    return PairList::make(n);
}

SEXP Rf_cons(SEXP cr, SEXP tl)
{
    return PairList::cons(cr, SEXP_downcast<PairList*>(tl));
}

Rboolean IS_ACTIVE_BINDING(SEXP b)
{
    const ConsCell* cc = SEXP_downcast<ConsCell*>(b);
    if (cc->sexptype() != LISTSXP)
	return FALSE;
    const PairList* pl = static_cast<const PairList*>(cc);
    return Rboolean(pl->m_active_binding);
}

SEXP SETCDR(SEXP x, SEXP y)
{
    if (!x)
	Rf_error(_("bad value"));
    ConsCell& cc = *SEXP_downcast<ConsCell*>(x);
    PairList* tl = SEXP_downcast<PairList*>(y);
    cc.setTail(tl);
    return y;
}

SEXP SETCADR(SEXP x, SEXP y)
{
    if (!x)
	Rf_error(_("bad value"));
    ConsCell* cc = SEXP_downcast<ConsCell*>(x);
    cc = cc->tail();
    if (!cc)
	Rf_error(_("bad value"));
    cc->setCar(y);
    return y;
}

SEXP SETCADDR(SEXP x, SEXP y)
{
    if (!x)
	Rf_error(_("bad value"));
    ConsCell* cc = SEXP_downcast<ConsCell*>(x);
    cc = cc->tail();
    if (!cc)
	Rf_error(_("bad value"));
    cc = cc->tail();
    if (!cc)
	Rf_error(_("bad value"));
    cc->setCar(y);
    return y;
}

SEXP SETCADDDR(SEXP x, SEXP y)
{
    if (!x)
	Rf_error(_("bad value"));
    ConsCell* cc = SEXP_downcast<ConsCell*>(x);
    cc = cc->tail();
    if (!cc)
	Rf_error(_("bad value"));
    cc = cc->tail();
    if (!cc)
	Rf_error(_("bad value"));
    cc = cc->tail();
    if (!cc)
	Rf_error(_("bad value"));
    cc->setCar(y);
    return y;
}

SEXP SETCAD4R(SEXP x, SEXP y)
{
    if (!x)
	Rf_error(_("bad value"));
    ConsCell* cc = SEXP_downcast<ConsCell*>(x);
    cc = cc->tail();
    if (!cc)
	Rf_error(_("bad value"));
    cc = cc->tail();
    if (!cc)
	Rf_error(_("bad value"));
    cc = cc->tail();
    if (!cc)
	Rf_error(_("bad value"));
    cc = cc->tail();
    if (!cc)
	Rf_error(_("bad value"));
    cc->setCar(y);
    return y;
}
