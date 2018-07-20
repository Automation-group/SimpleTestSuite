#!/bin/bash

# built qextserialport
cd ../qextserialport/
qmake-qt4
make -j5

#build qlfDevise
cd ../qlfdevices/
cmake .
make -j5


#build SimpleTestSuite
cd ../creepTest/
cmake -DQLFDEVICES_DIR=../qlfdevices/ -DQEXTSERIALPORT_DIR=../qextserialport/ .
make -j5

#package SimpleTestSuite
make package