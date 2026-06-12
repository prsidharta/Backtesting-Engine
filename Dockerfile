FROM ubuntu:22.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y \
    g++ \
    cmake \
    python3 \
    python3-pip

WORKDIR /app

COPY requirements.txt .
RUN pip3 install -r requirements.txt

COPY . .

RUN mkdir build && cd build && cmake .. && make

RUN chown -R 1000:1000 /app
USER 1000

CMD ./build/BacktestingEngine