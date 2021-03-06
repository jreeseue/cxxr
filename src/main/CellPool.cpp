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
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street Fifth Floor, Boston, MA 02110-1301 USA
 */

/** @file CellPool.cpp
 *
 * Implementation of class CellPool
 */

#include "CXXR/CellPool.hpp"

#ifndef __APPLE__
#include <features.h>
#endif

#include <algorithm>
#include <cstdlib>
#include <iostream>

using namespace std;
using namespace CXXR;

CellPool::~CellPool()
{
    for (vector<void*>::iterator it = m_admin->m_superblocks.begin();
	 it != m_admin->m_superblocks.end(); ++it)
#if _POSIX_C_SOURCE >= 200112L || _XOPEN_SOURCE >= 600
	free(*it);
#else
	::operator delete(*it);
#endif
    delete m_admin;
}

unsigned int CellPool::cellsFree() const
{
    unsigned int ans = 0;
    for (const Cell* c = m_free_cells; c; c = c->m_next)
	++ans;
    return ans;
}

bool CellPool::check() const
{
    unsigned int free_cells = 0;
    for (Cell* c = m_free_cells; c; c = c->m_next) {
	checkCell(c);
	++free_cells;
    }
    if (free_cells > m_admin->cellsExisting()) {
	cerr << "CellPool::check(): internal inconsistency\n";
	abort();
    }
    return true;
}

void CellPool::checkAllocatedCell(const void* p) const 
{
    checkCell(p);
    const Cell* cp = static_cast<const Cell*>(p);
    bool found = false;
    for (Cell* c = m_free_cells; !found && c; c = c->m_next)
	found = (c == cp);
    if (found) {
	cerr << "CellPool::checkCell : designated block"
	    " is (already) free.\n";
	abort();
    }
}

void CellPool::checkCell(const void* p) const
{
    if (!p) return;
    const char* pc = static_cast<const char*>(p);
    bool found = false;
    for (vector<void*>::const_iterator it = m_admin->m_superblocks.begin();
	 !found && it != m_admin->m_superblocks.end(); ++it) {
	ptrdiff_t offset = pc - static_cast<const char*>(*it);
	if (offset >= 0
	    && offset < static_cast<long>(m_admin->m_superblocksize)) {
	    found = true;
	    if (std::size_t(offset)%m_admin->m_cellsize != 0) {
		cerr << "CellPool::checkCell : designated block"
		        " is misaligned\n";
		abort();
	    }
	}
    }
    if (!found) {
	cerr << "CellPool::checkCell : "
	    "designated block doesn't belong to this CellPool\n";
	abort();
    }
}

void CellPool::defragment()
{
    vector<Cell*> vf(cellsFree());
    // Assemble vector of pointers to free cells:
    {
	Cell* c = m_free_cells;
	for (vector<Cell*>::iterator it = vf.begin(); it != vf.end(); ++it) {
	    *it = c;
	    c = c->m_next;
	}
    }
    // Sort by increasing address:
    sort(vf.begin(), vf.end());
    // Restring the pearls:
    {
	Cell* next = 0;
	vector<Cell*>::reverse_iterator rit = vf.rbegin();
#ifdef CELLFIFO
	m_last_free_cell = (rit == vf.rend() ? 0 : *rit);
#endif
	for ( ; rit != vf.rend(); ++rit) {
	    Cell* c = *rit;
	    c->m_next = next;
	    next = c;
	}
	m_free_cells = next;
    }
    // check();
}

void CellPool::initialize(size_t dbls_per_cell, size_t cells_per_superblock)
{
    m_admin = new Admin(dbls_per_cell, cells_per_superblock);
}

CellPool::Cell* CellPool::seekMemory() throw (std::bad_alloc)
{
#if _POSIX_C_SOURCE >= 200112L || _XOPEN_SOURCE >= 600
    void* memblock;
    // We assume the memory page size is some multiple of 4096 bytes:
    if (0 != posix_memalign(&memblock, 4096, m_admin->m_superblocksize))
	throw bad_alloc();
    char* superblock = static_cast<char*>(memblock);
#else
    char* superblock
	= static_cast<char*>(::operator new(m_admin->m_superblocksize));
#endif
    m_admin->m_superblocks.push_back(superblock);
    // Initialise cells:
    {
	ptrdiff_t offset
	    = ptrdiff_t(m_admin->m_superblocksize - m_admin->m_cellsize);
#ifdef CELLFIFO
	m_last_free_cell = reinterpret_cast<Cell*>(superblock + offset);
#endif
	Cell* next = 0;
	while (offset >= 0) {
	    next = new (superblock + offset) Cell(next);
	    offset -= ptrdiff_t(m_admin->m_cellsize);
	}
	return next;
    }
}
