/*
 * (C) Copyright 2009-2016 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef QG_MODEL_QGTLM_H_
#define QG_MODEL_QGTLM_H_

#include <map>
#include <ostream>
#include <string>

#include <boost/noncopyable.hpp>
#include <boost/scoped_ptr.hpp>

#include "model/QgFortran.h"
#include "model/QgGeometry.h"
#include "model/QgModel.h"
#include "util/Duration.h"
#include "util/ObjectCounter.h"
#include "util/Printable.h"

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
/// QG linear model definition.
/*!
 *  QG linear model definition and configuration parameters.
 */

class QgTLM: public util::Printable,
             private boost::noncopyable,
             private util::ObjectCounter<QgTLM> {
 public:
  static const std::string classname() {return "qg::QgTLM";}

  QgTLM(const QgGeometry &, const eckit::Configuration &);
  ~QgTLM();

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

 private:
  void print(std::ostream &) const;
  typedef std::map< util::DateTime, int >::iterator trajIter;
  typedef std::map< util::DateTime, int >::const_iterator trajICst;

// Data
  int keyConfig_;
  util::Duration tstep_;
  const QgGeometry resol_;
  std::map< util::DateTime, int > traj_;
  const QgModel lrmodel_;
};
// -----------------------------------------------------------------------------

}  // namespace qg
#endif  // QG_MODEL_QGTLM_H_
