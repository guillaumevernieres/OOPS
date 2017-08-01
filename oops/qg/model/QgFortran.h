/*
 * (C) Copyright 2009-2016 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef QG_MODEL_QGFORTRAN_H_
#define QG_MODEL_QGFORTRAN_H_

// Forward declarations
namespace eckit {
  class Configuration;
}

namespace util {
  class DateTime;
  class Duration;
}

namespace qg {

/// Interface to Fortran QG model
/*!
 * The core of the QG model is coded in Fortran.
 * Here we define the interfaces to the Fortran code.
 */

extern "C" {
// -----------------------------------------------------------------------------
//  Geometry
// -----------------------------------------------------------------------------
  void qg_geo_setup_f90(int & keyGeom, const eckit::Configuration * const *);
  void qg_geo_clone_f90(const int & keyGeom, int & keyGeom_other);
  void qg_geo_info_f90(const int & keyGeom, int &, int &);
  void qg_geo_delete_f90(int & keyGeom);

// -----------------------------------------------------------------------------
//  Model
// -----------------------------------------------------------------------------
  void qg_setup_f90(const eckit::Configuration * const *, const int &, int & keyConf);
  void qg_delete_f90(int &);

  void qg_prepare_integration_f90(const int & keyConf, const int & keyFlds);
  void qg_prepare_integration_tl_f90(const int & keyConf, const int & keyFlds);
  void qg_prepare_integration_ad_f90(const int & keyConf, const int & keyFlds);

  void qg_propagate_f90(const int & keyConf, const int & keyFlds);
  void qg_prop_traj_f90(const int & keyConf, const int & keyFlds, int & keyTraj);
  void qg_propagate_tl_f90(const int & keyConf, const int & keyFlds,
                           const int & keyTraj);
  void qg_propagate_ad_f90(const int & keyConf, const int & keyFlds,
                           const int & keyTraj);

  void qg_wipe_traj_f90(int & keyTraj);
  void qg_traj_minmaxrms_f90(const int & keyTraj, double &);

// -----------------------------------------------------------------------------
//  Fields
// -----------------------------------------------------------------------------
  void qg_field_create_f90(int & keyFlds, const int &, const int & keyVars);
  void qg_field_delete_f90(int & keyFlds);

  void qg_field_copy_f90(const int & keyFlds, const int & keyFldsOther);
  void qg_field_zero_f90(const int & keyFlds);
  void qg_field_self_add_f90(const int & keyFlds, const int & keyFldsOther);
  void qg_field_self_sub_f90(const int & keyFlds, const int & keyFldsOther);
  void qg_field_self_mul_f90(const int & keyFlds, const double &);
  void qg_field_axpy_f90(const int & keyFlds, const double &, const int & keyFldsOther);
  void qg_field_dot_prod_f90(const int & keyFlds, const int & keyFldsOther, double &);
  void qg_field_self_schur_f90(const int & keyFlds, const int & keyFldsOther);
  void qg_field_random_f90(const int & keyFlds);

  void qg_field_add_incr_f90(const int & keyFlds, const int & keyFldsOther);
  void qg_field_diff_incr_f90(const int & keyFlds, const int & keyFldsOther,
                              const int & keyFldsOther2);

  void qg_field_change_resol_f90(const int & keyFlds, const int & keyFldsOther);

  void qg_field_read_file_f90(const int & keyFlds, const eckit::Configuration * const *,
                              util::DateTime * const *);
  void qg_field_write_file_f90(const int & keyFlds, const eckit::Configuration * const *,
                               const util::DateTime * const *);

  void qg_field_interp_f90(const int & keyFlds, const int &,
                           const int & keyGoms);
  void qg_field_interp_tl_f90(const int & keyFlds, const int &,
                              const int & keyGoms);
  void qg_field_interp_ad_f90(const int & keyFlds, const int &,
                              const int & keyGoms);

  void qg_field_gpnorm_f90(const int & keyFlds, const int &, double &);
  void qg_field_sizes_f90(const int & keyFlds, int &, int &, int &, int &);
  void qg_field_rms_f90(const int & keyFlds, double &);

// -----------------------------------------------------------------------------
//  Background error
// -----------------------------------------------------------------------------
  void qg_b_setup_f90(int & keyBmat, const eckit::Configuration * const *,
                      const int &);
  void qg_b_delete_f90(int & keyBmat);

  void qg_b_linearize_f90(const int & keyBmat, const eckit::Configuration * const *);

  void qg_b_mult_f90(const int & keyBmat,
                     const int & keyFlds, const int & keyFldsOther);
  void qg_b_invmult_f90(const int & keyBmat,
                        const int & keyFlds, const int & keyFldsOther);

  void qg_b_randomize_f90(const int & keyBmat, const int & keyFldsOther);

// -----------------------------------------------------------------------------
//  Localization matrix
// -----------------------------------------------------------------------------
  void qg_localization_setup_f90(int & keyBmat, const eckit::Configuration * const *,
                                 const int &);
  void qg_localization_delete_f90(int & keyBmat);
  void qg_localization_mult_f90(const int & keyBmat, const int & keyFlds);

// -----------------------------------------------------------------------------
//  Variables
// -----------------------------------------------------------------------------
  void qg_var_create_f90(int & keyVars, const eckit::Configuration * const *);
  void qg_var_clone_f90(const int & keyVars, int & keyVars_other);
  void qg_var_info_f90(const int & keyVars, int &, int &);
  void qg_var_delete_f90(int & keyVars);

// -----------------------------------------------------------------------------
//  Locations
// -----------------------------------------------------------------------------
  void qg_loc_delete_f90(int & keyLoc);
  void qg_loc_nobs_f90(const int & , int &);

// -----------------------------------------------------------------------------
//  Local Values (GOM)
// -----------------------------------------------------------------------------
  void qg_gom_create_f90(int & keyGoms);
  void qg_gom_delete_f90(int & keyGoms);
  void qg_gom_zero_f90(const int & keyGoms);
  void qg_gom_dotprod_f90(const int & keyGoms, const int & keyGomsOther, double &);
  void qg_gom_minmaxavg_f90(const int & keyGoms, int &, double &, double &, double &);

// -----------------------------------------------------------------------------
//  Streamfunction observations
// -----------------------------------------------------------------------------
  void qg_stream_setup_f90(int & keyOper, const eckit::Configuration * const *);
  void qg_stream_delete_f90(int & keyOper);

  void qg_stream_equiv_f90(const int & keyGoms, const int & keyOvec, const double &);
  void qg_stream_equiv_tl_f90(const int & keyGoms, const int & keyOvec, const double &);
  void qg_stream_equiv_ad_f90(const int & keyGoms, const int & keyOvec, double &);

// -----------------------------------------------------------------------------
//  Wind observations
// -----------------------------------------------------------------------------
  void qg_wind_setup_f90(int & keyOper, const eckit::Configuration * const *);
  void qg_wind_delete_f90(int & keyOper);

  void qg_wind_equiv_f90(const int & keyGoms, int & keyOvec, const double &);
  void qg_wind_equiv_tl_f90(const int & keyGoms, const int & keyOvec, const double &);
  void qg_wind_equiv_ad_f90(const int & keyGoms, const int & keyOvec, double &);

// -----------------------------------------------------------------------------
//  Wind speed observations
// -----------------------------------------------------------------------------
  void qg_wspeed_setup_f90(int & keyOper, const eckit::Configuration * const *);
  void qg_wspeed_delete_f90(int & keyOper);

  void qg_wspeed_eqv_f90(const int & keyGoms, const int & keyOvec, const double &);
  void qg_wspeed_equiv_tl_f90(const int & keyGoms, const int & keyOvec,
                              const int & keyGomsOther, const double &);
  void qg_wspeed_equiv_ad_f90(const int & keyGoms, const int & keyOvec,
                              const int & keyGomsOther, double &);

  void qg_wspeed_gettraj_f90(const int & keyOper, const int &, int & keyGoms);
  void qg_wspeed_settraj_f90(const int & keyGoms, const int & keyGomsOther);

// -----------------------------------------------------------------------------
//  Observation Vectors
// -----------------------------------------------------------------------------
  void qg_obsvec_setup_f90(int & keyOvec, const int &, const int &);
  void qg_obsvec_clone_f90(const int & keyOvec, int & keyOvecOther);
  void qg_obsvec_delete_f90(int & keyOvec);

  void qg_obsvec_assign_f90(const int & keyOvec, const int & keyOvecOther);
  void qg_obsvec_zero_f90(const int & keyOvec);
  void qg_obsvec_mul_scal_f90(const int & keyOvec, const double &);
  void qg_obsvec_add_f90(const int & keyOvec, const int & keyOvecOther);
  void qg_obsvec_sub_f90(const int & keyOvec, const int & keyOvecOther);
  void qg_obsvec_mul_f90(const int & keyOvec, const int & keyOvecOther);
  void qg_obsvec_div_f90(const int & keyOvec, const int & keyOvecOther);
  void qg_obsvec_axpy_f90(const int & keyOvec, const double &, const int & keyOvecOther);
  void qg_obsvec_invert_f90(const int & keyOvec);
  void qg_obsvec_random_f90(const int & keyOvec);
  void qg_obsvec_dotprod_f90(const int & keyOvec, const int & keyOvecOther, double &);
  void qg_obsvec_minmaxavg_f90(const int & keyOvec, double &, double &, double &);
  void qg_obsvec_nobs_f90(const int & keyOvec, int &);

// -----------------------------------------------------------------------------
//  Observation Handler
// -----------------------------------------------------------------------------
  void qg_obsdb_setup_f90(int & keyHelp, const eckit::Configuration * const *);
  void qg_obsdb_delete_f90(int & keyHelp);
  void qg_obsdb_get_f90(const int & keyHelp, const int &, const char *,
                        const int &, const char *, const int & keyOvec);
  void qg_obsdb_put_f90(const int & keyHelp, const int &, const char *,
                        const int &, const char *, const int & keyOvec);
  void qg_obsdb_locations_f90(const int & keyHelp, const int &, const char *,
                              const util::DateTime * const *, const util::DateTime * const *,
                              int & keyLoc);
  void qg_obsdb_getgom_f90(const int & keyHelp, const int &, const char *,
                           const int & keyVars,
                           const util::DateTime * const *, const util::DateTime * const *,
                           int & keyGoms);
  void qg_obsdb_generate_f90(const int & keyHelp, const int &, const char *,
                             const eckit::Configuration * const *, const util::DateTime * const *,
                             const util::Duration * const *, const int &, int &);
  void qg_obsdb_seterr_f90(const int & keyHelp, const int & keyOper, const double &);
  void qg_obsdb_nobs_f90(const int & keyHelp, const int &, const char *, int &);
  void qg_obsoper_inputs_f90(const int & keyOper, int & keyVars);
}

// -----------------------------------------------------------------------------

}  // namespace qg
#endif  // QG_MODEL_QGFORTRAN_H_
