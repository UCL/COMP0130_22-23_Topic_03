#!/bin/bash

sudo apt update
sudo apt install cmake build-essential libeigen3-dev libboost-dev libboost-filesystem-dev libblas-dev liblapack-dev libopencv-dev libglew-dev mesa-utils libgl1-mesa-glx unzip python3-pil.imagetk
pip3 install evo --upgrade --no-binary evo
