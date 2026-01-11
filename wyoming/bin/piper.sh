#!/bin/bash
#
# Script to start the TTS container (optionally with gpu acceleration) for the
#  Home Assistant Voice Assistant
#
# Usage: piper.sh {-n <voiceName>} {-p <hostPortNum>} {-g} {-v}

PIPER_CPU="rhasspy/wyoming-piper"
PIPER_GPU="ghcr.io/slackr31337/wyoming-piper-gpu:latest"
DEF_PIPER_PORT=10200
DEF_PIPER_VOICE="en_US-lessac-medium"
  # en_US-amy-medium: English, medium-sized, female
  # en_US-lessac-medium: English, medium-sized, ????
  # en_US-lessac-small: English, small-sized, ????
  # de_DE-jonas: German, male voice

PIPER_EXEC=$PIPER_CPU
PIPER_PORT=$DEF_PIPER_PORT
PIPER_VOICE=$DEF_PIPER_VOICE

while getopts "n:p:gv" opt; do
  case "$opt" in
    n) PIPER_VOICE="$OPTARG" ;;
    p) PIPER_PORT="$OPTARG" ;;
    g) PIPER_EXEC="--runtime=nvidia --gpus all $PIPER_GPU" ;;
    v) VERBOSE=1 ;;
    *) echo "Usage: $0 {-n <TTSvoiceName>} {-p <hostPortNum>} {-g} {-v}"; exit 1 ;;
  esac
done

if [ $VERBOSE ]; then
   echo "PIPER_VOICE: ${PIPER_VOICE}"
   echo "PIPER_PORT: ${PIPER_PORT}"
   echo "PIPER_EXEC: ${PIPER_EXEC}"
fi

docker run -d --name wyoming-piper \
       -p ${PIPER_PORT}:10200 \
       --voice ${PIPER_VOICE} \
       -v /home/jdn/Data2/PiperData:/data \
       ${PIPER_EXEC}
if [ $? -ne 0 ]; then
    echo "ERROR: Failed to start the Piper container for GPU execution"
    exit 1
fi
