! (C) Copyright 2009-2016 ECMWF.
! 
! This software is licensed under the terms of the Apache Licence Version 2.0
! which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
! In applying this licence, ECMWF does not waive the privileges and immunities 
! granted to it by virtue of its status as an intergovernmental organisation nor
! does it submit to any jurisdiction.

!> Delete for the QG model's background error covariance matrix

subroutine c_qg_b_delete(c_conf) bind (c,name='qg_b_delete_f90')

use iso_c_binding

implicit none
type(c_ptr), value :: c_conf !< The background covariance structure

integer(c_int), pointer :: f_conf

! ------------------------------------------------------------------------------

call c_f_pointer(c_conf,f_conf)

call qg_3d_covar_delete(f_conf)

end subroutine c_qg_b_delete
