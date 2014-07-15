/*CXXR
 *CXXR This file is part of CXXR, a project to refactor the R interpreter
 *CXXR into C++.  It may consist in whole or in part of program code and
 *CXXR documentation taken from the R project itself, incorporated into
 *CXXR CXXR (and possibly MODIFIED) under the terms of the GNU General Public
 *CXXR Licence.
 *CXXR
 *CXXR CXXR is Copyright (C) 2008-13 Andrew R. Runnalls, subject to such other
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

#include "CXXR/ArgList.hpp"
#include "CXXR/Environment.h"
#include "CXXR/FunctionBase.h"
#include "CXXR/PairList.h"
#include "CXXR/RObject.h"
#include "CXXR/Symbol.h"
#include "CXXR/jit/CompiledFrame.hpp"

/*
 * This file contains functions that are available in the runtime module.
 * It gets compiled into LLVM bytecode as part of the compilation process, and
 * the JIT compiler loads the bytecode and makes it available to the functions
 * it builds.
 * Because it is available as LLVM IR at runtime, LLVM is able to inline these
 * directly into the code that it generates.
 *
 * clang -emit-llvm -c  -o foo.bc # emits bitcode
 * clang -emit-llvm -S  -o foo.ll # emits human readable IR
 */

using namespace CXXR;

extern "C" {

RObject* cxxr_runtime_evaluate(RObject* value, Environment* environment)
{
    return value->evaluate(environment);
}

RObject* cxxr_runtime_lookupSymbol(const Symbol* value,
				   Environment* environment)
{
    return const_cast<Symbol*>(value)->evaluate(environment);
}

/*
 * Lookup a symbol in a CompiledFrame.
 * Note that this function doesn't handle the cases where the symbol is
 * ..., ..n, or missingArg().  cxxr_runtime_lookupSymbol should be used in
 * those cases.
 */
RObject* cxxr_runtime_lookupSymbolInCompiledFrame(const Symbol* value,
						  Environment* environment,
						  int position)
{
    assert(environment->frame() != nullptr);
    assert(value != DotsSymbol);
    assert(!value->isDotDotSymbol());
    assert(value != R_MissingArg);
    assert(position >= 0);

    JIT::CompiledFrame* frame
	// TODO(kmillar): when optimizing make this a static cast.
	= dynamic_cast<JIT::CompiledFrame*>(environment->frame());
    assert(frame != nullptr);

    Frame::Binding* binding = frame->binding(position);
    if (binding) {
	// NB: this logic handles the case where the binding is a promise that
	//   resolves to an unboundValue or missingArgument slightly differently
	//   than Symbol::evaluate() does.
	std::pair<RObject*, bool> pair = binding->forcedValue2();
	RObject* value = pair.first;
	if (value
	    && value != Symbol::missingArgument()
	    && value != Symbol::unboundValue())
	{
	    if (pair.second) {
		SET_NAMED(value, 2);
	    }
	    else if (NAMED(value) < 1) {
		SET_NAMED(value, 1);
	    }
	    return value;
	}
    }
    // Fallback to the interpreter.
    return cxxr_runtime_lookupSymbol(value, environment);
}

FunctionBase* cxxr_runtime_lookupFunction(const Symbol* symbol,
					  Environment* environment)
{
    return findFunction(symbol, environment, true);
}

RObject* cxxr_runtime_callFunction(const FunctionBase* function,
				   const PairList* args, const Expression* call,
				   Environment* environment)
{
    ArgList arglist(args, ArgList::RAW);
    return function->apply(&arglist, environment, call);
}
}
