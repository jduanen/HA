#!/bin/bash
# Script to start up the local STT container (with optional GPU acceleration)
#  Home Assistant Voice Assistant
#
# Usage: whisper.sh {-m <modelName>} {-p <hostPortNumber>} {-d <dataPath>} {-g} {-v}

WHISPER_CPU="rhasspy/wyoming-whisper"
WHISPER_GPU="ghcr.io/slackr31337/wyoming-whisper-gpu:latest"
DEF_WHISPER_PORT=10300
DEF_WHISPER_MODEL="medium-int8"
  #  * tiny-int8 (43 MB): smallest, fastest, low accuracy
  #  * tiny (152 MB):
  #  * base-int8 (80 MB):
  #  * base (291 MB): 
  #  * small-int8 (255 MB):
  #  * small (968 MB):
  #  * medium-int8 (786 MB):
  #  * medium (3.1 GB): larger, slower, more accurate
  #  * large (not always supported or practical on smaller devices)
DEF_WHISPER_DATA="${HOME}/wyoming/WhisperData"

WHISPER_EXEC=$WHISPER_CPU
WHISPER_PORT=$DEF_WHISPER_PORT
WHISPER_MODEL=$DEF_WHISPER_MODEL
WHISPER_DATA=$DEF_WHISPER_DATA

while getopts "m:p:d:gv" opt; do
  case "$opt" in
    m) WHISPER_MODEL="$OPTARG" ;;
    p) WHISPER_PORT="$OPTARG" ;;
    d) WHISPER_DATA="$OPTARG" ;;
    g) WHISPER_EXEC="--runtime=nvidia --gpus all $WHISPER_GPU" ;;
    v) VERBOSE=1 ;;
    *) echo "Usage: $0 {-m <modelName>} {-p <hostPortNumber>} {-d <dataPath>} {-g} {-v}"; exit 1 ;;
  esac
done

if [ $VERBOSE ]; then
   echo "WHISPER_MODEL: $WHISPER_MODEL"
   echo "WHISPER_PORT: $WHISPER_PORT"
   echo "WHISPER_DATA: $WHISPER_DATA"
   echo "WHISPER_EXEC: $WHISPER_EXEC"
fi

docker run -d --name wyoming-whisper \
       -p $WHISPER_PORT:10300 \
       --model $WHISPER_MODEL --language en --beam_size 5 \
       --compute_type int8 \
       --restart unless-stopped \
       -v $WHISPER_DATA:/data \
       $WHISPER_EXEC
if [ $? -ne 0 ]; then
    echo "ERROR: Failed to start the Whisper container"
    exit 1
fi
