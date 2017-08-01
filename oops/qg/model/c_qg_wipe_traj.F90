! (C) Copyright 2009-2016 ECMWF.
! 
! This software is licensed under the terms of the Apache Licence Version 2.0
! which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
! In applying this licence, ECMWF does not waive the privileges and immunities 
! granted to it by virtue of its status as an intergovernmental organisation nor
! does it submit to any jurisdiction.

!> Wipe out trajectory of the QG model

subroutine c_qg_wipe_traj(c_key_traj) bind(c,name='qg_wipe_traj_f90')

use iso_c_binding
use qg_trajectories

implicit none
integer(c_int), intent(inout) :: c_key_traj  !< Trajectory structure
type(qg_trajectory), pointer :: traj

! ------------------------------------------------------------------------------

call qg_traj_registry%get(c_key_traj,traj)
call delete_traj(traj)
call qg_traj_registry%remove(c_key_traj)

! ------------------------------------------------------------------------------
return
end subroutine c_qg_wipe_traj
