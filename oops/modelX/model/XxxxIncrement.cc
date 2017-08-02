/*
 * (C) Copyright 2009-2016 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "model/XxxxIncrement.h"

#include <algorithm>
#include <string>

#include "eckit/config/LocalConfiguration.h"
#include "util/Logger.h"
#include "model/XxxxFields.h"
#include "model/XxxxGeometry.h"
#include "model/XxxxState.h"
#include "model/XxxxVariables.h"
#include "util/DateTime.h"
#include "util/Duration.h"

using oops::Log;

namespace xxxx {

// -----------------------------------------------------------------------------
/// Constructor, destructor
// -----------------------------------------------------------------------------
XxxxIncrement::XxxxIncrement(const XxxxGeometry & resol, const XxxxVariables & vars,
                         const util::DateTime & vt)
  : fields_(new XxxxFields(resol, vars, vt)), stash_()
{
  fields_->zero();
  Log::trace() << "XxxxIncrement constructed." << std::endl;
}
// -----------------------------------------------------------------------------
XxxxIncrement::XxxxIncrement(const XxxxGeometry & resol, const XxxxIncrement & other)
  : fields_(new XxxxFields(*other.fields_, resol)), stash_()
{
  Log::trace() << "XxxxIncrement constructed from other." << std::endl;
}
// -----------------------------------------------------------------------------
XxxxIncrement::XxxxIncrement(const XxxxIncrement & other, const bool copy)
  : fields_(new XxxxFields(*other.fields_, copy)), stash_()
{
  Log::trace() << "XxxxIncrement copy-created." << std::endl;
}
// -----------------------------------------------------------------------------
XxxxIncrement::XxxxIncrement(const XxxxIncrement & other)
  : fields_(new XxxxFields(*other.fields_)), stash_()
{
  Log::trace() << "XxxxIncrement copy-created." << std::endl;
}
// -----------------------------------------------------------------------------
XxxxIncrement::~XxxxIncrement() {
  Log::trace() << "XxxxIncrement destructed" << std::endl;
}
// -----------------------------------------------------------------------------
void XxxxIncrement::activateModel() {
// Should get variables from model. YT
  eckit::LocalConfiguration modelvars;
  modelvars.set("variables", "tl");
  XxxxVariables vars(modelvars);
// Should get variables from model. YT
  stash_.reset(new XxxxFields(*fields_, vars));
  swap(fields_, stash_);
  ASSERT(fields_);
  ASSERT(stash_);
  Log::trace() << "XxxxIncrement activated for TLM" << std::endl;
}
// -----------------------------------------------------------------------------
void XxxxIncrement::deactivateModel() {
  swap(fields_, stash_);
  *fields_ = *stash_;
  stash_.reset();
  ASSERT(fields_);
  ASSERT(!stash_);
  Log::trace() << "XxxxIncrement deactivated for TLM" << std::endl;
}
// -----------------------------------------------------------------------------
/// Basic operators
// -----------------------------------------------------------------------------
void XxxxIncrement::diff(const XxxxState & x1, const XxxxState & x2) {
  ASSERT(this->validTime() == x1.validTime());
  ASSERT(this->validTime() == x2.validTime());
  Log::debug() << "XxxxIncrement:diff incr " << *fields_ << std::endl;
  Log::debug() << "XxxxIncrement:diff x1 " << x1.fields() << std::endl;
  Log::debug() << "XxxxIncrement:diff x2 " << x2.fields() << std::endl;
  fields_->diff(x1.fields(), x2.fields());
}
// -----------------------------------------------------------------------------
XxxxIncrement & XxxxIncrement::operator=(const XxxxIncrement & rhs) {
  *fields_ = *rhs.fields_;
  return *this;
}
// -----------------------------------------------------------------------------
XxxxIncrement & XxxxIncrement::operator+=(const XxxxIncrement & dx) {
  ASSERT(this->validTime() == dx.validTime());
  *fields_ += *dx.fields_;
  return *this;
}
// -----------------------------------------------------------------------------
XxxxIncrement & XxxxIncrement::operator-=(const XxxxIncrement & dx) {
  ASSERT(this->validTime() == dx.validTime());
  *fields_ -= *dx.fields_;
  return *this;
}
// -----------------------------------------------------------------------------
XxxxIncrement & XxxxIncrement::operator*=(const double & zz) {
  *fields_ *= zz;
  return *this;
}
// -----------------------------------------------------------------------------
void XxxxIncrement::zero() {
  fields_->zero();
}
// -----------------------------------------------------------------------------
void XxxxIncrement::zero(const util::DateTime & vt) {
  fields_->zero(vt);
}
// -----------------------------------------------------------------------------
void XxxxIncrement::axpy(const double & zz, const XxxxIncrement & dx,
                       const bool check) {
  ASSERT(!check || this->validTime() == dx.validTime());
  fields_->axpy(zz, *dx.fields_);
}
// -----------------------------------------------------------------------------
void XxxxIncrement::accumul(const double & zz, const XxxxState & xx) {
  fields_->axpy(zz, xx.fields());
}
// -----------------------------------------------------------------------------
void XxxxIncrement::schur_product_with(const XxxxIncrement & dx) {
  fields_->schur_product_with(*dx.fields_);
}
// -----------------------------------------------------------------------------
double XxxxIncrement::dot_product_with(const XxxxIncrement & other) const {
  return dot_product(*fields_, *other.fields_);
}
// -----------------------------------------------------------------------------
void XxxxIncrement::random() {
  fields_->random();
}
// -----------------------------------------------------------------------------
/// I/O and diagnostics
// -----------------------------------------------------------------------------
void XxxxIncrement::read(const eckit::Configuration & files) {
  fields_->read(files);
}
// -----------------------------------------------------------------------------
void XxxxIncrement::write(const eckit::Configuration & files) const {
  fields_->write(files);
}
// -----------------------------------------------------------------------------
void XxxxIncrement::print(std::ostream & os) const {
  os << std::endl << "  Valid time: " << validTime();
  os << *fields_;
}
// -----------------------------------------------------------------------------

}  // namespace xxxx
