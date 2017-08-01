! (C) Copyright 2009-2016 ECMWF.
! 
! This software is licensed under the terms of the Apache Licence Version 2.0
! which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
! In applying this licence, ECMWF does not waive the privileges and immunities 
! granted to it by virtue of its status as an intergovernmental organisation nor
! does it submit to any jurisdiction.

!> Fortran module for generating normally-distributed random numbers using the Box-Muller method.
module random_vectors_gauss_mod

use, intrinsic :: iso_c_binding
use kinds

implicit none
private
public :: random_vector_gauss

! ------------------------------------------------------------------------------

integer, save, private :: iseed=0

!> Fortran generic for generating random 1d, 2d and 3d arrays
interface random_vector_gauss
module procedure random_vector_gauss_1, random_vector_gauss_2, random_vector_gauss_3
end interface

! ------------------------------------------------------------------------------
contains

! ------------------------------------------------------------------------------

!> Generate a random vector of c_doubles
subroutine c_random_vector_gauss(nn, xx) bind(c,name='random_vector_gauss_f90')
implicit none
integer(c_int), intent(in)    :: nn
real(c_double), intent(inout) :: xx(nn)
integer :: jj
real(kind_real) :: zfac, ztwopi, zx(size(xx)+1)

iseed=iseed+1
call random_seed(iseed)
call random_number(zx)

ztwopi = 8.0_kind_real*atan(1.0_kind_real)

do jj=1,nn-1,2
  zfac = sqrt(-2.0_kind_real*log(1.0_kind_real-zx(jj)))
  xx(jj  ) = zfac*cos(ztwopi*zx(jj+1))
  xx(jj+1) = zfac*sin(ztwopi*zx(jj+1))
enddo
if (mod(nn,2) /= 0) THEN
  zfac = sqrt(-2.0_kind_real*log(1.0_kind_real-zx(nn)))
  xx(nn) = zfac*cos(ztwopi*zx(nn+1))
endif

return
end subroutine c_random_vector_gauss

! ------------------------------------------------------------------------------

!> Generate a random 1d array of reals
subroutine random_vector_gauss_1(xx)
implicit none
real(kind=kind_real), intent(inout) :: xx(:)
integer :: jj, ilen
real(kind_real) :: zfac, ztwopi, zx(size(xx)+1)

iseed=iseed+1
call random_seed(iseed)
call random_number(zx)

ztwopi = 8.0_kind_real*atan(1.0_kind_real)
ilen=size(xx)

do jj=1,ilen-1,2
  zfac = sqrt(-2.0_kind_real*log(1.0_kind_real-zx(jj)))
  xx(jj  ) = zfac*cos(ztwopi*zx(jj+1))
  xx(jj+1) = zfac*sin(ztwopi*zx(jj+1))
enddo
if (mod(ilen,2) /= 0) THEN
  zfac = sqrt(-2.0_kind_real*log(1.0_kind_real-zx(ilen)))
  xx(ilen) = zfac*cos(ztwopi*zx(ilen+1))
endif

return
end subroutine random_vector_gauss_1

! ------------------------------------------------------------------------------

!> Generate a random 2d array of reals
subroutine random_vector_gauss_2(xx)
implicit none
real(kind_real), intent(inout) :: xx(:,:)
real(kind_real) :: zx(size(xx))

call random_vector_gauss_1(zx)
xx = reshape(zx, shape(xx))

return
end subroutine random_vector_gauss_2

! ------------------------------------------------------------------------------

!> Generate a random 3d array of reals
subroutine random_vector_gauss_3(xx)
implicit none
real(kind_real), intent(inout) :: xx(:,:,:)
real(kind_real) :: zx(size(xx))

call random_vector_gauss_1(zx)
xx = reshape(zx, shape(xx))

return
end subroutine random_vector_gauss_3

! ------------------------------------------------------------------------------
end module random_vectors_gauss_mod
