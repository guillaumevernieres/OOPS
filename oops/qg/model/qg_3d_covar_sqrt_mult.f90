! (C) Copyright 2009-2016 ECMWF.
! 
! This software is licensed under the terms of the Apache Licence Version 2.0
! which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
! In applying this licence, ECMWF does not waive the privileges and immunities 
! granted to it by virtue of its status as an intergovernmental organisation nor
! does it submit to any jurisdiction.

!> Multiply streamfunction by sqrt(C), where C is a 3d covariance matrix

subroutine qg_3d_covar_sqrt_mult(kx,ky,xincr,xctl,config)

use iso_c_binding
use qg_fields
use qg_3d_covar_configs
use kinds

implicit none
integer(c_int), intent(in) :: kx            !< Zonal grid dimension
integer(c_int), intent(in) :: ky            !< Meridional grid dimension
type(qg_field), intent(inout) :: xincr        !< sqrt(C) times streamfunction
real(c_double), intent(in) :: xctl(kx,ky,2) !< Streamfunction
type(qg_3d_covar_config), intent(in) :: config !< covariance config structure

integer :: ifax(13)
real(kind=kind_real), allocatable :: trigs(:), xext(:,:,:), work(:)
integer :: i, j, k, iri, m
real(kind=kind_real) :: zc, zero, one

! ------------------------------------------------------------------------------

!--- multiply by symmetric square-root of vertical correlation matrix

zc = sqrt(1.0_kind_real-config%vert_corr*config%vert_corr)
do j=1,ky
  do i=1,kx
    xincr%x(i,j,1) = xctl(i,j,1)
    xincr%x(i,j,2) = config%vert_corr * xctl(i,j,1) + zc * xctl(i,j,2)
  enddo
enddo

!--- multiply by square-root of meridional correlation matrix

zero = 0.0_kind_real
one = 1.0_kind_real
allocate(work(ky))
work(:)=0.0_kind_real
do k=1,2
  do i=1,kx
    call DSYMV('L',ky,one,config%sqrt_merid,ky,xincr%x(i,1,k),kx,zero,work,1)
    do j=1,ky
      xincr%x(i,j,k) = work(j)
    enddo
  enddo
enddo
deallocate(work)

!--- multiply by square-root of zonal correlation matrix

allocate (trigs(3*kx/2+1))
call set99 (trigs,ifax,kx)

allocate(xext(kx+2,ky,2))
xext(1:kx,:,:) = xincr%x(:,:,:)
xext(kx+1:kx+2,:,:) = 0.0_kind_real

allocate(work(2*ky*(kx+2)))

call fft991 (xext,work,trigs,ifax,1,kx+2,kx,2*ky,-1)

do k=1,2
  do j=1,ky
    do m=0,kx/2
      do iri=1,2
        xext(2*m+iri,j,k) = xext(2*m+iri,j,k) * config%sqrt_zonal(m)
      enddo
    enddo
  enddo
enddo

call fft991 (xext,work,trigs,ifax,1,kx+2,kx,2*ky,+1)
xincr%x(:,:,:) = xext(1:kx,:,:)

deallocate(work)
deallocate(xext)
deallocate(trigs)

!--- multiply by standard deviation

do k=1,2
  do j=1,ky
    do i=1,kx
      xincr%x(i,j,k) = xincr%x(i,j,k) * config%sigma
    enddo
  enddo
enddo

end subroutine qg_3d_covar_sqrt_mult
