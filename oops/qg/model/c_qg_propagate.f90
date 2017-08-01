! (C) Copyright 2009-2016 ECMWF.
! 
! This software is licensed under the terms of the Apache Licence Version 2.0
! which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
! In applying this licence, ECMWF does not waive the privileges and immunities 
! granted to it by virtue of its status as an intergovernmental organisation nor
! does it submit to any jurisdiction.

!> Perform a timestep of the QG model

subroutine c_qg_propagate(c_key_conf, c_key_state) bind(c,name='qg_propagate_f90')

use iso_c_binding
use qg_fields
use qg_configs

implicit none
integer(c_int), intent(in) :: c_key_conf  !< Config structure
integer(c_int), intent(in) :: c_key_state !< Model fields

type(qg_config), pointer :: conf
type(qg_field),  pointer :: flds

! ------------------------------------------------------------------------------

call qg_config_registry%get(c_key_conf, conf)
call qg_field_registry%get(c_key_state,flds)

call propagate(flds, conf)

! ------------------------------------------------------------------------------
return
end subroutine c_qg_propagate
