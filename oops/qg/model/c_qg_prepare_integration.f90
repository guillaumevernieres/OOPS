! (C) Copyright 2009-2016 ECMWF.
! 
! This software is licensed under the terms of the Apache Licence Version 2.0
! which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
! In applying this licence, ECMWF does not waive the privileges and immunities 
! granted to it by virtue of its status as an intergovernmental organisation nor
! does it submit to any jurisdiction.

!> Prepare for an integration of the QG model.

!> At the start of a timestep of the QG model, the state must contain
!! streamfunction, potential vorticity and wind components. The control
!! variable for the analysis, however, contains only streamfunction.
!! This routine calculates potential vorticity and wind from the
!! streamfunction, and is called before an integration of the QG model.

subroutine c_qg_prepare_integration(c_key_conf, c_key_state) &
         & bind(c,name='qg_prepare_integration_f90')

use iso_c_binding
use qg_fields
use qg_configs
use qg_constants, only: bet

implicit none
integer(c_int), intent(in) :: c_key_conf  !< Configuration structure
integer(c_int), intent(in) :: c_key_state !< Model fields

type(qg_config), pointer :: conf
type(qg_field), pointer  :: flds

! ------------------------------------------------------------------------------

call qg_field_registry%get(c_key_state,flds)
call qg_config_registry%get(c_key_conf, conf)

! -- calculate potential vorticity and wind components

call calc_pv(flds%nx,flds%ny,flds%q,flds%x,flds%x_north,flds%x_south, &
           & conf%f1,conf%f2,conf%deltax,conf%deltay,bet,conf%rs)
call zonal_wind(flds%u,flds%x,flds%x_north,flds%x_south,flds%nx,flds%ny, &
              & conf%deltay)
call meridional_wind(flds%v,flds%x,flds%nx,flds%ny,conf%deltax)

end subroutine c_qg_prepare_integration
