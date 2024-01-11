#!/bin/bash

set -exo pipefail

if [ ! -d "HElib" ] ; then
    git clone https://github.com/homenc/HElib.git
fi

if [ ! -d "SEAL" ] ; then
    git clone https://github.com/microsoft/SEAL.git
fi

if [ ! -d "palisade-release" ] ; then
    git clone https://gitlab.com/palisade/palisade-release.git
fi

if [ ! -d "OpenFHE" ] ; then
    git clone https://github.com/openfheorg/openfhe-development.git OpenFHE
fi

if [ ! -d "tfhe" ] ; then
    git clone https://github.com/tfhe/tfhe.git
fi
