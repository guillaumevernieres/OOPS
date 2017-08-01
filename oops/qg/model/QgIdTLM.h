/*
 * (C) Copyright 2009-2016 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef QG_MODEL_QGIDTLM_H_
#define QG_MODEL_QGIDTLM_H_

#include <string>

#include <boost/noncopyable.hpp>

#include "model/QgGeometry.h"
#include "util/Duration.h"
#include "util/ObjectCounter.h"

// Forward declarations
namespace eckit {
  class Configuration;
}

namespace qg {
  class ModelBias;
  class ModelBiasIncrement;
  class QgIncrement;
  class QgState;

// -----------------------------------------------------------------------------
/// QG identity linear model definition.

class QgIdTLM: private boost::noncopyable, private util::ObjectCounter<QgIdTLM> {
 public:
  static const std::string classname() {return "qg::QgIdTLM";}

  QgIdTLM(const QgGeometry &, const eckit::Configuration &);
  ~QgIdTLM();

/// Model trajectory computation
  void setTrajectory(const QgState &, QgState &, const ModelBias &);

/// Run TLM and its adjoint
  void initializeTL(QgIncrement &) const;
  void stepTL(QgIncrement &, const ModelBiasIncrement &) const;
  void finalizeTL(QgIncrement &) const;

  void initializeAD(QgIncrement &) const;
  void stepAD(QgIncrement &, ModelBiasIncrement &) const;
  void finalizeAD(QgIncrement &) const;

/// Other utilities
  const util::Duration & timeResolution() const {return tstep_;}
  const QgGeometry & resolution() const {return resol_;}
  friend std::ostream & operator<<(std::ostream &, const QgIdTLM &);

 private:
  int keyConfig_;
  util::Duration tstep_;
  const QgGeometry resol_;
};
// -----------------------------------------------------------------------------

}  // namespace qg
#endif  // QG_MODEL_QGIDTLM_H_
