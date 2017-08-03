# OOPS
Object Oriented Prediction System

(C) Copyright 1996-2017 ECMWF.

This software is licensed under the terms of the Apache Licence Version 2.0
which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
In applying this licence, ECMWF does not waive the privileges and immunities
granted to it by virtue of its status as an intergovernmental organisation nor
does it submit to any jurisdiction.


--- Requirements ---

The instructions below assume that the following packages are available:
cmake
Boost  (1.47 or more recent, available at http://www.boost.org)
Eigen3 (available at http://eigen.tuxfamily.org/)

--- Building OOPS ---

The variables ${SRC} and ${BUILD} below must be defined for your environement.
Note: It is good practice to build the code outside of the source tree.

The lines below can be copied into a script or executed manually:

    export SRC=/path/to/source
    export BUILD=/path/to/build

    rm -rf ${BUILD}; mkdir ${BUILD}; cd ${BUILD}

    export PATH=${PATH}:${SRC}/ecbuild/bin

    ecbuild --build=release ${SRC}
    make -j4

Note: ecbuild accepts all cmake flags, for example, compilers can be selected with:
    ecbuild -DCMAKE_CXX_COMPILER=/path/to/gcc-5.2/bin/g++ --build=release ${SRC}

For testing your OOPS build:

    cd ${BUILD}/oops
    ctest

--- Working with OOPS ---

After OOPS has been built successfully once, and assuming the libraries are not
modified, changes in OOPS can be tested by re-running only:

    cd ${BUILD}/oops
    make -j4
    ctest

