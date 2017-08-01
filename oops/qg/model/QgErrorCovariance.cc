/*
 * (C) Copyright 2009-2016 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "model/QgErrorCovariance.h"

#include <cmath>

#include "util/Logger.h"
#include "model/QgFields.h"
#include "model/QgFortran.h"
#include "model/QgGeometry.h"
#include "model/QgIncrement.h"
#include "model/QgState.h"
#include "eckit/config/Configuration.h"


using oops::Log;

// -----------------------------------------------------------------------------
namespace qg {
// -----------------------------------------------------------------------------

QgErrorCovariance::QgErrorCovariance(const QgGeometry & resol, const VarQG &,
                                     const eckit::Configuration & conf, const QgState &) {
  time_ = util::DateTime(conf.getString("date"));
  const eckit::Configuration * configc = &conf;
  qg_b_setup_f90(keyFtnConfig_, &configc, resol.toFortran());
  Log::trace() << "QgErrorCovariance created" << std::endl;
}

// -----------------------------------------------------------------------------

QgErrorCovariance::~QgErrorCovariance() {
  qg_b_delete_f90(keyFtnConfig_);
  Log::trace() << "QgErrorCovariance destructed" << std::endl;
}

// -----------------------------------------------------------------------------

void QgErrorCovariance::linearize(const QgState &, const QgGeometry & resol) {
  geom_.reset(new QgGeometry(resol));
}

// -----------------------------------------------------------------------------

void QgErrorCovariance::multiply(const QgIncrement & dxin, QgIncrement & dxout) const {
  qg_b_mult_f90(keyFtnConfig_, dxin.fields().toFortran(),
                            dxout.fields().toFortran());
}

// -----------------------------------------------------------------------------

void QgErrorCovariance::inverseMultiply(const QgIncrement & dxin, QgIncrement & dxout) const {
  qg_b_invmult_f90(keyFtnConfig_, dxin.fields().toFortran(),
                               dxout.fields().toFortran());
}

// -----------------------------------------------------------------------------

void QgErrorCovariance::randomize(QgIncrement & dx) const {
  qg_b_randomize_f90(keyFtnConfig_, dx.fields().toFortran());
}

// -----------------------------------------------------------------------------

void QgErrorCovariance::print(std::ostream & os) const {
  os << "QgErrorCovariance::print not implemented";
}

// -----------------------------------------------------------------------------

}  // namespace qg
