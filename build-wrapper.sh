#!/bin/sh

TARGET=${LAMBDA_TARGET:-i686-elf}
TAG=${LAMBDA_TAG:-latest}

docker run -it --rm \
           -v ${PWD}:/code:rw \
           -w /code \
           -u $(id -u):$(id -g) \
           -e CROSS_COMPILE="/opt/lambda-cross/bin/$TARGET-" \
           git.pfarley.dev/lambda-os/builder:$TAG-$TARGET \
           "$@"
