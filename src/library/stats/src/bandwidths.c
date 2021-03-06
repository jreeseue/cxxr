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
 *  bandwidth.c by W. N. Venables and B. D. Ripley  Copyright (C) 1994-2001
 *  Copyright (C) 2012  The R Core Team
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

#include <stdlib.h> //abs
#include <math.h>
#include <Rinternals.h>

#ifndef max
#  define max(a,b) ((a) > (b) ? (a) : (b))
#  define min(a,b) ((a) < (b) ? (a) : (b))
#endif


#if !defined(PI)
#  define PI 3.14159265
#endif

#define DELMAX 1000
/* Avoid slow and possibly error-producing underflows by cutting off at
   plus/minus sqrt(DELMAX) std deviations */
/* Formulae (6.67) and (6.69) of Scott (1992), the latter corrected. */

SEXP bw_ucv(SEXP sn, SEXP sd, SEXP cnt, SEXP sh)
{
    double h = asReal(sh), d = asReal(sd), sum = 0.0, term, u;
    int n = asInteger(sn), nbin = LENGTH(cnt), *x = INTEGER(cnt);
    for (int i = 0; i < nbin; i++) {
	double delta = i * d / h;
	delta *= delta;
	if (delta >= DELMAX) break;
	term = exp(-delta / 4) - sqrt(8.0) * exp(-delta / 2);
	sum += term * x[i];
    }
    u = 1 / (2 * n * h * sqrt(PI)) + sum / (n * n * h * sqrt(PI));
    return ScalarReal(u);
}

SEXP bw_bcv(SEXP sn, SEXP sd, SEXP cnt, SEXP sh)
{
    double h = asReal(sh), d = asReal(sd), sum = 0.0, term, u;
    int n = asInteger(sn), nbin = LENGTH(cnt), *x = INTEGER(cnt);

    sum = 0.0;
    for (int i = 0; i < nbin; i++) {
	double delta = i * d / h; delta *= delta;
	if (delta >= DELMAX) break;
	term = exp(-delta / 4) * (delta * delta - 12 * delta + 12);
	sum += term * x[i];
    }
    u = 1 / (2 * n * h * sqrt(PI)) + sum / (64 * n * n * h * sqrt(PI));
    return ScalarReal(u);
}

SEXP bw_phi4(SEXP sn, SEXP sd, SEXP cnt, SEXP sh)
{
    double h = asReal(sh), d = asReal(sd), sum = 0.0, term, u;
    int n = asInteger(sn), nbin = LENGTH(cnt), *x = INTEGER(cnt);

    for (int i = 0; i < nbin; i++) {
	double delta = i * d / h; delta *= delta;
	if (delta >= DELMAX) break;
	term = exp(-delta / 2) * (delta * delta - 6 * delta + 3);
	sum += term * x[i];
    }
    sum = 2 * sum + n * 3;	/* add in diagonal */
    u = sum / (n * (n - 1) * pow(h, 5.0) * sqrt(2 * PI));
    return ScalarReal(u);
}

SEXP bw_phi6(SEXP sn, SEXP sd, SEXP cnt, SEXP sh)
{
    double h = asReal(sh), d = asReal(sd), sum = 0.0, term, u;
    int n = asInteger(sn), nbin = LENGTH(cnt), *x = INTEGER(cnt);

    for (int i = 0; i < nbin; i++) {
	double delta = i * d / h; delta *= delta;
	if (delta >= DELMAX) break;
	term = exp(-delta / 2) *
	    (delta * delta * delta - 15 * delta * delta + 45 * delta - 15);
	sum += term * x[i];
    }
    sum = 2 * sum - 15 * n;	/* add in diagonal */
    u = sum / (n * (n - 1) * pow(h, 7.0) * sqrt(2 * PI));
    return ScalarReal(u);
}

/* This would be impracticable for long vectors.  Better to bin x first */
SEXP bw_den(SEXP nbin, SEXP sx)
{
    int nb = asInteger(nbin), n = LENGTH(sx);
    double xmin, xmax, rang, dd, *x = REAL(sx);
    SEXP sc = PROTECT(allocVector(INTSXP, nb));
    int *cnt = INTEGER(sc);

    for (int i = 0; i < nb; i++) cnt[i] = 0;
    xmin = xmax = x[0];
    for (int i = 1; i < n; i++) {
	xmin = min(xmin, x[i]);
	xmax = max(xmax, x[i]);
    }
    rang = (xmax - xmin) * 1.01;
    dd = rang / nb;
    for (int i = 1; i < n; i++) {
	int ii = (int)(x[i] / dd);
	for (int j = 0; j < i; j++) {
	    int jj = (int)(x[j] / dd);
	    cnt[abs(ii - jj)]++;
	}
    }
    SEXP ans = PROTECT(allocVector(VECSXP, 2));
    SET_VECTOR_ELT(ans, 0, ScalarReal(dd));
    SET_VECTOR_ELT(ans, 1, sc);
    UNPROTECT(2);
    return ans;
}
