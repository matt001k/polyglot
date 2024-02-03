from ubuntu:20.04

VOLUME ["/polyglot"]
WORKDIR /polyglot
RUN cd /polyglot

ARG DEBIAN_FRONTEND=noninteractive
RUN apt-get update --fix-missing -q
RUN apt-get install --no-install-recommends -y \
    software-properties-common \
    build-essential \
    git \
    symlinks \
    expect \
    curl \
    cmake \
    ruby-full \
    python3.9 \
    python3-pip \
    openssh-client \
    flex \
    bison \
    doxygen \
    astyle
RUN gem install ceedling
RUN pip3 install gcovr==4.1

COPY ./run.sh /run.sh
RUN chmod +x /run.sh

ENTRYPOINT ["/run.sh"]
CMD ["-t"]
