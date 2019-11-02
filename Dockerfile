FROM ubuntu:xenial as dev

LABEL MAINTAINER="sklkim@ucdavis.edu"

ENV MAKEFLAGS "-j8"

FROM skkeem/pikos:dev as build
COPY . /pikos_popl2020
WORKDIR /pikos_popl2020
RUN bash install_dependencies.sh && adduser root benchexec
RUN curl https://bootstrap.pypa.io/get-pip.py | python3.6 \
    && python3 -m pip install pandas matplotlib scipy --user
RUN bash install.sh
RUN bash extract_benchmarks.sh

ENTRYPOINT ["/bin/bash"]
