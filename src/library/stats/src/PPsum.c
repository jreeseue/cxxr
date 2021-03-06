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

/* Copyright (C) 1997-1999  Adrian Trapletti
   Copyright (C) 2012 The R Core Team

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public
   License along with this library; if not, a copy is available at
   http://www.r-project.org/Licenses/
*/

#include <R.h>

static double R_pp_sum (double *u, int n, int l)
{
    double tmp1, tmp2;

    tmp1 = 0.0;
    for (int i = 1; i <= l; i++) {
	tmp2 = 0.0;
	for (int j = i; j < n; j++) tmp2 += u[j] * u[j-i];
	tmp2 *= 1.0 - i/(l + 1.0);
	tmp1 += tmp2;
    }
    return 2.0 * tmp1 / n;
}

#include <Rinternals.h>

SEXP pp_sum(SEXP u, SEXP sl)
{
    u = PROTECT(coerceVector(u, REALSXP));
    int n = LENGTH(u), l = asInteger(sl);
    double trm = R_pp_sum(REAL(u), n, l);
    UNPROTECT(1);
    return ScalarReal(trm);
}

SEXP intgrt_vec(SEXP x, SEXP xi, SEXP slag)
{
    x = PROTECT(coerceVector(x, REALSXP));
    xi = PROTECT(coerceVector(xi, REALSXP));
    int n = LENGTH(x), lag = asInteger(slag);
    SEXP ans = PROTECT(allocVector(REALSXP, n + lag));
    double *rx = REAL(x), *y = REAL(ans);
    Memzero(y, n + lag); Memcpy(y, REAL(xi), lag);
    for (int i = lag; i < lag + n; i++) y[i] = rx[i - lag] + y[i - lag];
    UNPROTECT(3);
    return ans;
}
