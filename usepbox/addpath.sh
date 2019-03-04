#!/bin/bash

# add LD_LIBRARY_PATH

BUILD=build

export LD_LIBRARY_PATH=${HOME}/src/ccbox/pbox/${BUILD}:${LD_LIBRARY_PATH}
echo $LD_LIBRARY_PATH
exec ${BUILD}/foo

