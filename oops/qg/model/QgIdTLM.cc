/*
 * (C) Copyright 2009-2016 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "model/QgIdTLM.h"

#include "eckit/config/LocalConfiguration.h"
#include "util/Logger.h"
#include "model/ModelBiasIncrement.h"
#include "model/QgFortran.h"
#include "model/QgGeometry.h"
#include "model/QgIncrement.h"
#include "model/QgState.h"
#include "util/DateTime.h"
#include "util/abor1_cpp.h"

using oops::Log;

namespace qg {
// -----------------------------------------------------------------------------
QgIdTLM::QgIdTLM(const QgGeometry & resol, const eckit::Configuration & tlConf)
  : keyConfig_(0), tstep_(), resol_(resol)
{
  tstep_ = util::Duration(tlConf.getString("tstep"));

  const eckit::Configuration * configc = &tlConf;
  qg_setup_f90(&configc, resol_.toFortran(), keyConfig_);

  Log::trace() << "QgIdTLM created" << std::endl;
}
// -----------------------------------------------------------------------------
QgIdTLM::~QgIdTLM() {
  qg_delete_f90(keyConfig_);
  Log::trace() << "QgIdTLM destructed" << std::endl;
}
// -----------------------------------------------------------------------------
void QgIdTLM::setTrajectory(const QgState &, QgState &, const ModelBias &) {}
// -----------------------------------------------------------------------------
void QgIdTLM::initializeTL(QgIncrement & dx) const {
  dx.activateModel();
  ASSERT(dx.fields().isForModel(false));
  qg_prepare_integration_tl_f90(keyConfig_, dx.fields().toFortran());
  Log::debug() << "QgIdTLM::initializeTL" << dx.fields() << std::endl;
}
// -----------------------------------------------------------------------------
void QgIdTLM::stepTL(QgIncrement & dx, const ModelBiasIncrement &) const {
  dx.updateTime(tstep_);
}
// -----------------------------------------------------------------------------
void QgIdTLM::finalizeTL(QgIncrement & dx) const {
  dx.deactivateModel();
  Log::debug() << "QgIdTLM::finalizeTL" << dx.fields() << std::endl;
}
// -----------------------------------------------------------------------------
void QgIdTLM::initializeAD(QgIncrement & dx) const {
  dx.activateModel();
  ASSERT(dx.fields().isForModel(false));
  Log::debug() << "QgIdTLM::initializeAD" << dx.fields() << std::endl;
}
// -----------------------------------------------------------------------------
void QgIdTLM::stepAD(QgIncrement & dx, ModelBiasIncrement &) const {
  dx.updateTime(-tstep_);
}
// -----------------------------------------------------------------------------
void QgIdTLM::finalizeAD(QgIncrement & dx) const {
  qg_prepare_integration_ad_f90(keyConfig_, dx.fields().toFortran());
  dx.deactivateModel();
  Log::debug() << "QgIdTLM::finalizeAD" << dx.fields() << std::endl;
}
// -----------------------------------------------------------------------------
void QgIdTLM::print(std::ostream & os) const {
  os << "QG IdTLM" << std::endl;
}
// -----------------------------------------------------------------------------
}  // namespace qg
