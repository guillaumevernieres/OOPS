/*
 * (C) Copyright 2009-2016 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef LORENZ95_TLML95_H_
#define LORENZ95_TLML95_H_

#include <map>
#include <ostream>
#include <string>

#include <boost/noncopyable.hpp>

#include "util/Duration.h"
#include "util/ObjectCounter.h"
#include "util/Printable.h"

#include "lorenz95/ModelL95.h"
#include "lorenz95/ModelTrajectory.h"
#include "lorenz95/Resolution.h"

// Forward declarations
namespace eckit {
  class Configuration;
}

namespace util {
  class DateTime;
}

namespace lorenz95 {
  class ModelBias;
  class ModelBiasCorrection;
  class FieldL95;
  class StateL95;
  class IncrementL95;

// -----------------------------------------------------------------------------
/// Lorenz 95 linear model definition.

class TLML95: public util::Printable,
              private boost::noncopyable,
              private util::ObjectCounter<TLML95> {
 public:
  static const std::string classname() {return "lorenz95::TLML95";}

  TLML95(const Resolution &, const eckit::Configuration &);
  ~TLML95();

/// Model trajectory computation
  void setTrajectory(const StateL95 &, StateL95 &, const ModelBias &);

/// Run TLM and its adjoint
  void initializeTL(IncrementL95 &) const;
  void stepTL(IncrementL95 &, const ModelBiasCorrection &) const;
  void finalizeTL(IncrementL95 &) const;

  void initializeAD(IncrementL95 &) const;
  void stepAD(IncrementL95 &, ModelBiasCorrection &) const;
  void finalizeAD(IncrementL95 &) const;

/// Other utilities
  const util::Duration & timeResolution() const {return tstep_;}
  const Resolution & resolution() const {return resol_;}

 private:
  const ModelTrajectory * getTrajectory(const util::DateTime &) const;
  void tendenciesTL(const FieldL95 &, const double &, const FieldL95 &, FieldL95 &) const;
  void tendenciesAD(FieldL95 &, double &, const FieldL95 &, const FieldL95 &) const;
  void print(std::ostream &) const;

  typedef std::map< util::DateTime, ModelTrajectory * >::iterator trajIter;
  typedef std::map< util::DateTime, ModelTrajectory * >::const_iterator trajICst;

// Data
  const Resolution resol_;
  const util::Duration tstep_;
  const double dt_;
  std::map< util::DateTime, ModelTrajectory * > traj_;
  const ModelL95 lrmodel_;
};

// -----------------------------------------------------------------------------

}  // namespace lorenz95

#endif  // LORENZ95_TLML95_H_
