! (C) Copyright 2009-2016 ECMWF.
! 
! This software is licensed under the terms of the Apache Licence Version 2.0
! which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
! In applying this licence, ECMWF does not waive the privileges and immunities 
! granted to it by virtue of its status as an intergovernmental organisation nor
! does it submit to any jurisdiction.

!> Setup for the QG model's 3d error covariance matrices (B and Q_i)

!> This routine queries the configuration for the parameters that define the
!! covariance matrix, and stores the relevant values in the
!! error covariance structure.

subroutine qg_3d_covar_setup(c_model, geom, config)

use qg_constants
use qg_3d_covar_configs
use qg_geom_mod
use iso_c_binding
use config_mod
use kinds
use fckit_log_module, only : fckit_log

implicit none
type(c_ptr), intent(in)   :: c_model  !< The configuration
type(qg_geom), intent(in) :: geom     !< Geometry
type(qg_3d_covar_config), intent(inout) :: config !< The covariance structure

integer :: ifax(13)
real(kind=kind_real), allocatable :: trigs(:), struct_fn(:), evals(:), &
                      work(:), evects(:,:), revals(:)
character(len=160) :: record

real(kind=kind_real) :: corr_length_scale, dy, z, dx, condition_number, threshold
integer :: i, j, k, info

! ------------------------------------------------------------------------------

config%nx         = geom%nx
config%ny         = geom%ny
config%sigma      = config_get_real(c_model,"standard_deviation")
config%vert_corr  = config_get_real(c_model,"vertical_correlation")
corr_length_scale = config_get_real(c_model,"horizontal_length_scale")
condition_number  = config_get_real(c_model,"maximum_condition_number")

dx = domain_zonal/real(config%nx,kind_real)
dy = domain_meridional/real(1+config%ny,kind_real)

!--- Generate the lower triangle of the meridional correlation matrix

allocate(evects(config%ny,config%ny))

do i=1,config%ny
  z = (sqrt(0.5_kind_real)*dy/corr_length_scale)*real(i-1,kind_real)
  evects(i,1) = exp(-z*z)
enddo

do j=2,config%ny
  do i=j,config%ny
    evects(i,j) = evects(i-j+1,1)
  enddo
enddo

!--- Calculate the eigen-decomposition (NB: assuming 8-byte reals)

allocate(evals(config%ny))
allocate(work((config%ny+3)*config%ny))

call DSYEV ('V','L',config%ny,evects,config%ny,evals,work,size(work),info)

if (info/=0) then
  write(record,*) "c_qg_3d_covar_setup: DSYEV returns info=",info
  call fckit_log%error(record)
  call abor1_ftn ("c_qg_3d_covar_setup: DSYEV failed to sqrt matrix")
endif

deallocate(work)

!--- Calculate the lower triangle of the symmetric square-root and its inverse
!--- (NB: assuming 8-byte reals)

allocate(revals(config%ny))

threshold = maxval(evals(:))/condition_number

do j=1,config%ny
  evals(j) = sqrt(max(threshold,evals(j)))
  revals(j) = 1.0_kind_real/evals(j)
enddo

allocate(config%sqrt_merid(config%ny,config%ny))
allocate(config%sqrt_inv_merid(config%ny,config%ny))

do j=1,config%ny
  do i=j,config%ny
    config%sqrt_merid(i,j) = 0.0_kind_real
    config%sqrt_inv_merid(i,j) = 0.0_kind_real

    do k=1,config%ny
      config%sqrt_merid(i,j) = config%sqrt_merid(i,j) &
                 & + evects(i,k)*evects(j,k)*evals(k)
      config%sqrt_inv_merid(i,j) = config%sqrt_inv_merid(i,j) &
                 & + evects(i,k)*evects(j,k)*revals(k)
    enddo
  enddo
enddo

deallocate(revals)
deallocate(evects)
deallocate(evals)

!--- Calculate spectral weights for zonal correlations:
!--- First we construct the structure function in grid space and FFT it

allocate(struct_fn(config%nx+2))
do j=1,config%nx
  z = (sqrt(0.5_kind_real)*dx/corr_length_scale) &
   & *real(min(j-1,1+config%nx-j),kind_real)
  struct_fn(j) = exp(-z*z)
enddo

allocate (trigs(3*config%nx/2+1))
allocate(work(config%nx+2))
call set99 (trigs,ifax,config%nx)
call fft991 (struct_fn,work,trigs,ifax,1,config%nx+2,config%nx,1,-1)
deallocate(work)
deallocate(trigs)

if (mod(config%nx,2)/=0) then
  write(record,*) "c_qg_3d_covar_setup: number of zonal gridpoints nx=",config%nx
  call fckit_log%error(record)
  call abor1_ftn("c_qg_3d_covar_setup: odd number of zonal grid points")
endif

!--- We are after sqrt(B) or sqrt(Q), so square-root the coefficents.
!--- The factor N ensures we get the struture function back if we apply
!--- B or Q to a unit delta function.

allocate(config%sqrt_zonal(0:config%nx/2))

threshold = 0.0_kind_real
do i=0,config%nx/2
  threshold = max(threshold,struct_fn(1+2*i))
enddo
threshold = threshold/condition_number

do i=0,config%nx/2
  config%sqrt_zonal(i) = sqrt(max(threshold,real(config%nx,kind_real)*struct_fn(1+2*i)))
enddo

deallocate(struct_fn)

! ------------------------------------------------------------------------------
return
end subroutine qg_3d_covar_setup
