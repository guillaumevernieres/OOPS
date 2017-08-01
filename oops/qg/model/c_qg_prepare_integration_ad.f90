! (C) Copyright 2009-2016 ECMWF.
! 
! This software is licensed under the terms of the Apache Licence Version 2.0
! which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
! In applying this licence, ECMWF does not waive the privileges and immunities 
! granted to it by virtue of its status as an intergovernmental organisation nor
! does it submit to any jurisdiction.

!> Prepare for an integration of the QG model - Adjoint.

subroutine c_qg_prepare_integration_ad(c_key_conf, c_key_incr) &
           bind(c,name='qg_prepare_integration_ad_f90')

use iso_c_binding
use qg_fields
use qg_configs
use kinds

implicit none
integer(c_int), intent(in) :: c_key_conf !< Configuration structure
integer(c_int), intent(in) :: c_key_incr !< Model fields

type(qg_config), pointer :: conf
type(qg_field), pointer  :: flds

! ------------------------------------------------------------------------------

call qg_config_registry%get(c_key_conf,conf)
call qg_field_registry%get(c_key_incr,flds)

! -- calculate potential vorticity and wind components

call meridional_wind_ad(flds%v,flds%x,flds%nx,flds%ny, conf%deltax)
call zonal_wind_ad(flds%u,flds%x,flds%nx,flds%ny,conf%deltay)
call calc_pv_ad(flds%q,flds%x,flds%nx,flds%ny, &
              & conf%f1,conf%f2,conf%deltax,conf%deltay)
flds%q=0.0_kind_real;
flds%u=0.0_kind_real;
flds%v=0.0_kind_real;

! ------------------------------------------------------------------------------
return
end subroutine c_qg_prepare_integration_ad
