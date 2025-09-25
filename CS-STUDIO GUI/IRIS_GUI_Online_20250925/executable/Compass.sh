#!/bin/bash

# The key-based authentication for ssh forwarding is enabled by ssh-keygen, the private key pair is stored in ~/.ssh/id_rsa, so that no password authentication is needed in the script
REMOTE_USER="isolpharm_iris"
REMOTE_HOST="10.23.14.8"
REMOTE_CMD="compass" #If the first calling crashes, try second time

ssh -Y "${REMOTE_USER}@${REMOTE_HOST}" "${REMOTE_CMD}"

# Directly use it on local machine
# ${REMOTE_CMD}
