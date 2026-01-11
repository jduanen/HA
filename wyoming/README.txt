* follow best practices for docker compose files that are to run on either cpu- or gpu-based
  - put common code into 'docker-compose.yml'
  - put cpu-/gpu-specific code into 'docker-compose.cpu.yml' and 'docker-compose.gpu.yml'
  - to run the containers on the CPU
    * `docker compose -f docker-compose.yml -f docker-compose.cpu.yml up -d`
  - to run the containers on the GPU
    * `docker compose -f docker-compose.yml -f docker-compose.gpu.yml up -d`

* common docker compose commands
  - start the STT and TTS containers for HA and run them in background
    * `docker compose up -d`
  - stop STT and TTS services and remove container instances
    * `docker compose down`
  - print logs from both containers
    * `docker compose logs`
  - print status of the running containers
    * `docker compose ps`
  - show the top processes in both containers
    * `docker compose top`
  - update both containers
    * `docker compose pull`
  - run a real-time monitor of the resource usage of the containers
    * `docker compose stats`
