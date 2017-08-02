/*
 * (C) Copyright 2009-2016 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef XXXX_MODEL_XXXXGEOMETRY_H_
#define XXXX_MODEL_XXXXGEOMETRY_H_

#include <ostream>
#include <string>

#include "model/XxxxFortran.h"
#include "util/ObjectCounter.h"
#include "util/Printable.h"

namespace eckit {
  class Configuration;
}

namespace xxxx {

// -----------------------------------------------------------------------------
/// XxxxGeometry handles geometry for XXXX model.

class XxxxGeometry : public util::Printable,
                     private util::ObjectCounter<XxxxGeometry> {
 public:
  static const std::string classname() {return "xxxx::XxxxGeometry";}

  explicit XxxxGeometry(const eckit::Configuration &);
  XxxxGeometry(const XxxxGeometry &);
  ~XxxxGeometry();

  int& toFortran() {return keyGeom_;}
  const int& toFortran() const {return keyGeom_;}

 private:
  XxxxGeometry & operator=(const XxxxGeometry &);
  void print(std::ostream &) const;
  int keyGeom_;
};
// -----------------------------------------------------------------------------

}  // namespace xxxx

#endif  // XXXX_MODEL_XXXXGEOMETRY_H_
