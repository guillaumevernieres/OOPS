/*
 * (C) Copyright 2009-2016 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "model/QgFields.h"

#include <cmath>
#include <map>
#include <string>
#include <vector>

#include "util/Logger.h"
#include "model/GomQG.h"
#include "model/LocQG.h"
#include "model/QgFortran.h"
#include "model/QgGeometry.h"
#include "model/VarQG.h"
#include "eckit/config/Configuration.h"
#include "util/DateTime.h"

// -----------------------------------------------------------------------------
namespace qg {
// -----------------------------------------------------------------------------
QgFields::QgFields(const QgGeometry & geom, const VarQG & vars,
                   const util::DateTime & time):
  geom_(new QgGeometry(geom)), vars_(new VarQG(vars)), time_(time)
{
  qg_field_create_f90(keyFlds_, geom_->toFortran(), vars_->toFortran());
}
// -----------------------------------------------------------------------------
QgFields::QgFields(const QgFields & other, const bool copy)
  : geom_(other.geom_), vars_(other.vars_), time_(other.time_)
{
  qg_field_create_f90(keyFlds_, geom_->toFortran(), vars_->toFortran());
  if (copy) {
    qg_field_copy_f90(keyFlds_, other.keyFlds_);
  } else {
    qg_field_zero_f90(keyFlds_);
  }
}
// -----------------------------------------------------------------------------
QgFields::QgFields(const QgFields & other)
  : geom_(other.geom_), vars_(other.vars_), time_(other.time_)
{
  qg_field_create_f90(keyFlds_, geom_->toFortran(), vars_->toFortran());
  qg_field_copy_f90(keyFlds_, other.keyFlds_);
}
// -----------------------------------------------------------------------------
QgFields::QgFields(const QgFields & other, const QgGeometry & geom)
  : geom_(new QgGeometry(geom)), vars_(other.vars_), time_(other.time_)
{
  qg_field_create_f90(keyFlds_, geom_->toFortran(), vars_->toFortran());
  qg_field_change_resol_f90(keyFlds_, other.keyFlds_);
}
// -----------------------------------------------------------------------------
QgFields::QgFields(const QgFields & other, const VarQG & vars)
  : geom_(other.geom_), vars_(new VarQG(vars)), time_(other.time_)
{
  qg_field_create_f90(keyFlds_, geom_->toFortran(), vars_->toFortran());
  qg_field_copy_f90(keyFlds_, other.keyFlds_);
}
// -----------------------------------------------------------------------------
QgFields::~QgFields() {
  qg_field_delete_f90(keyFlds_);
}
// -----------------------------------------------------------------------------
QgFields & QgFields::operator=(const QgFields & rhs) {
  qg_field_copy_f90(keyFlds_, rhs.keyFlds_);
  time_ = rhs.time_;
  return *this;
}
// -----------------------------------------------------------------------------
QgFields & QgFields::operator+=(const QgFields & rhs) {
  qg_field_self_add_f90(keyFlds_, rhs.keyFlds_);
  return *this;
}
// -----------------------------------------------------------------------------
QgFields & QgFields::operator-=(const QgFields & rhs) {
  qg_field_self_sub_f90(keyFlds_, rhs.keyFlds_);
  return *this;
}
// -----------------------------------------------------------------------------
QgFields & QgFields::operator*=(const double & zz) {
  qg_field_self_mul_f90(keyFlds_, zz);
  return *this;
}
// -----------------------------------------------------------------------------
void QgFields::zero() {
  qg_field_zero_f90(keyFlds_);
}
// -----------------------------------------------------------------------------
void QgFields::zero(const util::DateTime & time) {
  qg_field_zero_f90(keyFlds_);
  time_ = time;
}
// -----------------------------------------------------------------------------
void QgFields::axpy(const double & zz, const QgFields & rhs) {
  qg_field_axpy_f90(keyFlds_, zz, rhs.keyFlds_);
}
// -----------------------------------------------------------------------------
double QgFields::dot_product_with(const QgFields & fld2) const {
  double zz;
  qg_field_dot_prod_f90(keyFlds_, fld2.keyFlds_, zz);
  return zz;
}
// -----------------------------------------------------------------------------
void QgFields::schur_product_with(const QgFields & dx) {
    qg_field_self_schur_f90(keyFlds_, dx.keyFlds_);
}
// -----------------------------------------------------------------------------
void QgFields::random() {
  qg_field_random_f90(keyFlds_);
}
// -----------------------------------------------------------------------------
void QgFields::interpolate(const LocQG & locs, GomQG & gom) const {
  qg_field_interp_tl_f90(keyFlds_, locs.toFortran(), gom.toFortran());
}
// -----------------------------------------------------------------------------
void QgFields::interpolateTL(const LocQG & locs, GomQG & gom) const {
  qg_field_interp_tl_f90(keyFlds_, locs.toFortran(), gom.toFortran());
}
// -----------------------------------------------------------------------------
void QgFields::interpolateAD(const LocQG & locs, const GomQG & gom) {
  qg_field_interp_ad_f90(keyFlds_, locs.toFortran(), gom.toFortran());
}
// -----------------------------------------------------------------------------
void QgFields::changeResolution(const QgFields & other) {
  qg_field_change_resol_f90(keyFlds_, other.keyFlds_);
}
// -----------------------------------------------------------------------------
void QgFields::add(const QgFields & rhs) {
  qg_field_add_incr_f90(keyFlds_, rhs.keyFlds_);
}
// -----------------------------------------------------------------------------
void QgFields::diff(const QgFields & x1, const QgFields & x2) {
  qg_field_diff_incr_f90(keyFlds_, x1.keyFlds_, x2.keyFlds_);
}
// -----------------------------------------------------------------------------
void QgFields::read(const eckit::Configuration & config) {
  const eckit::Configuration * conf = &config;
  util::DateTime * dtp = &time_;
  qg_field_read_file_f90(keyFlds_, &conf, &dtp);
}
// -----------------------------------------------------------------------------
void QgFields::write(const eckit::Configuration & config) const {
  const eckit::Configuration * conf = &config;
  const util::DateTime * dtp = &time_;
  qg_field_write_file_f90(keyFlds_, &conf, &dtp);
}
// -----------------------------------------------------------------------------
double QgFields::norm() const {
  double zz = 0.0;
  qg_field_rms_f90(keyFlds_, zz);
  return zz;
}
// -----------------------------------------------------------------------------
void QgFields::print(std::ostream & os) const {
  int nx = -1;
  int ny = -1;
  int nf = -1;
  int nb = -1;
  qg_field_sizes_f90(keyFlds_, nx, ny, nf, nb);
  os << std::endl << "  Resolution = " << nx << ", " << ny
     << ", Fields = " << nf << ", " << nb;
  nf += nb;
  std::vector<double> zstat(3*nf);
  qg_field_gpnorm_f90(keyFlds_, nf, zstat[0]);
  for (int jj = 0; jj < nf; ++jj) {
    os << std::endl << "  Min=" << zstat[3*jj]
       << ", Max=" << zstat[3*jj+1] << ", RMS=" << zstat[3*jj+2];
  }
}
// -----------------------------------------------------------------------------
bool QgFields::isForModel(bool nonlinear) const {
  int nx = -1;
  int ny = -1;
  int nf = -1;
  int nb = -1;
  qg_field_sizes_f90(keyFlds_, nx, ny, nf, nb);
  bool ok = (nf == 4);
  if (nonlinear) ok = ok && (nb == 2);
  return ok;
}
// -----------------------------------------------------------------------------
}  // namespace qg
