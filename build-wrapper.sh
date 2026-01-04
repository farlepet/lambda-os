#!/bin/sh

os_name=`uname -s`

RUN_ARGS=""

if [ "$os_name" = "FreeBSD" ]; then
    if ! which podman > /dev/null; then
        echo "Must have Podman installed for FreeBSD!"
        exit 1
    fi
    if ! which sudo > /dev/null; then
        echo "Only 'sudo' is supported for priviledge escalation. This is required as Podman does not support rootless operation on FreeBSD."
        exit 1
    fi
    COMMAND="sudo podman"
    # We need to use Linuxulator on FreeBSD
    RUN_ARGS="--os linux"
else
    if which docker > /dev/null; then
        # Assume user has permissions
        COMMAND="docker"
    elif which podman > /dev/null; then
        # Assume rootless is supported
        COMMAND="podman"
    else
        echo "Must have either Docker or Podman installed!"
        exit 1
    fi
fi


TARGET=${LAMBDA_TARGET:-i686-elf}
TAG=${LAMBDA_TAG:-latest}

$COMMAND run $RUN_ARGS \
             -it --rm \
             -v ${PWD}:/code:rw \
             -w /code \
             -u $(id -u):$(id -g) \
             -e CROSS_COMPILE="/opt/lambda-cross/bin/$TARGET-" \
             -e HOST_CC="gcc" \
             -e HOST_AR="ar" \
             git.pfarley.dev/lambda-os/builder:$TAG-$TARGET \
             "$@"
