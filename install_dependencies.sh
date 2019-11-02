#!/bin/bash
cd /tmp

# Upgrade
apt-get update
apt-get upgrade -y

apt-get install -y software-properties-common

# Add ppa for llvm 8.0
echo "deb http://apt.llvm.org/xenial/ llvm-toolchain-xenial-8 main" > /etc/apt/sources.list.d/llvm-8.list

# Add llvm repository key
apt-get install -y wget gnupg
wget -O - https://apt.llvm.org/llvm-snapshot.gpg.key | apt-key add -

add-apt-repository -y ppa:deadsnakes/ppa
add-apt-repository -y ppa:ubuntu-toolchain-r/test
apt-get update

# Install all dependencies
apt-get install -y gcc-8 g++-8 cmake libgmp-dev libtbb-dev \
   libboost-dev libboost-filesystem-dev libboost-test-dev \
   python3.6 python3.6-dev \
   curl bc autoconf automake m4 libtool pkg-config \
   libsqlite3-dev libz-dev libedit-dev llvm-8 llvm-8-dev llvm-8-tools clang-8 \
   texlive dvipng texlive-latex-extra

update-alternatives --install /usr/bin/python3 python3 /usr/bin/python3.6 60 \
  && update-alternative --set python3 /usr/bin/python3.6

update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-8 60 \
                    --slave /usr/bin/g++ g++ /usr/bin/g++-8 \
  && update-alternatives --config gcc

# TCMalloc
git clone https://github.com/gperftools/gperftools.git
cd gperftools \
  && ./autogen.sh ; ./autogen.sh \
  && ./configure \
  && make \
  && make install \
  && cd .. && rm -rf gperftools

# BenchExec
apt-get install -y python3-tempita python3-yaml lxcfs
wget https://github.com/sosy-lab/benchexec/releases/download/1.18/benchexec_1.18-1_all.deb
dpkg -i benchexec_1.18-1_all.deb
