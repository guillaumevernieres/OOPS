/*
 * (C) Copyright 2009-2016 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef QG_MODEL_QGERRORCOVARIANCE_H_
#define QG_MODEL_QGERRORCOVARIANCE_H_

#include <ostream>
#include <string>
#include <boost/noncopyable.hpp>
#include <boost/scoped_ptr.hpp>

#include "model/QgFortran.h"
#include "model/QgGeometry.h"
#include "eckit/config/Configuration.h"
#include "util/DateTime.h"
#include "util/ObjectCounter.h"
#include "util/Printable.h"

// Forward declarations
namespace qg {
  class QgIncrement;
  class QgState;
  class VarQG;

// -----------------------------------------------------------------------------
/// Background error covariance matrix for QG model.

class QgErrorCovariance : public util::Printable,
                          private boost::noncopyable,
                          private util::ObjectCounter<QgErrorCovariance> {
 public:
  static const std::string classname() {return "qg::QgErrorCovariance";}

  QgErrorCovariance(const QgGeometry &, const VarQG &, const eckit::Configuration &, const QgState &);
  ~QgErrorCovariance();

  void linearize(const QgState &, const QgGeometry &);
  void multiply(const QgIncrement &, QgIncrement &) const;
  void inverseMultiply(const QgIncrement &, QgIncrement &) const;
  void randomize(QgIncrement &) const;

 private:
  void print(std::ostream &) const;
  int keyFtnConfig_;
  boost::scoped_ptr<const QgGeometry> geom_;
  util::DateTime time_;
};
// -----------------------------------------------------------------------------

}  // namespace qg
#endif  // QG_MODEL_QGERRORCOVARIANCE_H_
