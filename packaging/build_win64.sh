#!/bin/bash

# Windows 64-bit build script for SoulFu
# Usage: VERSION=1.7.1 ./packaging/build_win64.sh

# Check if version is set via environment variable
if [ -z "$VERSION" ]; then
    echo "Error: VERSION environment variable not set"
    echo "Usage: VERSION=<version> $0"
    exit 1
fi

INSTALLER_DIR="soulfu_win64"
ICON_FILE="Chest.ico"
OUTPUT_FILE="soulfu_win64_$VERSION.exe"
ZIP_FILE="soulfu_win64_$VERSION.zip"

# Define publisher, homepage, and company name
PUBLISHER="SoulFu: Back From The Dead Project"
HOMEPAGE="https://github.com/szymor/soulfu"
COMPANY_NAME="SoulFu: Back From The Dead Project"

# Clean previous build but preserve datafile.sdf
rm -f soulfu.exe
make -f Makefile.mingw64

# Check if compilation succeeded
if [ ! -f "soulfu.exe" ]; then
    echo "Error: Compilation failed - soulfu.exe not found"
    exit 1
fi

# Check if required files exist
if [ ! -f "datafile.sdf" ]; then
    echo "Error: datafile.sdf not found"
    exit 1
fi

if [ ! -f "Manual.htm" ]; then
    echo "Error: Manual.htm not found"
    exit 1
fi

if [ ! -f "soulfu.jpg" ]; then
    echo "Error: soulfu.jpg not found"
    exit 1
fi

if [ ! -f "$ICON_FILE" ]; then
    echo "Error: $ICON_FILE not found"
    exit 1
fi

# Use DLLs from packaging/win64 directory if available, otherwise try default path
DLL_DIR="packaging/win64"
MINGW_BIN_PATH="/usr/local/x86_64-w64-mingw32/bin"
DLLS_COPIED=0

echo "Looking for required DLLs..."

# Try packaging/win64 directory first
if [ -f "$DLL_DIR/SDL2.dll" ] && [ -f "$DLL_DIR/SDL2_net.dll" ] && \
   [ -f "$DLL_DIR/libogg-0.dll" ] && [ -f "$DLL_DIR/libvorbis-0.dll" ]; then
    echo "Using DLLs from $DLL_DIR"
    cp "$DLL_DIR/SDL2.dll" .
    cp "$DLL_DIR/SDL2_net.dll" .
    cp "$DLL_DIR/libogg-0.dll" .
    cp "$DLL_DIR/libvorbis-0.dll" .
    DLLS_COPIED=1
elif [ -d "$MINGW_BIN_PATH" ]; then
    # Try MinGW binary directory
    echo "Using DLLs from MinGW directory: $MINGW_BIN_PATH"
    for dll in "SDL2.dll" "SDL2_net.dll" "libogg-0.dll" "libvorbis-0.dll"; do
        if [ -f "$MINGW_BIN_PATH/$dll" ]; then
            cp "$MINGW_BIN_PATH/$dll" .
        else
            echo "Error: $dll not found in $MINGW_BIN_PATH"
            exit 1
        fi
    done
    DLLS_COPIED=1
else
    echo "Error: No DLLs found. Please ensure DLLs are in $DLL_DIR/ or MinGW is installed."
    exit 1
fi

# Create temporary installer directory
mkdir -p "$INSTALLER_DIR"

# Copy required files to the installer directory
echo "Copying files to installer directory..."
cp soulfu.exe "$INSTALLER_DIR/"
cp datafile.sdf "$INSTALLER_DIR/"
cp Manual.htm "$INSTALLER_DIR/"
cp soulfu.jpg "$INSTALLER_DIR/"
cp SDL2.dll "$INSTALLER_DIR/"
cp SDL2_net.dll "$INSTALLER_DIR/"
cp libogg-0.dll "$INSTALLER_DIR/"
cp libvorbis-0.dll "$INSTALLER_DIR/"
cp "$ICON_FILE" "$INSTALLER_DIR/"

# Prepare license file (always needed for zip and potentially for NSIS)
LICENSE_SOURCE=""
LICENSE_TEMP_CREATED=0

if [ -f "packaging/license.txt" ]; then
    echo "Using license.txt from packaging directory"
    LICENSE_SOURCE="packaging/license.txt"
elif [ -f "license.txt" ]; then
    echo "Using license.txt from current directory"
    LICENSE_SOURCE="license.txt"
else
    echo "Creating license.txt from GPL header..."
    cat > license.txt << 'EOF'
SoulFu - 3D Rogue-like dungeon crawler
Copyright (C) 2007 Aaron Bishop

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
EOF
    LICENSE_SOURCE="license.txt"
    LICENSE_TEMP_CREATED=1
fi

# Copy license to installer directory
cp "$LICENSE_SOURCE" "$INSTALLER_DIR/license.txt"

# For NSIS, we need license.txt in current directory
# For zip, we'll use the one in installer directory
if [ "$LICENSE_SOURCE" != "license.txt" ]; then
    # Copy to current directory for NSIS and zip
    cp "$LICENSE_SOURCE" license.txt
    LICENSE_TEMP_CREATED=1
fi

# Check if NSIS is available
if ! command -v makensis >/dev/null 2>&1; then
    echo "Warning: NSIS (makensis) not found, skipping installer creation"
    echo "Creating ZIP archive only..."
    ZIP_ONLY=1
else
    # Build the installer using NSIS
    echo "Building installer with NSIS..."
    makensis -NOCD -DVERSION="$VERSION" -DPUBLISHER="$PUBLISHER" -DHOMEPAGE="$HOMEPAGE" -DCOMPANY_NAME="$COMPANY_NAME" -DOUTFILE="$OUTPUT_FILE" -DINSTDIR_NAME="soulfu_win64" packaging/soulfu.nsi
fi

# Create portable version (zip file)
echo "Creating portable version..."
zip -r "$ZIP_FILE" soulfu.exe datafile.sdf Manual.htm soulfu.jpg SDL2.dll SDL2_net.dll libogg-0.dll libvorbis-0.dll "$ICON_FILE" license.txt

# Create packaging/bin directory if it doesn't exist
mkdir -p packaging/bin

# Move output files to packaging/bin
if [ -z "$ZIP_ONLY" ] && [ -f "$OUTPUT_FILE" ]; then
    mv "$OUTPUT_FILE" packaging/bin/
    echo "Installer moved to packaging/bin/$OUTPUT_FILE"
fi

if [ -f "$ZIP_FILE" ]; then
    mv "$ZIP_FILE" packaging/bin/
    echo "Portable version moved to packaging/bin/$ZIP_FILE"
fi

# Clean up temporary files
echo "Cleaning up temporary files..."
rm -rf "$INSTALLER_DIR"
rm -f soulfu.exe SDL2.dll SDL2_net.dll libogg-0.dll libvorbis-0.dll
# Remove temporary license.txt if we created it
if [ $LICENSE_TEMP_CREATED -eq 1 ] && [ -f "license.txt" ]; then
    rm -f license.txt
fi

echo ""
echo "Build completed successfully!"
if [ -z "$ZIP_ONLY" ]; then
    echo "Installer: packaging/bin/$OUTPUT_FILE"
fi
echo "Portable: packaging/bin/$ZIP_FILE"
