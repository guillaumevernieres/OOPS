! (C) Copyright 2009-2016 ECMWF.
! 
! This software is licensed under the terms of the Apache Licence Version 2.0
! which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
! In applying this licence, ECMWF does not waive the privileges and immunities 
! granted to it by virtue of its status as an intergovernmental organisation nor
! does it submit to any jurisdiction.

!> Fortran module for generating random vectors
module random_vectors_mod

use, intrinsic :: iso_c_binding
use kinds

implicit none
private
public :: random_vector

! ------------------------------------------------------------------------------

integer, save, private :: iseed=0

!> Fortran generic for generating random 1d, 2d and 3d arrays
interface random_vector
module procedure random_vector_1, random_vector_2, random_vector_3
end interface

! ------------------------------------------------------------------------------
contains
! ------------------------------------------------------------------------------

!> Generate a random vector of c_doubles
subroutine c_random_vector(nn, xx) bind(c,name='random_vector_f90')
implicit none
integer(c_int), intent(in)    :: nn
real(c_double), intent(inout) :: xx(nn)
integer :: jj

iseed=iseed+1
call random_seed(iseed)
call random_number(xx)
do jj=1,nn
  xx(jj)=2.0_kind_real*xx(jj)-1.0_kind_real
enddo

return
end subroutine c_random_vector

! ------------------------------------------------------------------------------

!> Generate a random 1d array of reals
subroutine random_vector_1(xx)
implicit none
real(kind=kind_real), intent(inout) :: xx(:)

iseed=iseed+1
call random_seed(iseed)
call random_number(xx)
xx(:)=2.0_kind_real*xx(:)-1.0_kind_real

return
end subroutine random_vector_1

! ------------------------------------------------------------------------------

!> Generate a random 2d array of reals
subroutine random_vector_2(xx)
implicit none
real(kind_real), intent(inout) :: xx(:,:)

iseed=iseed+1
call random_seed(iseed)
call random_number(xx)
xx(:,:)=2.0_kind_real*xx(:,:)-1.0_kind_real

return
end subroutine random_vector_2

! ------------------------------------------------------------------------------

!> Generate a random 3d array of reals
subroutine random_vector_3(xx)
implicit none
real(kind_real), intent(inout) :: xx(:,:,:)

iseed=iseed+1
call random_seed(iseed)
call random_number(xx)
xx(:,:,:)=2.0_kind_real*xx(:,:,:)-1.0_kind_real

return
end subroutine random_vector_3

! ------------------------------------------------------------------------------

end module random_vectors_mod
