! (C) Copyright 2009-2016 ECMWF.
! 
! This software is licensed under the terms of the Apache Licence Version 2.0
! which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
! In applying this licence, ECMWF does not waive the privileges and immunities 
! granted to it by virtue of its status as an intergovernmental organisation nor
! does it submit to any jurisdiction.

!> Save trajectory and perform a timestep of the QG model

subroutine c_qg_prop_traj(c_key_conf, c_key_state, c_key_traj) bind(c,name='qg_prop_traj_f90')

use iso_c_binding
use qg_fields
use qg_configs
use qg_trajectories

implicit none
integer(c_int), intent(in)    :: c_key_conf  !< Config structure
integer(c_int), intent(in)    :: c_key_state !< Model fields
integer(c_int), intent(inout) :: c_key_traj  !< Trajectory structure

type(qg_config),     pointer :: conf
type(qg_field),      pointer :: flds
type(qg_trajectory), pointer :: traj

! ------------------------------------------------------------------------------

call qg_config_registry%get(c_key_conf,conf)
call qg_field_registry%get(c_key_state,flds)

call qg_traj_registry%init()            
call qg_traj_registry%add(c_key_traj)
call qg_traj_registry%get(c_key_traj,traj)
call set_traj(traj,flds%nx,flds%ny, &
            & flds%x,flds%x_north,flds%x_south,flds%q_north,flds%q_south)

! ------------------------------------------------------------------------------
return
end subroutine c_qg_prop_traj
