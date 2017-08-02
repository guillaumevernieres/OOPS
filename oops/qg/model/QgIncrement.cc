/*
 * (C) Copyright 2009-2016 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "model/QgIncrement.h"

#include <algorithm>
#include <string>

#include "eckit/config/LocalConfiguration.h"
#include "util/Logger.h"
#include "model/GomQG.h"
#include "model/LocQG.h"
#include "model/ModelBiasIncrement.h"
#include "model/QgErrorCovariance.h"
#include "model/QgFields.h"
#include "model/QgGeometry.h"
#include "model/QgState.h"
#include "model/VarQG.h"
#include "util/DateTime.h"
#include "util/Duration.h"

using oops::Log;


namespace qg {

// -----------------------------------------------------------------------------
/// Constructor, destructor
// -----------------------------------------------------------------------------
QgIncrement::QgIncrement(const QgGeometry & resol, const VarQG & vars,
                         const util::DateTime & vt)
  : fields_(new QgFields(resol, vars, vt)), stash_()
{
  fields_->zero();
  Log::trace() << "QgIncrement constructed." << std::endl;
}
// -----------------------------------------------------------------------------
QgIncrement::QgIncrement(const QgGeometry & resol, const QgIncrement & other)
  : fields_(new QgFields(*other.fields_, resol)), stash_()
{
  Log::trace() << "QgIncrement constructed from other." << std::endl;
}
// -----------------------------------------------------------------------------
QgIncrement::QgIncrement(const QgIncrement & other, const bool copy)
  : fields_(new QgFields(*other.fields_, copy)), stash_()
{
  Log::trace() << "QgIncrement copy-created." << std::endl;
}
// -----------------------------------------------------------------------------
QgIncrement::QgIncrement(const QgIncrement & other)
  : fields_(new QgFields(*other.fields_)), stash_()
{
  Log::trace() << "QgIncrement copy-created." << std::endl;
}
// -----------------------------------------------------------------------------
QgIncrement::~QgIncrement() {
  Log::trace() << "QgIncrement destructed" << std::endl;
}
// -----------------------------------------------------------------------------
void QgIncrement::activateModel() {
// Should get variables from model. YT
  eckit::LocalConfiguration modelvars;
  modelvars.set("variables", "tl");
  VarQG vars(modelvars);
// Should get variables from model. YT
  stash_.reset(new QgFields(*fields_, vars));
  swap(fields_, stash_);
  ASSERT(fields_);
  ASSERT(stash_);
  Log::trace() << "QgIncrement activated for TLM" << std::endl;
}
// -----------------------------------------------------------------------------
void QgIncrement::deactivateModel() {
  swap(fields_, stash_);
  *fields_ = *stash_;
  stash_.reset();
  ASSERT(fields_);
  ASSERT(!stash_);
  Log::trace() << "QgIncrement deactivated for TLM" << std::endl;
}
// -----------------------------------------------------------------------------
/// Basic operators
// -----------------------------------------------------------------------------
void QgIncrement::diff(const QgState & x1, const QgState & x2) {
  ASSERT(this->validTime() == x1.validTime());
  ASSERT(this->validTime() == x2.validTime());
  Log::debug() << "QgIncrement:diff incr " << *fields_ << std::endl;
  Log::debug() << "QgIncrement:diff x1 " << x1.fields() << std::endl;
  Log::debug() << "QgIncrement:diff x2 " << x2.fields() << std::endl;
  fields_->diff(x1.fields(), x2.fields());
}
// -----------------------------------------------------------------------------
QgIncrement & QgIncrement::operator=(const QgIncrement & rhs) {
  *fields_ = *rhs.fields_;
  return *this;
}
// -----------------------------------------------------------------------------
QgIncrement & QgIncrement::operator+=(const QgIncrement & dx) {
  ASSERT(this->validTime() == dx.validTime());
  *fields_ += *dx.fields_;
  return *this;
}
// -----------------------------------------------------------------------------
QgIncrement & QgIncrement::operator-=(const QgIncrement & dx) {
  ASSERT(this->validTime() == dx.validTime());
  *fields_ -= *dx.fields_;
  return *this;
}
// -----------------------------------------------------------------------------
QgIncrement & QgIncrement::operator*=(const double & zz) {
  *fields_ *= zz;
  return *this;
}
// -----------------------------------------------------------------------------
void QgIncrement::zero() {
  fields_->zero();
}
// -----------------------------------------------------------------------------
void QgIncrement::zero(const util::DateTime & vt) {
  fields_->zero(vt);
}
// -----------------------------------------------------------------------------
void QgIncrement::axpy(const double & zz, const QgIncrement & dx,
                       const bool check) {
  ASSERT(!check || this->validTime() == dx.validTime());
  fields_->axpy(zz, *dx.fields_);
}
// -----------------------------------------------------------------------------
void QgIncrement::accumul(const double & zz, const QgState & xx) {
  fields_->axpy(zz, xx.fields());
}
// -----------------------------------------------------------------------------
void QgIncrement::schur_product_with(const QgIncrement & dx) {
  fields_->schur_product_with(*dx.fields_);
}
// -----------------------------------------------------------------------------
double QgIncrement::dot_product_with(const QgIncrement & other) const {
  return dot_product(*fields_, *other.fields_);
}
// -----------------------------------------------------------------------------
void QgIncrement::random() {
  fields_->random();
}
// -----------------------------------------------------------------------------
/// Interpolate to observation location
// -----------------------------------------------------------------------------
void QgIncrement::interpolateTL(const LocQG & locs, GomQG & cols) const {
  Log::debug() << "QgIncrement::interpolateTL fields in" << *fields_ << std::endl;
  fields_->interpolateTL(locs, cols);
  Log::debug() << "QgIncrement::interpolateTL fields out" << *fields_ << std::endl;
  Log::debug() << "QgIncrement::interpolateTL gom " << cols << std::endl;
}
// -----------------------------------------------------------------------------
void QgIncrement::interpolateAD(const LocQG & locs, const GomQG & cols) {
  Log::debug() << "QgIncrement::interpolateAD gom " << cols << std::endl;
  Log::debug() << "QgIncrement::interpolateAD fields in" << *fields_ << std::endl;
  fields_->interpolateAD(locs, cols);
  Log::debug() << "QgIncrement::interpolateAD fields out" << *fields_ << std::endl;
}
// -----------------------------------------------------------------------------
/// I/O and diagnostics
// -----------------------------------------------------------------------------
void QgIncrement::read(const eckit::Configuration & files) {
  fields_->read(files);
}
// -----------------------------------------------------------------------------
void QgIncrement::write(const eckit::Configuration & files) const {
  fields_->write(files);
}
// -----------------------------------------------------------------------------
void QgIncrement::print(std::ostream & os) const {
  os << std::endl << "  Valid time: " << validTime();
  os << *fields_;
}
// -----------------------------------------------------------------------------

}  // namespace qg
