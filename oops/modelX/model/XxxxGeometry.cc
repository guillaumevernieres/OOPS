/*
 * (C) Copyright 2009-2016 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "util/Logger.h"
#include "model/XxxxGeometry.h"
#include "model/XxxxFortran.h"
#include "eckit/config/Configuration.h"

// -----------------------------------------------------------------------------
namespace xxxx {
// -----------------------------------------------------------------------------
XxxxGeometry::XxxxGeometry(const eckit::Configuration & conf) {
  const eckit::Configuration * configc = &conf;
  xxxx_geo_setup_f90(keyGeom_, &configc);
}
// -----------------------------------------------------------------------------
XxxxGeometry::XxxxGeometry(const XxxxGeometry & other) {
  const int key_geo = other.keyGeom_;
  xxxx_geo_clone_f90(key_geo, keyGeom_);
}
// -----------------------------------------------------------------------------
XxxxGeometry::~XxxxGeometry() {
  xxxx_geo_delete_f90(keyGeom_);
}
// -----------------------------------------------------------------------------
void XxxxGeometry::print(std::ostream & os) const {
  int nx;
  int ny;
  xxxx_geo_info_f90(keyGeom_, nx, ny);
  os << "nx = " << nx << ", ny = " << ny;
}
// -----------------------------------------------------------------------------
}  // namespace xxxx
