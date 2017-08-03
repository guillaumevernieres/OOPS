/*
 * (C) Copyright 2009-2016 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef LORENZ95_IDTLM_H_
#define LORENZ95_IDTLM_H_

#include <string>

#include <boost/noncopyable.hpp>

#include "util/Duration.h"
#include "util/ObjectCounter.h"

#include "lorenz95/Resolution.h"

// Forward declarations
namespace eckit {
  class Configuration;
}

namespace lorenz95 {
  class ModelBias;
  class ModelBiasCorrection;
  class StateL95;
  class IncrementL95;

// -----------------------------------------------------------------------------
/// Lorenz 95 linear model definition.

class IdTLM: private boost::noncopyable, private util::ObjectCounter<IdTLM> {
 public:
  static const std::string classname() {return "lorenz95::IdTLM";}

  IdTLM(const Resolution &, const eckit::Configuration &);
  ~IdTLM();

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
  friend std::ostream & operator<<(std::ostream &, const IdTLM &);

 private:
  const Resolution resol_;
  const util::Duration tstep_;
};

// -----------------------------------------------------------------------------

}  // namespace lorenz95

#endif  // LORENZ95_IDTLM_H_
