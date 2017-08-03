/*
 * (C) Copyright 2009-2016 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "lorenz95/IdTLM.h"

#include "lorenz95/IncrementL95.h"
#include "lorenz95/ModelBias.h"
#include "lorenz95/ModelBiasCorrection.h"
#include "lorenz95/Resolution.h"
#include "lorenz95/StateL95.h"
#include "util/Duration.h"

namespace lorenz95 {
// -----------------------------------------------------------------------------
IdTLM::IdTLM(const Resolution & resol, const eckit::Configuration & tlConf)
  : resol_(resol), tstep_(util::Duration(tlConf.getString("tstep")))
{}
// -----------------------------------------------------------------------------
IdTLM::~IdTLM() {}
// -----------------------------------------------------------------------------
/// Don't run TLM or its adjoint
// -----------------------------------------------------------------------------
void IdTLM::setTrajectory(const StateL95 &, StateL95 &, const ModelBias &) {}
void IdTLM::initializeTL(IncrementL95 &) const {}
void IdTLM::finalizeTL(IncrementL95 &) const {}
void IdTLM::initializeAD(IncrementL95 &) const {}
void IdTLM::finalizeAD(IncrementL95 &) const {}
// -----------------------------------------------------------------------------
void IdTLM::stepTL(IncrementL95 & dx, const ModelBiasCorrection &) const {
  dx.updateTime(tstep_);
}
// -----------------------------------------------------------------------------
void IdTLM::stepAD(IncrementL95 & dx, ModelBiasCorrection &) const {
  dx.updateTime(-tstep_);
}
// -----------------------------------------------------------------------------
std::ostream & operator<<(std::ostream & outs, const IdTLM &) {
  outs << "L95 Identity Linear Model" << std::endl;
  return outs;
}
// -----------------------------------------------------------------------------
}  // namespace lorenz95
