/*
 * (C) Copyright 2009-2016 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef XXXX_MODEL_XXXXINCREMENT_H_
#define XXXX_MODEL_XXXXINCREMENT_H_

#include <ostream>
#include <string>

#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>

#include "model/XxxxFields.h"
#include "model/XxxxGeometry.h"
#include "oops/base/GeneralizedDepartures.h"
#include "util/DateTime.h"
#include "util/Duration.h"
#include "util/ObjectCounter.h"
#include "util/Printable.h"
#include "util/dot_product.h"

namespace eckit {
  class Configuration;
}

namespace xxxx {
  class XxxxState;
  class XxxxVariables;

/// Increment Class: Difference between two states
/*!
 *  Some fields that are present in a State may not be present in
 *  an Increment. The Increment contains everything that is needed by
 *  the tangent-linear and adjoint models.
 */

// -----------------------------------------------------------------------------

class XxxxIncrement : public oops::GeneralizedDepartures,
                    public util::Printable,
                    private util::ObjectCounter<XxxxIncrement> {
 public:
  static const std::string classname() {return "xxxx::XxxxIncrement";}

/// Constructor, destructor
  XxxxIncrement(const XxxxGeometry &, const XxxxVariables &, const util::DateTime &);
  XxxxIncrement(const XxxxGeometry &, const XxxxIncrement &);
  XxxxIncrement(const XxxxIncrement &, const bool);
  XxxxIncrement(const XxxxIncrement &);
  virtual ~XxxxIncrement();

/// Basic operators
  void diff(const XxxxState &, const XxxxState &);
  void zero();
  void zero(const util::DateTime &);
  XxxxIncrement & operator =(const XxxxIncrement &);
  XxxxIncrement & operator+=(const XxxxIncrement &);
  XxxxIncrement & operator-=(const XxxxIncrement &);
  XxxxIncrement & operator*=(const double &);
  void axpy(const double &, const XxxxIncrement &, const bool check = true);
  double dot_product_with(const XxxxIncrement &) const;
  void schur_product_with(const XxxxIncrement &);
  void random();

/// Interpolate to observation location
//  void interpolateTL(const LocQG &, GomQG &) const;
//  void interpolateAD(const LocQG &, const GomQG &);

/// I/O and diagnostics
  void read(const eckit::Configuration &);
  void write(const eckit::Configuration &) const;
  double norm() const {return fields_->norm();}
  const util::DateTime & validTime() const {return fields_->time();}
  util::DateTime & validTime() {return fields_->time();}
  void updateTime(const util::Duration & dt) {fields_->time() += dt;}

/// Access to fields
  XxxxFields & fields() {return *fields_;}
  const XxxxFields & fields() const {return *fields_;}

  boost::shared_ptr<const XxxxGeometry> geometry() const {
    return fields_->geometry();
  }

/// Other
  void activateModel();
  void deactivateModel();

  void accumul(const double &, const XxxxState &);

/// Data
 private:
  void print(std::ostream &) const;
  boost::scoped_ptr<XxxxFields> fields_;
  boost::scoped_ptr<XxxxFields> stash_;
};
// -----------------------------------------------------------------------------

}  // namespace xxxx

#endif  // XXXX_MODEL_XXXXINCREMENT_H_
