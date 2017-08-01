/*
 * (C) Copyright 2009-2016 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef QG_MODEL_QGMODEL_H_
#define QG_MODEL_QGMODEL_H_

#include <ostream>
#include <string>
#include <boost/noncopyable.hpp>
#include <boost/scoped_ptr.hpp>

#include "model/QgFortran.h"
#include "model/QgGeometry.h"
#include "util/Duration.h"
#include "util/ObjectCounter.h"
#include "util/Printable.h"

// Forward declarations
namespace eckit {
  class Configuration;
}

namespace qg {
  class F90traj;
  class ModelBias;
  class QgFields;
  class QgState;

// -----------------------------------------------------------------------------
/// QG model definition.
/*!
 *  QG nonlinear model definition and configuration parameters.
 */

class QgModel: public util::Printable,
               private boost::noncopyable,
               private util::ObjectCounter<QgModel> {
 public:
  static const std::string classname() {return "qg::QgModel";}

  QgModel(const QgGeometry &, const eckit::Configuration &);
  ~QgModel();

/// Prepare model integration
  void initialize(QgState &) const;

/// Model integration
  void step(QgState &, const ModelBias &) const;
  int saveTrajectory(QgState &, const ModelBias &) const;

/// Finish model integration
  void finalize(QgState &) const;

/// Utilities
  const util::Duration & timeResolution() const {return tstep_;}

 private:
  void print(std::ostream &) const;
  int keyConfig_;
  util::Duration tstep_;
  const QgGeometry geom_;
};
// -----------------------------------------------------------------------------

}  // namespace qg
#endif  // QG_MODEL_QGMODEL_H_
