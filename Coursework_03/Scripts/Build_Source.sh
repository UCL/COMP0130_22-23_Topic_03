#!/bin/bash

if [ "$#" -ne 1 ];
then
  build_type=Debug
else
  build_type=$1
fi

if [ "$build_type" != "Debug" ] && [ "$build_type" != "Release" ]
then
  echo Unknown build type ${build_type}
  exit 1
fi

# Check if the vocabulary file exists. If not, uncompress it

if [ ! -f "Source/Vocabulary/ORBvoc.txt" ]
then
    echo "Uncompressing vocabulary file."
    unzip Source/Vocabulary/ORBvoc.txt.zip
    mv ORBvoc.txt Source/Vocabulary
fi

# Heuristic to suggest how many CPUs to use - take
# total memory, divide by 4GB, and round up
unameOut="$(uname -s)"
case "${unameOut}" in
    Linux*)     num_jobs=`free -m | grep Mem | awk '{print int(($2)/4096)+1}'`;;
    Darwin*)    num_jobs=`sysctl hw.memsize | awk '{print int(($2)/4294967296)+1}'`;;
    *)          num_jobs=2
esac

echo Build type ${build_type} using ${num_jobs} parallel jobs
root_dir=`pwd`
mkdir -p Build/${build_type}/Source
pushd Build/${build_type}/Source > /dev/null
cmake "${root_dir}/Source" -DCMAKE_BUILD_TYPE=${build_type} -DCMAKE_INSTALL_PREFIX=${BUILD_INSTALL_PREFIX}
make -j${num_jobs}
make install
popd
