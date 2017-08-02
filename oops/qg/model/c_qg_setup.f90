! (C) Copyright 2009-2016 ECMWF.
! 
! This software is licensed under the terms of the Apache Licence Version 2.0
! which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
! In applying this licence, ECMWF does not waive the privileges and immunities 
! granted to it by virtue of its status as an intergovernmental organisation nor
! does it submit to any jurisdiction.

!> Setup the QG model

!> The C++ code only knows about dimensional variables, whereas the QG model
!! is expressed in terms of non-dimensional variables.
!! Here, we query the configuration for the dimensional values, calculate the
!! equivalent non-dimensional values, and store them in the config structure.
!!
!! The convention used in the QG model is that a zero is appended to the
!! names of dimensional variables, whereas non-dimensional variable names do
!! not end in zero. (An exception to this convention are the layer depths:
!! D1 and D2.)
!!
!! The non-dimensionalisation is the usual one:
!! \f{eqnarray*}{
!!t &=& \tilde t \frac{\overline U}{L} \\\\
!!x &=& \frac{\tilde x}{L} \\\\
!!y &=& \frac{\tilde y}{L} \\\\
!!u &=& \frac{\tilde u}{\overline U} \\\\
!!v &=& \frac{\tilde v}{\overline U} \\\\
!!F_1 &=& \frac{f_0^2 L^2}{D_1 g \Delta\theta / {\overline\theta}} \\\\
!!F_2 &=& \frac{f_0^2 L^2}{D_2 g \Delta\theta / {\overline\theta}} \\\\
!!\beta &=& \beta_0 \frac{L^2}{\overline U}
!! \f}
!! where \f$ \overline U \f$, \f$ L \f$,
!! \f$ \Delta\theta / {\overline\theta} \f$, etc. are defined in
!! qg_constants.f90 .
!!
!! The Rossby number is \f$ \epsilon = {\overline U} / f_0 L \f$.

subroutine c_qg_setup(c_confspec, c_key_geom, c_key_confdata) bind (c,name='qg_setup_f90')

use qg_constants
use qg_configs
use qg_geom_mod
use iso_c_binding
use config_mod
use duration_mod
use kinds
use fckit_log_module, only : fckit_log

implicit none
type(c_ptr), intent(in)    :: c_confspec  !< pointer to object of class Config
integer(c_int), intent(in) :: c_key_geom    !< Geometry
integer(c_int), intent(inout) :: c_key_confdata  !< Key to configuration data

type(qg_config), pointer :: config
type(qg_geom), pointer :: geom

integer :: icentre, jcentre, ii, jj
real(kind=kind_real) :: distx, disty
type(duration) :: dtstep
character(len=20) :: ststep
character(len=160) :: record

! ------------------------------------------------------------------------------

call qg_geom_registry%get(c_key_geom, geom)
call qg_config_registry%init()
call qg_config_registry%add(c_key_confdata)
call qg_config_registry%get(c_key_confdata, config)

config%nx  = geom%nx
config%ny  = geom%ny
write(record,*)'c_qg_setup: nx, ny=',config%nx,config%ny
call fckit_log%info(record)

config%d1  = config_get_real(c_confspec,"top_layer_depth")
config%d2  = config_get_real(c_confspec,"bottom_layer_depth")
write(record,*)'c_qg_setup: d1, d2=',config%d1,config%d2
call fckit_log%info(record)

ststep = config_get_string(c_confspec,len(ststep),"tstep")
dtstep = trim(ststep)
config%dt0 = duration_seconds(dtstep)
write(record,*)'c_qg_setup: dt0=',config%dt0
call fckit_log%info(record)

config%dt = config%dt0 * ubar/scale_length
config%f1 = f0*f0*scale_length*scale_length/(g*dlogtheta*config%d1)
config%f2 = f0*f0*scale_length*scale_length/(g*dlogtheta*config%d2)
config%rsmax = horog/(rossby_number*config%d2)
config%deltax0 = domain_zonal/real(config%nx,kind_real)
config%deltay0 = domain_meridional/real(config%ny+1,kind_real)
config%deltax = config%deltax0/scale_length
config%deltay = config%deltay0/scale_length

!--- Orography = Gaussian hill centred on (icentre,jcentre)

allocate(config%rs(config%nx,config%ny))

icentre=config%nx/4
jcentre=3*config%ny/4
do jj=1,config%ny
  do ii=1,config%nx
    distx = real(min(icentre-ii,config%nx-(icentre-ii)),kind_real) &
         & *config%deltax0
    disty = real(abs(jj-jcentre),kind_real) * config%deltay0
    config%rs(ii,jj) = config%rsmax &
                    & *exp(-(distx*distx+disty*disty)/(worog*worog))
  enddo
enddo

! ------------------------------------------------------------------------------
return
end subroutine c_qg_setup
