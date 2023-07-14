# Build stage 

FROM alpine:3.17.0 AS BUILD

RUN apk update && \
  apk add --no-cache \
    build-base \
    cmake \
    boost-dev

WORKDIR /stashserver

COPY src/ ./src/
COPY include ./include/
COPY CMakeLists.txt .

WORKDIR /stashserver/build

RUN cmake -DCMAKE_BUILD_TYPE=Release .. && \
  cmake --build . --parallel 8

# Server image

FROM alpine:3.17.0

RUN apk update && \
    apk add --no-cache \
    boost

RUN addgroup -S shs && adduser -S shs -G shs
USER shs

COPY --chown=shs:shs --from=build \
    ./stashserver/build/stashserver \
    ./app/

ENTRYPOINT [ "./app/stashserver", "3030" ]

EXPOSE 3030

