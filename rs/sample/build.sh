#!/bin/bash

INCDIR=/usr/local/include
LIBDIR=/usr/local/lib
LIBS='-lrealsense2 -lopencv_core -lopencv_imgproc -lopencv_highgui'

echo "build bgr..."
g++ -std=c++11 bgr.cpp \
    -I${INCDIR} -L${LIBDIR}  \
    ${LIBS} \
    -o bgr

echo "build ir..."
g++ -std=c++11 ir.cpp \
    -I${INCDIR} -L${LIBDIR}  \
    ${LIBS} \
    -o ir
