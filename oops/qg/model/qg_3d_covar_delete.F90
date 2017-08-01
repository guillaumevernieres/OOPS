! (C) Copyright 2009-2016 ECMWF.
! 
! This software is licensed under the terms of the Apache Licence Version 2.0
! which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
! In applying this licence, ECMWF does not waive the privileges and immunities 
! granted to it by virtue of its status as an intergovernmental organisation nor
! does it submit to any jurisdiction.

!> Delete for the QG model's 3d error covariance matrices

subroutine qg_3d_covar_delete(c_key_conf)

use iso_c_binding
use qg_3d_covar_configs

implicit none
integer(c_int), intent(inout) :: c_key_conf !< The model covariance structure

type(qg_3d_covar_config), pointer :: conf !< covar structure

! ------------------------------------------------------------------------------

call qg_3d_cov_registry%get(c_key_conf, conf)

deallocate(conf%sqrt_zonal)
deallocate(conf%sqrt_merid)
deallocate(conf%sqrt_inv_merid)
call qg_3d_cov_registry%remove(c_key_conf)

! ------------------------------------------------------------------------------

end subroutine qg_3d_covar_delete
