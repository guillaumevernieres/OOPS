! (C) Copyright 2009-2016 ECMWF.
! 
! This software is licensed under the terms of the Apache Licence Version 2.0
! which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
! In applying this licence, ECMWF does not waive the privileges and immunities 
! granted to it by virtue of its status as an intergovernmental organisation nor
! does it submit to any jurisdiction.

!> Delete the QG model

subroutine c_qg_delete(c_key_conf) bind (c,name='qg_delete_f90')

use qg_configs
use iso_c_binding

implicit none
integer(c_int), intent(inout) :: c_key_conf !< Key to configuration structure
type(qg_config), pointer :: conf

! ------------------------------------------------------------------------------

call qg_config_registry%get(c_key_conf, conf)
deallocate(conf%rs)
call qg_config_registry%remove(c_key_conf)

! ------------------------------------------------------------------------------
return
end subroutine c_qg_delete
