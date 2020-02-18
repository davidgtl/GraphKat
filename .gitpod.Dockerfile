FROM gitpod/workspace-full-vnc
                    
USER gitpod

RUN sudo apt-get -q update && sudo apt-get install -yq xorg-dev libglu1-mesa-dev

# Install custom tools, runtime, etc. using apt-get
# For example, the command below would install "bastet" - a command line tetris clone:
#
# RUN sudo apt-get -q update && #     sudo apt-get install -yq bastet && #     sudo rm -rf /var/lib/apt/lists/*
#
# More information: https://www.gitpod.io/docs/config-docker/
