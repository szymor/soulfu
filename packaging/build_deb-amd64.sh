#!/bin/bash

# Debian AMD64 package build script for SoulFu
# Usage: VERSION=1.7.1 ./packaging/build_deb-amd64.sh

# Check if version is set via environment variable
if [ -z "$VERSION" ]; then
    echo "Error: VERSION environment variable not set"
    echo "Usage: VERSION=<version> $0"
    exit 1
fi

PACKAGE_NAME="soulfu"
STAGING_DIR="${PACKAGE_NAME}-pkg"
ARCHITECTURE="amd64"
MAINTAINER="xmeadow <eke@rosauntier.com>"
DESCRIPTION="A 3D action role-playing hack and slash dungeon crawler made by Aaron Bishop."
DEPENDENCIES="libsdl2-2.0-0, libsdl2-net-2.0-0, libvorbis0a, libgl1, libogg0, libjpeg62-turbo | libjpeg62"

# Compile with release flag
rm -f soulfu
PKG_CONFIG_PATH=/usr/lib/x86_64-linux-gnu/pkgconfig/ make release

# Create directory structure
mkdir -p ${STAGING_DIR}/DEBIAN
mkdir -p ${STAGING_DIR}/usr/share/doc/${PACKAGE_NAME}
mkdir -p ${STAGING_DIR}/usr/share/soulfu
mkdir -p ${STAGING_DIR}/usr/share/applications
mkdir -p ${STAGING_DIR}/usr/games/
mkdir -p ${STAGING_DIR}/usr/share/icons/hicolor/16x16/apps/
mkdir -p ${STAGING_DIR}/usr/share/icons/hicolor/24x24/apps/
mkdir -p ${STAGING_DIR}/usr/share/icons/hicolor/32x32/apps/
mkdir -p ${STAGING_DIR}/usr/share/icons/hicolor/48x48/apps/
mkdir -p ${STAGING_DIR}/usr/share/icons/hicolor/64x64/apps/
mkdir -p ${STAGING_DIR}/usr/share/icons/hicolor/96x96/apps/
mkdir -p ${STAGING_DIR}/usr/share/icons/hicolor/128x128/apps/

# Check if files exist and copy them
if [ -f "soulfu" ] && [ -f "Manual.htm" ] && [ -f "datafile.sdf" ] && [ -f "soulfu.jpg" ]; then
    cp soulfu ${STAGING_DIR}/usr/games/
    chmod +x ${STAGING_DIR}/usr/games/soulfu
    cp datafile.sdf ${STAGING_DIR}/usr/share/soulfu
    cp Manual.htm ${STAGING_DIR}/usr/share/doc/${PACKAGE_NAME}/
    cp soulfu.jpg ${STAGING_DIR}/usr/share/doc/${PACKAGE_NAME}/
    cp packaging/icons/hicolor/16x16/apps/soulfu.png ${STAGING_DIR}/usr/share/icons/hicolor/16x16/apps/
    cp packaging/icons/hicolor/24x24/apps/soulfu.png ${STAGING_DIR}/usr/share/icons/hicolor/24x24/apps/
    cp packaging/icons/hicolor/32x32/apps/soulfu.png ${STAGING_DIR}/usr/share/icons/hicolor/32x32/apps/
    cp packaging/icons/hicolor/48x48/apps/soulfu.png ${STAGING_DIR}/usr/share/icons/hicolor/48x48/apps/
    cp packaging/icons/hicolor/64x64/apps/soulfu.png ${STAGING_DIR}/usr/share/icons/hicolor/64x64/apps/
    cp packaging/icons/hicolor/96x96/apps/soulfu.png ${STAGING_DIR}/usr/share/icons/hicolor/96x96/apps/
    cp packaging/icons/hicolor/128x128/apps/soulfu.png ${STAGING_DIR}/usr/share/icons/hicolor/128x128/apps/
else
    echo "Compilation might have failed. One or more required files are missing."
    exit 1
fi

# Create .desktop file
cp packaging/soulfu.desktop ${STAGING_DIR}/usr/share/applications/soulfu.desktop

# Create control file
cat <<EOL > ${STAGING_DIR}/DEBIAN/control
Package: ${PACKAGE_NAME}
Version: ${VERSION}
Section: base
Priority: optional
Architecture: ${ARCHITECTURE}
Depends: ${DEPENDENCIES}
Maintainer: ${MAINTAINER}
Description: ${DESCRIPTION}
EOL

# Create packaging/bin directory
mkdir -p packaging/bin

# Build the package
fakeroot dpkg-deb --build ${STAGING_DIR}

# Move package to packaging/bin with version in filename
if mv ${STAGING_DIR}.deb packaging/bin/${PACKAGE_NAME}_${VERSION}_${ARCHITECTURE}.deb; then
    echo "Package ${PACKAGE_NAME}_${VERSION}_${ARCHITECTURE}.deb created successfully in packaging/bin/"
else
    echo "Error: Failed to move package to packaging/bin/"
    # Clean up leftover .deb file in current directory
    rm -f ${STAGING_DIR}.deb
    exit 1
fi

# Clean up
rm -rf ${STAGING_DIR}
rm -f soulfu
