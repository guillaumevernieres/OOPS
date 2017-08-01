/*
 * (C) Copyright 2009-2016 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "model/QgModel.h"

#include "util/Logger.h"
#include "model/ModelBias.h"
#include "model/QgFields.h"
#include "model/QgFortran.h"
#include "model/QgGeometry.h"
#include "model/QgState.h"
#include "eckit/config/Configuration.h"
#include "util/DateTime.h"

using oops::Log;

namespace qg {
// -----------------------------------------------------------------------------
QgModel::QgModel(const QgGeometry & resol, const eckit::Configuration & model)
  : keyConfig_(0), tstep_(0), geom_(resol)
{
  Log::trace() << "QgModel::QgModel" << std::endl;
  tstep_ = util::Duration(model.getString("tstep"));
  const eckit::Configuration * configc = &model;
  qg_setup_f90(&configc, geom_.toFortran(), keyConfig_);
  Log::trace() << "QgModel created" << std::endl;
}
// -----------------------------------------------------------------------------
QgModel::~QgModel() {
  qg_delete_f90(keyConfig_);
  Log::trace() << "QgModel destructed" << std::endl;
}
// -----------------------------------------------------------------------------
void QgModel::initialize(QgState & xx) const {
  xx.activateModel();
  ASSERT(xx.fields().isForModel(true));
  qg_prepare_integration_f90(keyConfig_, xx.fields().toFortran());
  Log::debug() << "QgModel::initialize" << xx.fields() << std::endl;
}
// -----------------------------------------------------------------------------
void QgModel::step(QgState & xx, const ModelBias &) const {
  ASSERT(xx.fields().isForModel(true));
  Log::debug() << "QgModel::step fields in" << xx.fields() << std::endl;
  qg_propagate_f90(keyConfig_, xx.fields().toFortran());
  xx.validTime() += tstep_;
  Log::debug() << "QgModel::step fields out" << xx.fields() << std::endl;
}
// -----------------------------------------------------------------------------
void QgModel::finalize(QgState & xx) const {
  xx.deactivateModel();
  Log::debug() << "QgModel::finalize" << xx.fields() << std::endl;
}
// -----------------------------------------------------------------------------
int QgModel::saveTrajectory(QgState & xx, const ModelBias &) const {
  //ASSERT(xx.fields().isForModel(true));
  int ftraj = 0;
  Log::debug() << "QgModel::saveTrajectory fields in" << xx.fields() << std::endl;
  qg_prop_traj_f90(keyConfig_, xx.fields().toFortran(), ftraj);
//  xx.validTime() += tstep_;
  ASSERT(ftraj != 0);
  Log::debug() << "QgModel::saveTrajectory fields out" << xx.fields() << std::endl;
  return ftraj;
}
// -----------------------------------------------------------------------------
void QgModel::print(std::ostream & os) const {
  os << "QgModel::print not implemented";
}
// -----------------------------------------------------------------------------
}  // namespace qg
