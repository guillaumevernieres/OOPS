/*
 * (C) Copyright 2009-2016 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef QG_MODEL_QGSTATE_H_
#define QG_MODEL_QGSTATE_H_

#include <ostream>
#include <string>

#include <boost/scoped_ptr.hpp>

#include "model/QgFields.h"
#include "util/DateTime.h"
#include "util/ObjectCounter.h"
#include "util/Printable.h"

namespace eckit {
  class Configuration;
}

namespace qg {
  class GomQG;
  class LocQG;
  class QgGeometry;
  class QgIncrement;
  class VarQG;

/// QG model state
/*!
 * A State contains everything that is needed to propagate the state
 * forward in time.
 */

// -----------------------------------------------------------------------------
class QgState : public util::Printable,
                private util::ObjectCounter<QgState> {
 public:
  static const std::string classname() {return "qg::QgState";}

/// Constructor, destructor
  QgState(const QgGeometry &, const VarQG &, const util::DateTime &);  // Is it used?
  QgState(const QgGeometry &, const eckit::Configuration &);
  QgState(const QgGeometry &, const QgState &);
  QgState(const QgState &);
  virtual ~QgState();
  QgState & operator=(const QgState &);

/// Interpolate to observation location
  void interpolate(const LocQG &, GomQG &) const;

/// Interpolate full fields
  void changeResolution(const QgState & xx);

/// Interactions with Increment
  QgState & operator+=(const QgIncrement &);

/// I/O and diagnostics
  void read(const eckit::Configuration &);
  void write(const eckit::Configuration &) const;
  double norm() const {return fields_->norm();}
  const util::DateTime & validTime() const {return fields_->time();}
  util::DateTime & validTime() {return fields_->time();}

/// Access to fields
  QgFields & fields() {return *fields_;}
  const QgFields & fields() const {return *fields_;}

  boost::shared_ptr<const QgGeometry> geometry() const {
    return fields_->geometry();
  }

/// Other
  void activateModel();
  void deactivateModel();

  void zero();
  void accumul(const double &, const QgState &);

 private:
  void print(std::ostream &) const;
  boost::scoped_ptr<QgFields> fields_;
  boost::scoped_ptr<QgFields> stash_;
};
// -----------------------------------------------------------------------------

}  // namespace qg

#endif  // QG_MODEL_QGSTATE_H_
