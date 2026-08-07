#!/bin/bash

# Build DMG script for SoundReader macOS installer
# Usage: ./build-dmg.sh <app_path> <output_dmg_path> [app_name]

set -e

APP_PATH="${1:-build-arm64/SoundReader.app}"
OUTPUT_DMG="${2:-SoundReader.dmg}"
APP_NAME="${3:-SoundReader}"

if [ ! -d "$APP_PATH" ]; then
    echo "Error: App path '$APP_PATH' does not exist"
    exit 1
fi

dmg_path="${OUTPUT_DMG%.dmg}.dmg"

echo "Creating DMG for $APP_NAME..."
hdiutil create -volname "$APP_NAME" \
    -srcfolder "$APP_PATH" \
    -ov -format UDZO \
    -fs-args "-c c=64,a=16,e=16" \
    "$dmg_path"

if [ $? -eq 0 ]; then
    echo "DMG created successfully: $dmg_path"
else
    echo "Failed to create DMG"
    exit 1
fi

# Optional: Copy to a releases directory if needed
# mkdir -p releases
# cp "$dmg_path" "releases/"