#!/bin/bash
#
# Script to update the contents of the STT and TTS containers


CONTAINER_PREFIX=""
CONTAINER_PREFIX="rhasspy/wyoming-"


docker pull ${CONTAINER_PREFIX}piper:latest

docker pull ${CONTAINER_PREFIX}-whisper:latest
