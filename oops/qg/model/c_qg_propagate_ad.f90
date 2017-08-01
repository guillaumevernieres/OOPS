! (C) Copyright 2009-2016 ECMWF.
! 
! This software is licensed under the terms of the Apache Licence Version 2.0
! which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
! In applying this licence, ECMWF does not waive the privileges and immunities 
! granted to it by virtue of its status as an intergovernmental organisation nor
! does it submit to any jurisdiction.

!> Perform a timestep of the QG model - Adjoint

!> This routine is called from C++ to propagate the adjoint variables

subroutine c_qg_propagate_ad(c_key_conf, c_key_incr, c_key_traj) &
           bind(c,name='qg_propagate_ad_f90')

use iso_c_binding
use qg_fields
use qg_trajectories
use qg_configs
use qg_constants, only: bet
use kinds

implicit none
integer(c_int), intent(in) :: c_key_conf !< Config structure
integer(c_int), intent(in) :: c_key_incr !< Model fields
integer(c_int), intent(in) :: c_key_traj !< Trajectory structure

type(qg_config),     pointer :: conf
type(qg_field),      pointer :: flds
type(qg_trajectory), pointer :: traj

real(kind=kind_real), allocatable :: qnew(:,:,:), x_traj(:,:,:)
real(kind=kind_real), allocatable :: q_traj(:,:,:), u_traj(:,:,:), v_traj(:,:,:)
real(kind=kind_real), allocatable :: qn_traj(:,:), qs_traj(:,:)
real(kind=kind_real) :: xn_traj(2), xs_traj(2)

! ------------------------------------------------------------------------------

call qg_config_registry%get(c_key_conf,conf)
call qg_field_registry%get(c_key_incr,flds)
call qg_traj_registry%get(c_key_traj,traj)

!--- workspace and trajectory
allocate(qnew(flds%nx,flds%ny,2))
allocate(x_traj(flds%nx,flds%ny,2))
allocate(qn_traj(flds%nx,2))
allocate(qs_traj(flds%nx,2))
allocate(q_traj(flds%nx,flds%ny,2))
allocate(u_traj(flds%nx,flds%ny,2))
allocate(v_traj(flds%nx,flds%ny,2))
call get_traj(traj,flds%nx,flds%ny,x_traj,xn_traj,xs_traj,qn_traj,qs_traj)

!--- generate trajectory values for potential vorticity and wind

call calc_pv(flds%nx,flds%ny,q_traj,x_traj,xn_traj,xs_traj, &
    &        conf%f1,conf%f2,conf%deltax,conf%deltay,bet,conf%rs)
call zonal_wind (u_traj,x_traj,xn_traj,xs_traj,flds%nx,flds%ny, conf%deltay)
call meridional_wind (v_traj,x_traj,flds%nx,flds%ny,conf%deltax)

! -- calculate potential vorticity and wind components

call meridional_wind_ad(flds%v,flds%x,flds%nx,flds%ny,conf%deltax)
call zonal_wind_ad(flds%u,flds%x,flds%nx,flds%ny,conf%deltay)
qnew(:,:,:) = flds%q(:,:,:)

!--- invert the potential vorticity to determine streamfunction

call invert_pv_ad(flds%x,qnew,flds%nx,flds%ny, &
                & conf%deltax,conf%deltay,conf%f1,conf%f2)

!--- advect the potential vorticity

call advect_pv_ad(qnew,flds%q,q_traj,qn_traj,qs_traj, &
    &             flds%u,u_traj,flds%v,v_traj,flds%nx,flds%ny, &
    &             conf%deltax,conf%deltay,conf%dt)

!--- clean-up
deallocate(qnew)
deallocate(x_traj)
deallocate(qn_traj)
deallocate(qs_traj)
deallocate(q_traj)
deallocate(u_traj)
deallocate(v_traj)
! ------------------------------------------------------------------------------
return
end subroutine c_qg_propagate_ad
