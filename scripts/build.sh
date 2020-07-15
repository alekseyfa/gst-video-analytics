# ==============================================================================
<<<<<<< HEAD
# Copyright (C) 2018-2020 Intel Corporation
=======
# Copyright (C) 2018-2019 Intel Corporation
>>>>>>> 1bdbbc8... Release 2020.4
#
# SPDX-License-Identifier: MIT
# ==============================================================================

BASEDIR=$(dirname "$0")/..

[ ! -d "${BASEDIR}/build" ] && mkdir ${BASEDIR}/build
cd ${BASEDIR}/build
cmake ..
make -j $(nproc)
