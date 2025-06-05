#!/bin/bash

PWD=`pwd`
cd ./out
sudo cmake --build . --target install
cd $PWD