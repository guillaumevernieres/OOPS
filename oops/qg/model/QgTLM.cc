/*
 * (C) Copyright 2009-2016 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <vector>

#include "model/QgTLM.h"

#include "eckit/config/LocalConfiguration.h"
#include "util/Logger.h"
#include "model/ModelBiasIncrement.h"
#include "model/QgFortran.h"
#include "model/QgGeometry.h"
#include "model/QgIncrement.h"
#include "model/QgModel.h"
#include "model/QgState.h"
#include "util/DateTime.h"
#include "util/abor1_cpp.h"

using oops::Log;


namespace qg {
// -----------------------------------------------------------------------------
QgTLM::QgTLM(const QgGeometry & resol, const eckit::Configuration & tlConf)
  : keyConfig_(0), tstep_(), resol_(resol), traj_(),
    lrmodel_(resol_, eckit::LocalConfiguration(tlConf, "trajectory"))
{
  tstep_ = util::Duration(tlConf.getString("tstep"));

  const eckit::Configuration * configc = &tlConf;
  qg_setup_f90(&configc, resol_.toFortran(), keyConfig_);

  Log::trace() << "QgTLM created" << std::endl;
}
// -----------------------------------------------------------------------------
QgTLM::~QgTLM() {
  qg_delete_f90(keyConfig_);
  for (trajIter jtra = traj_.begin(); jtra != traj_.end(); ++jtra) {
    qg_wipe_traj_f90(jtra->second);
  }
  Log::trace() << "QgTLM destructed" << std::endl;
}
// -----------------------------------------------------------------------------
void QgTLM::setTrajectory(const QgState & xx, QgState & xlr, const ModelBias & bias) {
  //QgState xlr(resol_, xx);
  xlr.changeResolution(xx);
  int ftraj = lrmodel_.saveTrajectory(xlr, bias);
  traj_[xx.validTime()] = ftraj;

// should be in print method
  std::vector<double> zstat(15);
  qg_traj_minmaxrms_f90(ftraj, zstat[0]);
  Log::debug() << "QgTLM trajectory at time " << xx.validTime() << std::endl;
  for (unsigned int jj = 0; jj < 5; ++jj) {
    Log::debug() << "  Min=" << zstat[3*jj] << ", Max=" << zstat[3*jj+1]
                 << ", RMS=" << zstat[3*jj+2] << std::endl;
  }
// should be in print method
}
// -----------------------------------------------------------------------------
void QgTLM::initializeTL(QgIncrement & dx) const {
  dx.activateModel();
  ASSERT(dx.fields().isForModel(false));
  qg_prepare_integration_tl_f90(keyConfig_, dx.fields().toFortran());
  Log::debug() << "QgTLM::initializeTL" << dx.fields() << std::endl;
}
// -----------------------------------------------------------------------------
void QgTLM::stepTL(QgIncrement & dx, const ModelBiasIncrement &) const {
  trajICst itra = traj_.find(dx.validTime());
  if (itra == traj_.end()) {
    Log::error() << "QgTLM: trajectory not available at time " << dx.validTime() << std::endl;
    ABORT("QgTLM: trajectory not available");
  }
  ASSERT(dx.fields().isForModel(false));
  Log::debug() << "QgTLM::stepTL fields in" << dx.fields() << std::endl;
  qg_propagate_tl_f90(keyConfig_, dx.fields().toFortran(), itra->second);
  Log::debug() << "QgTLM::stepTL fields out" << dx.fields() << std::endl;
  dx.validTime() += tstep_;
}
// -----------------------------------------------------------------------------
void QgTLM::finalizeTL(QgIncrement & dx) const {
  dx.deactivateModel();
  Log::debug() << "QgTLM::finalizeTL" << dx.fields() << std::endl;
}
// -----------------------------------------------------------------------------
void QgTLM::initializeAD(QgIncrement & dx) const {
  dx.activateModel();
  ASSERT(dx.fields().isForModel(false));
  Log::debug() << "QgTLM::initializeAD" << dx.fields() << std::endl;
}
// -----------------------------------------------------------------------------
void QgTLM::stepAD(QgIncrement & dx, ModelBiasIncrement &) const {
  dx.validTime() -= tstep_;
  trajICst itra = traj_.find(dx.validTime());
  if (itra == traj_.end()) {
    Log::error() << "QgTLM: trajectory not available at time " << dx.validTime() << std::endl;
    ABORT("QgTLM: trajectory not available");
  }
  ASSERT(dx.fields().isForModel(false));
  Log::debug() << "QgTLM::stepAD fields in" << dx.fields() << std::endl;
  qg_propagate_ad_f90(keyConfig_, dx.fields().toFortran(), itra->second);
  Log::debug() << "QgTLM::stepAD fields out" << dx.fields() << std::endl;
}
// -----------------------------------------------------------------------------
void QgTLM::finalizeAD(QgIncrement & dx) const {
  qg_prepare_integration_ad_f90(keyConfig_, dx.fields().toFortran());
  dx.deactivateModel();
  Log::debug() << "QgTLM::finalizeAD" << dx.fields() << std::endl;
}
// -----------------------------------------------------------------------------
void QgTLM::print(std::ostream & os) const {
  os << "QG TLM Trajectory, nstep=" << traj_.size() << std::endl;
  typedef std::map< util::DateTime, int >::const_iterator trajICst;
  if (traj_.size() > 0) {
    os << "QG TLM Trajectory: times are:";
    for (trajICst jtra = traj_.begin(); jtra != traj_.end(); ++jtra) {
      os << "  " << jtra->first;
    }
  }
}
// -----------------------------------------------------------------------------
}  // namespace qg
