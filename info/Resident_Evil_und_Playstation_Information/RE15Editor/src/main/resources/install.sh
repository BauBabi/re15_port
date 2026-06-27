#!/bin/bash

wget -qO - https://adoptopenjdk.jfrog.io/adoptopenjdk/api/gpg/key/public | sudo apt-key add -
sudo add-apt-repository --yes https://adoptopenjdk.jfrog.io/adoptopenjdk/deb/
sudo apt update

sudo apt -y install adoptopenjdk-11-hotspot # Java 11 / HotSpot VM
