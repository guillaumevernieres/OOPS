! (C) Copyright 2009-2016 ECMWF.
! 
! This software is licensed under the terms of the Apache Licence Version 2.0
! which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
! In applying this licence, ECMWF does not waive the privileges and immunities 
! granted to it by virtue of its status as an intergovernmental organisation nor
! does it submit to any jurisdiction.

!> Calculate potential vorticity - Tangent Linear

subroutine calc_pv_tl (pv,x,nx,ny,f1,f2,deltax,deltay)

!--- calculate potential vorticity from streamfunction

use kinds

implicit none
integer, intent(in) :: nx         !< Zonal grid dimension
integer, intent(in) :: ny         !< Meridional grid dimension
real(kind=kind_real), intent(out) :: pv(nx,ny,2)  !< Potential vorticity increment
real(kind=kind_real), intent(in)  :: x(nx,ny,2)   !< Streamfunction increment
real(kind=kind_real), intent(in) :: f1            !< Coefficient in PV operator
real(kind=kind_real), intent(in) :: f2            !< Coefficient in PV operator
real(kind=kind_real), intent(in) :: deltax        !< Zonal grid spacing (non-dimensional)
real(kind=kind_real), intent(in) :: deltay        !< Meridional grid spacing (non-dimensional)

!--- apply the linear operator

call pv_operator(x,pv,nx,ny,f1,f2,deltax,deltay)

!--- add the contribution from the boundaries

  !<-- TL of this is identity. -->

!--- add the beta term

  !<-- TL of this is identity. -->

!--- add the orography/heating term

  !<-- TL of this is identity. -->

end subroutine calc_pv_tl
