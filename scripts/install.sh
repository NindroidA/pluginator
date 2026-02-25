#!/usr/bin/env bash
# Pluginator Installer
#
# Downloads and installs the latest Pluginator binary for Linux/macOS.
#
# Usage:
#   curl -fsSL https://raw.githubusercontent.com/NindroidA/pluginator/main/scripts/install.sh | bash
#
# Options:
#   --version <ver>   Install a specific version (e.g. 2.3.8)
#   --prefix <dir>    Install to a custom directory (default: /usr/local/bin)
#
# Examples:
#   curl -fsSL ... | bash
#   curl -fsSL ... | bash -s -- --version 2.3.8
#   curl -fsSL ... | bash -s -- --prefix ~/.local/bin

set -euo pipefail

REPO="NindroidA/pluginator"
INSTALL_DIR="/usr/local/bin"
VERSION=""
BINARY_NAME="pluginator"

# Parse arguments
while [[ $# -gt 0 ]]; do
  case "$1" in
    --version)
      VERSION="$2"
      shift 2
      ;;
    --prefix)
      INSTALL_DIR="$2"
      shift 2
      ;;
    --help|-h)
      echo "Pluginator Installer"
      echo ""
      echo "Usage: curl -fsSL <url> | bash -s -- [options]"
      echo ""
      echo "Options:"
      echo "  --version <ver>   Install specific version (e.g. 2.3.8)"
      echo "  --prefix <dir>    Install directory (default: /usr/local/bin)"
      echo "  --help            Show this help"
      exit 0
      ;;
    *)
      echo "Unknown option: $1"
      exit 1
      ;;
  esac
done

# Detect OS
OS="$(uname -s)"
case "$OS" in
  Linux)  PLATFORM="linux" ;;
  Darwin) PLATFORM="darwin" ;;
  *)
    echo "Error: Unsupported OS: $OS"
    echo "This installer supports Linux and macOS only."
    echo "For Windows, use: scoop install pluginator"
    exit 1
    ;;
esac

# Detect architecture
ARCH="$(uname -m)"
case "$ARCH" in
  x86_64|amd64) ARCH="x64" ;;
  aarch64|arm64) ARCH="arm64" ;;
  *)
    echo "Error: Unsupported architecture: $ARCH"
    echo "Supported: x86_64/amd64, aarch64/arm64"
    exit 1
    ;;
esac

TARGET="${PLATFORM}-${ARCH}"
echo "Detected platform: ${TARGET}"

# Get latest version from GitHub API if not specified
if [ -z "$VERSION" ]; then
  echo "Fetching latest version..."
  VERSION=$(curl -fsSL "https://api.github.com/repos/${REPO}/releases/latest" \
    -H "Accept: application/vnd.github.v3+json" \
    | grep -o '"tag_name": *"[^"]*"' | head -1 | cut -d'"' -f4 | sed 's/^v//')

  if [ -z "$VERSION" ]; then
    echo "Error: Could not determine latest version."
    echo "Try specifying a version: --version 2.3.8"
    exit 1
  fi
fi

echo "Installing Pluginator v${VERSION} for ${TARGET}..."

# Download the archive
ARCHIVE_NAME="pluginator-${VERSION}-${TARGET}.tar.gz"
DOWNLOAD_URL="https://github.com/${REPO}/releases/download/v${VERSION}/${ARCHIVE_NAME}"

TMPDIR=$(mktemp -d)
trap 'rm -rf "$TMPDIR"' EXIT

echo "Downloading ${DOWNLOAD_URL}..."
if ! curl -fsSL "$DOWNLOAD_URL" -o "${TMPDIR}/${ARCHIVE_NAME}"; then
  echo ""
  echo "Error: Download failed."
  echo "Check that version ${VERSION} exists and has a ${TARGET} build:"
  echo "  https://github.com/${REPO}/releases/tag/v${VERSION}"
  exit 1
fi

# Verify checksum if available
CHECKSUM_URL="https://github.com/${REPO}/releases/download/v${VERSION}/checksums-sha256.txt"
if curl -fsSL "$CHECKSUM_URL" -o "${TMPDIR}/checksums.txt" 2>/dev/null; then
  echo "Verifying checksum..."
  EXPECTED=$(grep "$ARCHIVE_NAME" "${TMPDIR}/checksums.txt" | awk '{print $1}')
  if [ -n "$EXPECTED" ]; then
    if command -v sha256sum >/dev/null 2>&1; then
      ACTUAL=$(sha256sum "${TMPDIR}/${ARCHIVE_NAME}" | awk '{print $1}')
    elif command -v shasum >/dev/null 2>&1; then
      ACTUAL=$(shasum -a 256 "${TMPDIR}/${ARCHIVE_NAME}" | awk '{print $1}')
    else
      echo "Warning: No sha256sum or shasum found, skipping checksum verification"
      ACTUAL=""
    fi

    if [ -n "$ACTUAL" ] && [ "$EXPECTED" != "$ACTUAL" ]; then
      echo "Error: Checksum mismatch!"
      echo "  Expected: ${EXPECTED}"
      echo "  Got:      ${ACTUAL}"
      exit 1
    elif [ -n "$ACTUAL" ]; then
      echo "Checksum verified."
    fi
  fi
fi

# Extract
echo "Extracting..."
tar -xzf "${TMPDIR}/${ARCHIVE_NAME}" -C "${TMPDIR}"

# Find the binary (name includes version and platform)
BINARY_FILE=$(find "${TMPDIR}" -maxdepth 1 -name "pluginator-*" -not -name "*.tar.gz" -not -name "*.txt" | head -1)
if [ -z "$BINARY_FILE" ]; then
  echo "Error: Could not find binary in archive."
  exit 1
fi

# Install
mkdir -p "$INSTALL_DIR"
if [ -w "$INSTALL_DIR" ]; then
  cp "$BINARY_FILE" "${INSTALL_DIR}/${BINARY_NAME}"
  chmod 755 "${INSTALL_DIR}/${BINARY_NAME}"
else
  echo "Need sudo to install to ${INSTALL_DIR}..."
  sudo cp "$BINARY_FILE" "${INSTALL_DIR}/${BINARY_NAME}"
  sudo chmod 755 "${INSTALL_DIR}/${BINARY_NAME}"
fi

echo ""
echo "Pluginator v${VERSION} installed to ${INSTALL_DIR}/${BINARY_NAME}"

# Check if install dir is in PATH
if ! echo "$PATH" | tr ':' '\n' | grep -qx "$INSTALL_DIR"; then
  echo ""
  echo "Warning: ${INSTALL_DIR} is not in your PATH."
  echo "Add it with:"
  echo "  export PATH=\"${INSTALL_DIR}:\$PATH\""
fi

echo ""
echo "Run 'pluginator' to get started."
