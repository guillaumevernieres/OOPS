/*
 * (C) Copyright 2009-2016 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "model/QgState.h"

#include <algorithm>
#include <string>

#include "eckit/config/LocalConfiguration.h"
#include "util/Logger.h"
#include "model/GomQG.h"
#include "model/LocQG.h"
#include "model/ModelBias.h"
#include "model/QgFields.h"
#include "model/QgGeometry.h"
#include "model/QgIncrement.h"
#include "model/QgModel.h"
#include "model/VarQG.h"
#include "util/DateTime.h"
#include "util/Duration.h"

using oops::Log;

namespace qg {

// -----------------------------------------------------------------------------
/// Constructor, destructor
// -----------------------------------------------------------------------------
QgState::QgState(const QgGeometry & resol, const VarQG & vars,
                 const util::DateTime & vt)
  : fields_(new QgFields(resol, vars, vt)), stash_()
{
  Log::trace() << "QgState::QgState created." << std::endl;
}
// -----------------------------------------------------------------------------
QgState::QgState(const QgGeometry & resol, const eckit::Configuration & file)
  : fields_(), stash_()
{
// Should get variables from file. YT
  eckit::LocalConfiguration modelvars;
  modelvars.set("variables", "cv");
  VarQG vars(modelvars);
// Should get variables from file. YT
  fields_.reset(new QgFields(resol, vars, util::DateTime()));
  fields_->read(file);

  ASSERT(fields_);
  Log::trace() << "QgState::QgState created and read in." << std::endl;
}
// -----------------------------------------------------------------------------
QgState::QgState(const QgGeometry & resol, const QgState & other)
  : fields_(new QgFields(*other.fields_, resol)), stash_()
{
  ASSERT(fields_);
  Log::trace() << "QgState::QgState created by interpolation." << std::endl;
}
// -----------------------------------------------------------------------------
QgState::QgState(const QgState & other)
  : fields_(new QgFields(*other.fields_)), stash_()
{
  ASSERT(fields_);
  Log::trace() << "QgState::QgState copied." << std::endl;
}
// -----------------------------------------------------------------------------
QgState::~QgState() {
  Log::trace() << "QgState::QgState destructed." << std::endl;
}
// -----------------------------------------------------------------------------
void QgState::activateModel() {
// Should get variables from model. YT
  eckit::LocalConfiguration modelvars;
  modelvars.set("variables", "nl");
  VarQG vars(modelvars);
// Should get variables from model. YT
  stash_.reset(new QgFields(*fields_, vars));
  swap(fields_, stash_);
  ASSERT(fields_);
  ASSERT(stash_);
  Log::trace() << "QgState activated for Model" << std::endl;
}
// -----------------------------------------------------------------------------
void QgState::deactivateModel() {
  swap(fields_, stash_);
  *fields_ = *stash_;
  stash_.reset();
  ASSERT(fields_);
  ASSERT(!stash_);
  Log::trace() << "QgState deactivated for Model" << std::endl;
}
// -----------------------------------------------------------------------------
/// Basic operators
// -----------------------------------------------------------------------------
QgState & QgState::operator=(const QgState & rhs) {
  ASSERT(fields_);
  *fields_ = *rhs.fields_;
  return *this;
}
// -----------------------------------------------------------------------------
/// Interpolate to observation location
// -----------------------------------------------------------------------------
void QgState::interpolate(const LocQG & locs, GomQG & cols) const {
  fields_->interpolate(locs, cols);
}
// -----------------------------------------------------------------------------
/// Interpolate full fields
// -----------------------------------------------------------------------------
void QgState::changeResolution(const QgState & other) {
  fields_->changeResolution(*other.fields_);
  Log::trace() << "QgState interpolated" << std::endl;
}
// -----------------------------------------------------------------------------
/// Interactions with Increments
// -----------------------------------------------------------------------------
QgState & QgState::operator+=(const QgIncrement & dx) {
  ASSERT(this->validTime() == dx.validTime());
  ASSERT(fields_);
  fields_->add(dx.fields());
  return *this;
}
// -----------------------------------------------------------------------------
/// I/O and diagnostics
// -----------------------------------------------------------------------------
void QgState::read(const eckit::Configuration & files) {
  fields_->read(files);
}
// -----------------------------------------------------------------------------
void QgState::write(const eckit::Configuration & files) const {
  fields_->write(files);
}
// -----------------------------------------------------------------------------
void QgState::print(std::ostream & os) const {
  os << std::endl << "  Valid time: " << validTime();
  os << *fields_;
}
// -----------------------------------------------------------------------------
/// For accumulator
// -----------------------------------------------------------------------------
void QgState::zero() {
  fields_->zero();
}
// -----------------------------------------------------------------------------
void QgState::accumul(const double & zz, const QgState & xx) {
  fields_->axpy(zz, *xx.fields_);
}
// -----------------------------------------------------------------------------

}  // namespace qg
