/*
 * (C) Copyright 2009-2016 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "model/XxxxState.h"

#include <algorithm>
#include <string>

#include "eckit/config/LocalConfiguration.h"
#include "util/Logger.h"
#include "model/XxxxFields.h"
#include "model/XxxxGeometry.h"
#include "model/XxxxIncrement.h"
#include "model/XxxxVariables.h"
#include "util/DateTime.h"
#include "util/Duration.h"

using oops::Log;

namespace xxxx {

// -----------------------------------------------------------------------------
/// Constructor, destructor
// -----------------------------------------------------------------------------
XxxxState::XxxxState(const XxxxGeometry & resol, const XxxxVariables & vars,
                 const util::DateTime & vt)
  : fields_(new XxxxFields(resol, vars, vt)), stash_()
{
  Log::trace() << "XxxxState::XxxxState created." << std::endl;
}
// -----------------------------------------------------------------------------
XxxxState::XxxxState(const XxxxGeometry & resol, const eckit::Configuration & file)
  : fields_(), stash_()
{
// Should get variables from file. YT
  eckit::LocalConfiguration modelvars;
  modelvars.set("variables", "cv");
  XxxxVariables vars(modelvars);
// Should get variables from file. YT
  fields_.reset(new XxxxFields(resol, vars, util::DateTime()));
  fields_->read(file);

  ASSERT(fields_);
  Log::trace() << "XxxxState::XxxxState created and read in." << std::endl;
}
// -----------------------------------------------------------------------------
XxxxState::XxxxState(const XxxxGeometry & resol, const XxxxState & other)
  : fields_(new XxxxFields(*other.fields_, resol)), stash_()
{
  ASSERT(fields_);
  Log::trace() << "XxxxState::XxxxState created by interpolation." << std::endl;
}
// -----------------------------------------------------------------------------
XxxxState::XxxxState(const XxxxState & other)
  : fields_(new XxxxFields(*other.fields_)), stash_()
{
  ASSERT(fields_);
  Log::trace() << "XxxxState::XxxxState copied." << std::endl;
}
// -----------------------------------------------------------------------------
XxxxState::~XxxxState() {
  Log::trace() << "XxxxState::XxxxState destructed." << std::endl;
}
// -----------------------------------------------------------------------------
void XxxxState::activateModel() {
// Should get variables from model. YT
  eckit::LocalConfiguration modelvars;
  modelvars.set("variables", "nl");
  XxxxVariables vars(modelvars);
// Should get variables from model. YT
  stash_.reset(new XxxxFields(*fields_, vars));
  swap(fields_, stash_);
  ASSERT(fields_);
  ASSERT(stash_);
  Log::trace() << "XxxxState activated for Model" << std::endl;
}
// -----------------------------------------------------------------------------
void XxxxState::deactivateModel() {
  swap(fields_, stash_);
  *fields_ = *stash_;
  stash_.reset();
  ASSERT(fields_);
  ASSERT(!stash_);
  Log::trace() << "XxxxState deactivated for Model" << std::endl;
}
// -----------------------------------------------------------------------------
/// Basic operators
// -----------------------------------------------------------------------------
XxxxState & XxxxState::operator=(const XxxxState & rhs) {
  ASSERT(fields_);
  *fields_ = *rhs.fields_;
  return *this;
}
// -----------------------------------------------------------------------------
/// Interpolate full fields
// -----------------------------------------------------------------------------
void XxxxState::changeResolution(const XxxxState & other) {
  fields_->changeResolution(*other.fields_);
  Log::trace() << "XxxxState interpolated" << std::endl;
}
// -----------------------------------------------------------------------------
/// Interactions with Increments
// -----------------------------------------------------------------------------
XxxxState & XxxxState::operator+=(const XxxxIncrement & dx) {
  ASSERT(this->validTime() == dx.validTime());
  ASSERT(fields_);
  fields_->add(dx.fields());
  return *this;
}
// -----------------------------------------------------------------------------
/// I/O and diagnostics
// -----------------------------------------------------------------------------
void XxxxState::read(const eckit::Configuration & files) {
  fields_->read(files);
}
// -----------------------------------------------------------------------------
void XxxxState::write(const eckit::Configuration & files) const {
  fields_->write(files);
}
// -----------------------------------------------------------------------------
void XxxxState::print(std::ostream & os) const {
  os << std::endl << "  Valid time: " << validTime();
  os << *fields_;
}
// -----------------------------------------------------------------------------
/// For accumulator
// -----------------------------------------------------------------------------
void XxxxState::zero() {
  fields_->zero();
}
// -----------------------------------------------------------------------------
void XxxxState::accumul(const double & zz, const XxxxState & xx) {
  fields_->axpy(zz, *xx.fields_);
}
// -----------------------------------------------------------------------------

}  // namespace xxxx
