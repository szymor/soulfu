#!/bin/bash

# Linux AMD64 tar.gz build script for SoulFu
# Usage: VERSION=1.8 ./packaging/build_linux-amd64.sh

# Check if version is set via environment variable
if [ -z "$VERSION" ]; then
    echo "Error: VERSION environment variable not set"
    echo "Usage: VERSION=<version> $0"
    exit 1
fi

PACKAGE_NAME="soulfu"
ARCHITECTURE="amd64"
STAGING_DIR="${PACKAGE_NAME}-${VERSION}-linux-${ARCHITECTURE}"

# Compile with release flag
rm -f soulfu
PKG_CONFIG_PATH=/usr/lib/x86_64-linux-gnu/pkgconfig/ make release

# Check if required files exist
if [ ! -f "soulfu" ] || [ ! -f "datafile.sdf" ]; then
    echo "Compilation might have failed. One or more required files are missing."
    exit 1
fi

# Create directory structure
mkdir -p ${STAGING_DIR}

# Copy game files
cp soulfu ${STAGING_DIR}/
chmod +x ${STAGING_DIR}/soulfu
cp datafile.sdf ${STAGING_DIR}/
[ -f "Manual.htm" ] && cp Manual.htm ${STAGING_DIR}/
[ -f "soulfu.jpg" ] && cp soulfu.jpg ${STAGING_DIR}/
[ -f "packaging/license.txt" ] && cp packaging/license.txt ${STAGING_DIR}/

# Create packaging/bin directory
mkdir -p packaging/bin

# Build the tar.gz
TARBALL="packaging/bin/${PACKAGE_NAME}_${VERSION}_linux-${ARCHITECTURE}.tar.gz"
tar czf "${TARBALL}" ${STAGING_DIR}

if [ $? -eq 0 ]; then
    echo "Package ${TARBALL} created successfully"
else
    echo "Error: Failed to create tar.gz"
    rm -rf ${STAGING_DIR}
    exit 1
fi

# Clean up
rm -rf ${STAGING_DIR}
rm -f soulfu
