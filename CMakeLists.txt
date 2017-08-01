# (C) Copyright 1996-2017 ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
# In applying this licence, ECMWF does not waive the privileges and immunities
# granted to it by virtue of its status as an intergovernmental organisation nor
# does it submit to any jurisdiction.

#
# OOPS bundle
#
cmake_minimum_required( VERSION 3.3.2 FATAL_ERROR )

set( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH};${CMAKE_CURRENT_SOURCE_DIR}/ecbuild/cmake)
include( ecbuild_bundle )
project( oops-bundle C CXX Fortran )

set( ENABLE_MPI OFF CACHE BOOL "Do not compile with MPI" )

ecbuild_bundle_initialize()

ecbuild_requires_macro_version( 2.5 )

ecbuild_bundle( PROJECT ecbuild       STASH "ecsdk/ecbuild"  TAG     2.6.2 )
ecbuild_bundle( PROJECT eckit         STASH "ecsdk/eckit"    TAG     0.16.2 )
ecbuild_bundle( PROJECT fckit         STASH "ecsdk/fckit"    TAG     0.2.2 )
ecbuild_bundle( PROJECT oops          STASH "oops/oops"      TAG     0.1.0 )

ecbuild_bundle_finalize()
