/*
 * (C) Copyright 2009-2016 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef XXXX_MODEL_XXXXTRAITS_H_
#define XXXX_MODEL_XXXXTRAITS_H_

#include <string>

#include "model/XxxxGeometry.h"
#include "model/XxxxIncrement.h"
#include "model/XxxxState.h"
#include "model/XxxxVariables.h"

namespace xxxx {

struct XxxxTraits {
  static std::string name() {return "XXXX";}

  typedef xxxx::XxxxGeometry            Geometry;
  typedef xxxx::XxxxVariables           Variables;
  typedef xxxx::XxxxState               State;
  typedef xxxx::XxxxIncrement           Increment;
};

}  // namespace xxxx

#endif  // XXXX_MODEL_XXXXTRAITS_H_
