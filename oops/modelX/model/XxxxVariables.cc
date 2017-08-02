/*
 * (C) Copyright 2009-2016 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <iostream>

#include "model/XxxxVariables.h"
#include "model/XxxxFortran.h"

// -----------------------------------------------------------------------------
namespace xxxx {
// -----------------------------------------------------------------------------

void XxxxVariables::print(std::ostream & os) const {
  int nv;
  int nl;
  xxxx_var_info_f90(keyVar_, nv, nl);
  os << nv;
  if (nl == 1) os << " with LBC";
  ASSERT(nl == 0 || nl == 1);
}

// -----------------------------------------------------------------------------

}  // namespace xxxx
