/*
 * (C) Copyright 2009-2016 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef UTIL_RANDOM_VECTOR_F90_H_
#define UTIL_RANDOM_VECTOR_F90_H_

namespace util {

extern "C" {
  void random_vector_f90(const int &, double *);
  void random_vector_gauss_f90(const int &, double *);
}

}  // namespace util
#endif  // UTIL_RANDOM_VECTOR_F90_H_
