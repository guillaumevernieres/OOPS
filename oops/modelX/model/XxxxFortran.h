/*
 * (C) Copyright 2009-2016 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef XXXX_MODEL_XXXXFORTRAN_H_
#define XXXX_MODEL_XXXXFORTRAN_H_

// Forward declarations
namespace eckit {
  class Configuration;
}

namespace util {
  class DateTime;
  class Duration;
}

namespace xxxx {

/// Interface to Fortran XXXX model
/*!
 * The core of the XXXX model is coded in Fortran.
 * Here we define the interfaces to the Fortran code.
 */

extern "C" {
// -----------------------------------------------------------------------------
//  Geometry
// -----------------------------------------------------------------------------
  void xxxx_geo_setup_f90(int & keyGeom, const eckit::Configuration * const *);
  void xxxx_geo_clone_f90(const int & keyGeom, int & keyGeom_other);
  void xxxx_geo_info_f90(const int & keyGeom, int &, int &);
  void xxxx_geo_delete_f90(int & keyGeom);

// -----------------------------------------------------------------------------
//  Fields
// -----------------------------------------------------------------------------
  void xxxx_field_create_f90(int & keyFlds, const int &, const int & keyVars);
  void xxxx_field_delete_f90(int & keyFlds);

  void xxxx_field_copy_f90(const int & keyFlds, const int & keyFldsOther);
  void xxxx_field_zero_f90(const int & keyFlds);
  void xxxx_field_self_add_f90(const int & keyFlds, const int & keyFldsOther);
  void xxxx_field_self_sub_f90(const int & keyFlds, const int & keyFldsOther);
  void xxxx_field_self_mul_f90(const int & keyFlds, const double &);
  void xxxx_field_axpy_f90(const int & keyFlds, const double &, const int & keyFldsOther);
  void xxxx_field_dot_prod_f90(const int & keyFlds, const int & keyFldsOther, double &);
  void xxxx_field_self_schur_f90(const int & keyFlds, const int & keyFldsOther);
  void xxxx_field_random_f90(const int & keyFlds);

  void xxxx_field_add_incr_f90(const int & keyFlds, const int & keyFldsOther);
  void xxxx_field_diff_incr_f90(const int & keyFlds, const int & keyFldsOther,
                              const int & keyFldsOther2);

  void xxxx_field_change_resol_f90(const int & keyFlds, const int & keyFldsOther);

  void xxxx_field_read_file_f90(const int & keyFlds, const eckit::Configuration * const *,
                              util::DateTime * const *);
  void xxxx_field_write_file_f90(const int & keyFlds, const eckit::Configuration * const *,
                               const util::DateTime * const *);

  void xxxx_field_gpnorm_f90(const int & keyFlds, const int &, double &);
  void xxxx_field_sizes_f90(const int & keyFlds, int &, int &, int &, int &);
  void xxxx_field_rms_f90(const int & keyFlds, double &);

// -----------------------------------------------------------------------------
//  Variables
// -----------------------------------------------------------------------------
  void xxxx_var_create_f90(int & keyVars, const eckit::Configuration * const *);
  void xxxx_var_clone_f90(const int & keyVars, int & keyVars_other);
  void xxxx_var_info_f90(const int & keyVars, int &, int &);
  void xxxx_var_delete_f90(int & keyVars);
}

// -----------------------------------------------------------------------------

}  // namespace xxxx
#endif  // XXXX_MODEL_XXXXFORTRAN_H_
