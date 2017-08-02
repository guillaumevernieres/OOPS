/*
 * (C) Copyright 2009-2016 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "model/XxxxFields.h"

#include <cmath>
#include <map>
#include <string>
#include <vector>

#include "util/Logger.h"
#include "model/XxxxFortran.h"
#include "model/XxxxGeometry.h"
#include "model/XxxxVariables.h"
#include "eckit/config/Configuration.h"
#include "util/DateTime.h"

// -----------------------------------------------------------------------------
namespace xxxx {
// -----------------------------------------------------------------------------
XxxxFields::XxxxFields(const XxxxGeometry & geom, const XxxxVariables & vars,
                   const util::DateTime & time):
  geom_(new XxxxGeometry(geom)), vars_(new XxxxVariables(vars)), time_(time)
{
  xxxx_field_create_f90(keyFlds_, geom_->toFortran(), vars_->toFortran());
}
// -----------------------------------------------------------------------------
XxxxFields::XxxxFields(const XxxxFields & other, const bool copy)
  : geom_(other.geom_), vars_(other.vars_), time_(other.time_)
{
  xxxx_field_create_f90(keyFlds_, geom_->toFortran(), vars_->toFortran());
  if (copy) {
    xxxx_field_copy_f90(keyFlds_, other.keyFlds_);
  } else {
    xxxx_field_zero_f90(keyFlds_);
  }
}
// -----------------------------------------------------------------------------
XxxxFields::XxxxFields(const XxxxFields & other)
  : geom_(other.geom_), vars_(other.vars_), time_(other.time_)
{
  xxxx_field_create_f90(keyFlds_, geom_->toFortran(), vars_->toFortran());
  xxxx_field_copy_f90(keyFlds_, other.keyFlds_);
}
// -----------------------------------------------------------------------------
XxxxFields::XxxxFields(const XxxxFields & other, const XxxxGeometry & geom)
  : geom_(new XxxxGeometry(geom)), vars_(other.vars_), time_(other.time_)
{
  xxxx_field_create_f90(keyFlds_, geom_->toFortran(), vars_->toFortran());
  xxxx_field_change_resol_f90(keyFlds_, other.keyFlds_);
}
// -----------------------------------------------------------------------------
XxxxFields::XxxxFields(const XxxxFields & other, const XxxxVariables & vars)
  : geom_(other.geom_), vars_(new XxxxVariables(vars)), time_(other.time_)
{
  xxxx_field_create_f90(keyFlds_, geom_->toFortran(), vars_->toFortran());
  xxxx_field_copy_f90(keyFlds_, other.keyFlds_);
}
// -----------------------------------------------------------------------------
XxxxFields::~XxxxFields() {
  xxxx_field_delete_f90(keyFlds_);
}
// -----------------------------------------------------------------------------
XxxxFields & XxxxFields::operator=(const XxxxFields & rhs) {
  xxxx_field_copy_f90(keyFlds_, rhs.keyFlds_);
  time_ = rhs.time_;
  return *this;
}
// -----------------------------------------------------------------------------
XxxxFields & XxxxFields::operator+=(const XxxxFields & rhs) {
  xxxx_field_self_add_f90(keyFlds_, rhs.keyFlds_);
  return *this;
}
// -----------------------------------------------------------------------------
XxxxFields & XxxxFields::operator-=(const XxxxFields & rhs) {
  xxxx_field_self_sub_f90(keyFlds_, rhs.keyFlds_);
  return *this;
}
// -----------------------------------------------------------------------------
XxxxFields & XxxxFields::operator*=(const double & zz) {
  xxxx_field_self_mul_f90(keyFlds_, zz);
  return *this;
}
// -----------------------------------------------------------------------------
void XxxxFields::zero() {
  xxxx_field_zero_f90(keyFlds_);
}
// -----------------------------------------------------------------------------
void XxxxFields::zero(const util::DateTime & time) {
  xxxx_field_zero_f90(keyFlds_);
  time_ = time;
}
// -----------------------------------------------------------------------------
void XxxxFields::axpy(const double & zz, const XxxxFields & rhs) {
  xxxx_field_axpy_f90(keyFlds_, zz, rhs.keyFlds_);
}
// -----------------------------------------------------------------------------
double XxxxFields::dot_product_with(const XxxxFields & fld2) const {
  double zz;
  xxxx_field_dot_prod_f90(keyFlds_, fld2.keyFlds_, zz);
  return zz;
}
// -----------------------------------------------------------------------------
void XxxxFields::schur_product_with(const XxxxFields & dx) {
    xxxx_field_self_schur_f90(keyFlds_, dx.keyFlds_);
}
// -----------------------------------------------------------------------------
void XxxxFields::random() {
  xxxx_field_random_f90(keyFlds_);
}
// -----------------------------------------------------------------------------
void XxxxFields::changeResolution(const XxxxFields & other) {
  xxxx_field_change_resol_f90(keyFlds_, other.keyFlds_);
}
// -----------------------------------------------------------------------------
void XxxxFields::add(const XxxxFields & rhs) {
  xxxx_field_add_incr_f90(keyFlds_, rhs.keyFlds_);
}
// -----------------------------------------------------------------------------
void XxxxFields::diff(const XxxxFields & x1, const XxxxFields & x2) {
  xxxx_field_diff_incr_f90(keyFlds_, x1.keyFlds_, x2.keyFlds_);
}
// -----------------------------------------------------------------------------
void XxxxFields::read(const eckit::Configuration & config) {
  const eckit::Configuration * conf = &config;
  util::DateTime * dtp = &time_;
  xxxx_field_read_file_f90(keyFlds_, &conf, &dtp);
}
// -----------------------------------------------------------------------------
void XxxxFields::write(const eckit::Configuration & config) const {
  const eckit::Configuration * conf = &config;
  const util::DateTime * dtp = &time_;
  xxxx_field_write_file_f90(keyFlds_, &conf, &dtp);
}
// -----------------------------------------------------------------------------
double XxxxFields::norm() const {
  double zz = 0.0;
  xxxx_field_rms_f90(keyFlds_, zz);
  return zz;
}
// -----------------------------------------------------------------------------
void XxxxFields::print(std::ostream & os) const {
  int nx = -1;
  int ny = -1;
  int nf = -1;
  int nb = -1;
  xxxx_field_sizes_f90(keyFlds_, nx, ny, nf, nb);
  os << std::endl << "  Resolution = " << nx << ", " << ny
     << ", Fields = " << nf << ", " << nb;
  nf += nb;
  std::vector<double> zstat(3*nf);
  xxxx_field_gpnorm_f90(keyFlds_, nf, zstat[0]);
  for (int jj = 0; jj < nf; ++jj) {
    os << std::endl << "  Min=" << zstat[3*jj]
       << ", Max=" << zstat[3*jj+1] << ", RMS=" << zstat[3*jj+2];
  }
}
// -----------------------------------------------------------------------------
}  // namespace xxxx
