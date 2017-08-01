/*
 * (C) Copyright 2009-2016 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef QG_MODEL_QGGEOMETRY_H_
#define QG_MODEL_QGGEOMETRY_H_

#include <ostream>
#include <string>

#include "model/QgFortran.h"
#include "util/ObjectCounter.h"
#include "util/Printable.h"

namespace eckit {
  class Configuration;
}

namespace qg {

// -----------------------------------------------------------------------------
/// QgGeometry handles geometry for QG model.

class QgGeometry : public util::Printable,
                   private util::ObjectCounter<QgGeometry> {
 public:
  static const std::string classname() {return "qg::QgGeometry";}

  explicit QgGeometry(const eckit::Configuration &);
  QgGeometry(const QgGeometry &);
  ~QgGeometry();

  int& toFortran() {return keyGeom_;}
  const int& toFortran() const {return keyGeom_;}

 private:
  QgGeometry & operator=(const QgGeometry &);
  void print(std::ostream &) const;
  int keyGeom_;
};
// -----------------------------------------------------------------------------

}  // namespace qg

#endif  // QG_MODEL_QGGEOMETRY_H_
