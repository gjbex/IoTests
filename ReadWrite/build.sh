#!/bin/bash -l

if [ -z "${VSC_INSITUTE}" ]
then
    module load foss/2014a
    module load HDF5/1.8.12-foss-2014a-serial
    HDF5_DIR="${EBROOTHDF5}"
else
    HDF5_DIR=/usr
fi

cd src/
make HDF5_DIR=${HDF5_DIR} clean all
cp writer ..
cp reader ..
