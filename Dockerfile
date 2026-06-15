FROM ubuntu:22.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y \
    g++ \
    cmake \
    python3 \
    python3-pip \
    git \
    libcurl4-openssl-dev \
    libssl-dev \
    uuid-dev \
    zlib1g-dev

RUN git clone --recurse-submodules https://github.com/aws/aws-sdk-cpp.git && \
    cd aws-sdk-cpp && \
    mkdir build && cd build && \
    cmake -DCMAKE_BUILD_TYPE=Release -DBUILD_ONLY="s3" -DBUILD_SHARED_LIBS=OFF .. && \
    make -j$(nproc) && \
    make install && \
    cd ../.. && rm -rf aws-sdk-cpp

WORKDIR /app

COPY requirements.txt .
RUN pip3 install -r requirements.txt

COPY . .

RUN mkdir build && cd build && cmake .. && make

RUN chown -R 1000:1000 /app
USER 1000

CMD ["./build/BacktestingEngine"]