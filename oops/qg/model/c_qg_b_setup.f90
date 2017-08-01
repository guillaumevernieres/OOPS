! (C) Copyright 2009-2016 ECMWF.
! 
! This software is licensed under the terms of the Apache Licence Version 2.0
! which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
! In applying this licence, ECMWF does not waive the privileges and immunities 
! granted to it by virtue of its status as an intergovernmental organisation nor
! does it submit to any jurisdiction.

!> Setup for the QG model's background error covariance matrix

subroutine c_qg_b_setup(c_key_conf, c_model, c_key_geom) &
          & bind (c,name='qg_b_setup_f90')

use iso_c_binding
use qg_3d_covar_configs
use qg_geom_mod

implicit none
integer(c_int), intent(inout) :: c_key_conf   !< The background covariance structure
type(c_ptr), intent(in)    :: c_model  !< The configuration
integer(c_int), intent(in) :: c_key_geom !< Geometry

type(qg_3d_covar_config), pointer :: conf
type(qg_geom),  pointer :: geom

! ------------------------------------------------------------------------------

call qg_geom_registry%get(c_key_geom, geom)
call qg_3d_cov_registry%init()
call qg_3d_cov_registry%add(c_key_conf)
call qg_3d_cov_registry%get(c_key_conf, conf)

call qg_3d_covar_setup(c_model, geom, conf)

end subroutine c_qg_b_setup
