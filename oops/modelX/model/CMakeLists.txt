list( APPEND oops_xxxx_src_files
XxxxFields.cc
XxxxFields.h
XxxxFortran.h
XxxxGeometry.cc
XxxxGeometry.h
XxxxIncrement.cc
XxxxIncrement.h
XxxxState.cc
XxxxState.h
XxxxTraits.h
XxxxVariables.cc
XxxxVariables.h
calc_pv.f90
laplacian_2d.f90
pv_operator.f90
xxxx_constants.f90
xxxx_fields.F90
xxxx_fields_interface.F90
xxxx_geom_mod.F90
xxxx_invent_state.f90
xxxx_vars_mod.F90
)

if ( CMAKE_Fortran_COMPILER_ID MATCHES "Cray" )
  set( cfdealloc C_F_NO_DEALLOC )
endif()

ecbuild_add_library( TARGET   xxxx
                     DEFINITIONS ${cfdealloc}
                     SOURCES  ${oops_xxxx_src_files}
                     LIBS     oops ${LAPACK_LIBRARIES}
                     INSTALL_HEADERS LISTED
                     LINKER_LANGUAGE ${OOPS_LINKER_LANGUAGE}
                    )

