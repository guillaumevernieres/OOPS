/*
 * (C) Copyright 2009-2016 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef OOPS_ASSIMILATION_UPTRISOLVE_H_
#define OOPS_ASSIMILATION_UPTRISOLVE_H_

#include <vector>

namespace oops {

/*! \file UpTriSolve.h
 * \brief Solves the upper triangular system y = H \ s
*/

void UpTriSolve(const std::vector <std::vector<double> > & H,
                const std::vector <double> & s, std::vector <double> & y,
                const int & dim) {
  // Backward solve H s = y
  for (int iiter = dim - 1; iiter >= 0; iiter--) {
    y[iiter] = s[iiter];
    for (int jiter = iiter + 1; jiter < dim; jiter++) {
      y[iiter] -= H[jiter][iiter] * y[jiter];
    }
    y[iiter] *= 1/H[iiter][iiter];
  }
}
}  // namespace oops

#endif  // OOPS_ASSIMILATION_UPTRISOLVE_H_
