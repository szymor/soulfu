#!/bin/bash

# Debian AMD64 package build script for SoulFu
# Usage: VERSION=1.7.1 ./packaging/build_deb-amd64.sh

# Check if version is set via environment variable
if [ -z "$VERSION" ]; then
    echo "Error: VERSION environment variable not set"
    echo "Usage: VERSION=<version> $0"
    exit 1
fi

PACKAGE_NAME="soulfu-bftd"
ARCHITECTURE="amd64"
MAINTAINER="xmeadow <eke@rosauntier.com>"
DESCRIPTION="A 3D action role-playing hack and slash dungeon crawler made by Aaron Bishop."
DEPENDENCIES="libsdl2-2.0-0, libsdl2-net-2.0-0, libvorbis0a, libgl1, libogg0, libjpeg62-turbo | libjpeg62"

# Compile with release flag
rm -f soulfu
PKG_CONFIG_PATH=/usr/lib/x86_64-linux-gnu/pkgconfig/ make release

# Copy icon from packaging directory if it exists
if [ -f "packaging/icons/hicolor/128x128/apps/soulfu.png" ]; then
    cp packaging/icons/hicolor/128x128/apps/soulfu.png .
else
    echo "Error: Icon not found at packaging/icons/hicolor/128x128/apps/soulfu.png"
    exit 1
fi

# Create directory structure
mkdir -p ${PACKAGE_NAME}/DEBIAN
mkdir -p ${PACKAGE_NAME}/usr/share/doc/${PACKAGE_NAME}
mkdir -p ${PACKAGE_NAME}/usr/share/soulfu
mkdir -p ${PACKAGE_NAME}/usr/share/applications
mkdir -p ${PACKAGE_NAME}/usr/local/games/

# Check if files exist and copy them
if [ -f "soulfu" ] && [ -f "Manual.htm" ] && [ -f "datafile.sdf" ] && [ -f "soulfu.png" ]; then
    cp soulfu ${PACKAGE_NAME}/usr/local/games/
    chmod +x ${PACKAGE_NAME}/usr/local/games/soulfu
    cp datafile.sdf ${PACKAGE_NAME}/usr/share/soulfu
    cp Manual.htm ${PACKAGE_NAME}/usr/share/doc/${PACKAGE_NAME}/
    cp soulfu.png ${PACKAGE_NAME}/usr/share/soulfu
else
    echo "Compilation might have failed. One or more required files are missing."
    exit 1
fi

# Create .desktop file
cat <<EOL > ${PACKAGE_NAME}/usr/share/applications/soulfu.desktop
[Desktop Entry]
Type=Application
Name=Soulfu
Comment= Play a 3D action role-playing hack and slash dungeon crawler.
Exec=/usr/local/games/soulfu
Icon=/usr/share/soulfu/soulfu.png
Terminal=false
Categories=Game;
Keywords=rpg;
EOL

# Create control file
cat <<EOL > ${PACKAGE_NAME}/DEBIAN/control
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
dpkg-deb --build ${PACKAGE_NAME}

# Move package to packaging/bin with version in filename
if mv ${PACKAGE_NAME}.deb packaging/bin/${PACKAGE_NAME}_${VERSION}_${ARCHITECTURE}.deb; then
    echo "Package ${PACKAGE_NAME}_${VERSION}_${ARCHITECTURE}.deb created successfully in packaging/bin/"
else
    echo "Error: Failed to move package to packaging/bin/"
    # Clean up leftover .deb file in current directory
    rm -f ${PACKAGE_NAME}.deb
    exit 1
fi

# Clean up
rm -rf ${PACKAGE_NAME}
rm -f soulfu soulfu.png
