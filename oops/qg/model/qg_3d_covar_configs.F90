! (C) Copyright 2009-2016 ECMWF.
! 
! This software is licensed under the terms of the Apache Licence Version 2.0
! which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
! In applying this licence, ECMWF does not waive the privileges and immunities 
! granted to it by virtue of its status as an intergovernmental organisation nor
! does it submit to any jurisdiction.

!> Structure holding configuration variables for the 3d error
!! covariance matrices of the QG analysis.

module qg_3d_covar_configs

use kinds
implicit none

!> Fortran derived type to hold configuration data for the QG background/model covariance
type :: qg_3d_covar_config
  integer :: nx !< Zonal grid dimension
  integer :: ny !< Meridional grid dimension
  real(kind=kind_real)    :: sigma        !< Standard deviation
  real(kind=kind_real)    :: vert_corr    !< Vertical correlation between levels
  real(kind=kind_real), allocatable :: sqrt_merid(:,:) !< sqrt(meridional correlation matrix)
  real(kind=kind_real), allocatable :: sqrt_inv_merid(:,:) !< Its inverse
  real(kind=kind_real), allocatable :: sqrt_zonal(:) !< Spectral weights for sqrt(zonal corr)

end type qg_3d_covar_config

#define LISTED_TYPE qg_3d_covar_config

!> Linked list interface - defines registry_t type
#include "linkedList_i.f"

!> Global registry
type(registry_t) :: qg_3d_cov_registry

! ------------------------------------------------------------------------------
contains
! ------------------------------------------------------------------------------
!> Linked list implementation
#include "linkedList_c.f"

! ------------------------------------------------------------------------------

end module qg_3d_covar_configs
