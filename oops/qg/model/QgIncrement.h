/*
 * (C) Copyright 2009-2016 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef QG_MODEL_QGINCREMENT_H_
#define QG_MODEL_QGINCREMENT_H_

#include <ostream>
#include <string>

#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>

#include "model/QgFields.h"
#include "model/QgGeometry.h"
#include "oops/base/GeneralizedDepartures.h"
#include "util/DateTime.h"
#include "util/Duration.h"
#include "util/ObjectCounter.h"
#include "util/Printable.h"
#include "util/dot_product.h"

namespace eckit {
  class Configuration;
}

namespace qg {
  class GomQG;
  class LocQG;
  class ModelBiasIncrement;
  class QgErrorCovariance;
  class QgState;
  class VarQG;

/// Increment Class: Difference between two states
/*!
 *  Some fields that are present in a State may not be present in
 *  an Increment. The Increment contains everything that is needed by
 *  the tangent-linear and adjoint models.
 */

// -----------------------------------------------------------------------------

class QgIncrement : public oops::GeneralizedDepartures,
                    public util::Printable,
                    private util::ObjectCounter<QgIncrement> {
 public:
  static const std::string classname() {return "qg::QgIncrement";}

/// Constructor, destructor
  QgIncrement(const QgGeometry &, const VarQG &, const util::DateTime &);
  QgIncrement(const QgGeometry &, const QgIncrement &);
  QgIncrement(const QgIncrement &, const bool);
  QgIncrement(const QgIncrement &);
  virtual ~QgIncrement();

/// Basic operators
  void diff(const QgState &, const QgState &);
  void zero();
  void zero(const util::DateTime &);
  QgIncrement & operator =(const QgIncrement &);
  QgIncrement & operator+=(const QgIncrement &);
  QgIncrement & operator-=(const QgIncrement &);
  QgIncrement & operator*=(const double &);
  void axpy(const double &, const QgIncrement &, const bool check = true);
  double dot_product_with(const QgIncrement &) const;
  void schur_product_with(const QgIncrement &);
  void random();

/// Interpolate to observation location
  void interpolateTL(const LocQG &, GomQG &) const;
  void interpolateAD(const LocQG &, const GomQG &);

/// I/O and diagnostics
  void read(const eckit::Configuration &);
  void write(const eckit::Configuration &) const;
  double norm() const {return fields_->norm();}
  const util::DateTime & validTime() const {return fields_->time();}
  util::DateTime & validTime() {return fields_->time();}
  void updateTime(const util::Duration & dt) {fields_->time() += dt;}

/// Access to fields
  QgFields & fields() {return *fields_;}
  const QgFields & fields() const {return *fields_;}

  boost::shared_ptr<const QgGeometry> geometry() const {
    return fields_->geometry();
  }

/// Other
  void activateModel();
  void deactivateModel();

  void accumul(const double &, const QgState &);

/// Data
 private:
  void print(std::ostream &) const;
  boost::scoped_ptr<QgFields> fields_;
  boost::scoped_ptr<QgFields> stash_;
};
// -----------------------------------------------------------------------------

}  // namespace qg

#endif  // QG_MODEL_QGINCREMENT_H_
