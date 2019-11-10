FROM debian:10

# Avoid warnings by switching to noninteractive
ENV DEBIAN_FRONTEND=noninteractive

# Or your actual UID, GID on Linux if not the default 1000
ARG USERNAME=vscode
ARG USER_UID=1000
ARG USER_GID=$USER_UID

# Configure apt and install packages
RUN apt-get update \
    && apt-get -y install --no-install-recommends apt-utils dialog 2>&1 \
    # 
    # Verify git, process tools, lsb-release (useful for CLI installs) installed
    && apt-get -y install git procps lsb-release \
    #
    # Install C++ tools
    && apt-get -y install build-essential zlib1g-dev cmake cppcheck valgrind libcurl4-openssl-dev \
       libssl-dev openssl libcurl4-openssl-dev uuid-dev libpulse-dev python3 python3-pip \
    && pip3 install awscli \
    # Create a non-root user to use if preferred - see https://aka.ms/vscode-remote/containers/non-root-user.
    && groupadd --gid $USER_GID $USERNAME \
    && useradd -s /bin/bash --uid $USER_UID --gid $USER_GID -m $USERNAME \
    # [Optional] Add sudo support for non-root user
    && apt-get install -y sudo \
    && echo $USERNAME ALL=\(root\) NOPASSWD:ALL > /etc/sudoers.d/$USERNAME \
    && chmod 0440 /etc/sudoers.d/$USERNAME \
    #
    # Clean up
    && apt-get autoremove -y \
    && apt-get clean -y \
    && rm -rf /var/lib/apt/lists/*

# Install AWS SDK
WORKDIR /opt/deps/
RUN git clone --recurse https://github.com/aws/aws-sdk-cpp.git
WORKDIR /opt/deps/aws-sdk-cpp/build
RUN cmake -DCMAKE_BUILD_TYPE=Debug -DBUILD_ONLY="s3" .. && \
    make -j $(nproc) && \
    make install

# Switch back to dialog for any ad-hoc use of apt-get
ENV DEBIAN_FRONTEND=
ENV LD_LIBRARY_PATH /usr/local/lib
ENV LIBRARY_PATH /usr/local/lib
