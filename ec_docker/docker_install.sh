#!/bin/bash

# Make sure the script is run as root.
if [ $USER != "root" ] ; then
    echo "Please run as root."
    exit
fi

# Note: docker installation instructions come from https://docs.docker.com/install/linux/docker-ce/ubuntu/

# Update apt get
apt-get update

# Remove any old versions of docker
# NOTE: some newer versions require removing docker-ce
sudo apt-get remove docker docker-ce docker-engine docker.io containerd runc

# The above command doesn't quite do everything it needs to. The additional steps to purge
# docker completely (from https://askubuntu.com/questions/935569/how-to-completely-uninstall-docker)
# are necessary.
sudo apt-get purge -y docker-engine docker docker.io docker-ce
sudo apt-get autoremove -y --purge docker-engine docker docker.io docker-ce
sudo rm -rf /var/lib/docker /etc/docker
sudo rm /etc/apparmor.d/docker
sudo groupdel docker
sudo rm -rf /var/run/docker.sock

# Install packages to allow apt to use a repository over HTTPS
sudo apt-get install \
    apt-transport-https \
    ca-certificates \
    curl \
    gnupg-agent \
    software-properties-common

# Add Docker’s official GPG key
sudo curl -fsSL https://download.docker.com/linux/ubuntu/gpg | sudo apt-key add -

# Check for key fingerprint
sudo apt-key fingerprint 0EBFCD88

# Set up the stable repository
sudo add-apt-repository \
   "deb [arch=amd64] https://download.docker.com/linux/ubuntu \
   $(lsb_release -cs) \
   stable"

# Update and install the latest docker engine
sudo apt-get update
sudo apt-get install docker-ce docker-ce-cli containerd.io

# Try to run a check
sudo docker run hello-world
