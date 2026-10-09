# S-Shot Wayland (v1.32)

[![CI Build & Test](https://github.com/uri-travoski/S-Shot-Wayland/actions/workflows/ci.yml/badge.svg)](https://github.com/uri-travoski/S-Shot-Wayland/actions)
[![Release](https://img.shields.io/github/v/release/uri-travoski/S-Shot-Wayland)](https://github.com/uri-travoski/S-Shot-Wayland/releases)
[![License: GPL v3](https://img.shields.io/badge/License-GPLv3-blue.svg)](https://www.gnu.org/licenses/gpl-3.0)

**S-Shot Wayland** is a native, ultra-lightweight Linux Wayland screenshot capture and annotation editor with a modern **ksnip-inspired** interface. Built from the ground up for modern Wayland compositors (GNOME Shell / Mutter, KDE Plasma / KWin, Sway, Hyprland, Wayfire, COSMIC, etc.) with **zero X11/Xlib/Xfixes/Xtst legacy dependencies**.

Designed for minimal system resource footprint (~18–28 MB idle memory) with instantaneous startup, Wayland Desktop Portal protocol compliance, and a full-featured annotation suite.

---

## 🌟 Key Features

### 🚀 Pure Wayland Architecture
- **Zero X11 Legacy**: Built strictly with Qt6 native Wayland backends (`qtwayland`) and D-Bus interfaces.
- **XDG Desktop Portal Integration**: Seamless fullscreen, active window, and interactive region capture via `org.freedesktop.portal.Screenshot`.
- **Portal Screen Color Picker**: Pick colors from anywhere across multi-monitor Wayland displays using `org.freedesktop.portal.Screenshot.PickColor` with real-time RGB/HEX formatting.
- **Global Shortcuts Portal**: Native hotkey registration via `org.freedesktop.portal.GlobalShortcuts`.
- **Low Memory Footprint**: Lazy window instantiation maintains minimal idle RAM (~18–28 MB) when running in the background/system tray.

### 🎨 Complete Annotation & Editor Suite
- **Multi-Tab Workspace**: Edit multiple screenshots and images in concurrent tabs with individual undo/redo stacks.
- **Interactive Directional Canvas Resizing**:
  - 8 perimeter and corner resize handles.
  - **Inward Cropping**: Drag inward from any edge (left, right, top, bottom, corners) to trim/crop canvas directly from that side while preserving content alignment.
  - **Outward Expansion**: Drag outward from any edge to expand the canvas with a clean white fill (`#FFFFFF`).
- **Area Selection & Manipulation**:
  - Select any rectangular area of a screenshot.
  - Floating action menu: Copy (📋), Cut (✂), Delete (🗑), and Crop (⛶).
  - Deleting/cutting an area fills the erased region with clean white (`#FFFFFF`) rather than transparency.
- **Direct Clipboard Image Pasting**:
  - Paste images directly onto an active canvas as a movable, resizable layer (`PixmapItem`) with full undo/redo.
  - Option to paste as a new image/tab.
- **Borderless & Transparent Text Annotations**:
  - Clean inline text editing without distracting border lines or opaque backgrounds.
  - Transparent text boxes with customizable fonts, weights, sizes, and colors.
  - Standard text editing with normal Backspace / Delete behavior without deleting the textbox element.
- **Comprehensive Drawing Tools**:
  - **Pen**: Smooth antialiased freehand drawing.
  - **Highlighter**: Realistic marker blending preserving underlying image text and contrast.
  - **Arrows & Double Arrows**: Crisp directional indicators with customizable arrowheads.
  - **Shapes**: Outlined and filled rectangles, rounded rectangles, and ellipses.
  - **Number / Stepper Badges**: Sequential circular step markers (`1`, `2`, `3`...) with step increment/decrement (`+` / `-`) and one-click reset.
  - **Blur / Pixelate**: Redaction tool with adjustable block sizes (levels 1–5) and memory-optimized rendering.
  - **Flood Fill Bucket**: Color fill tool with customizable color tolerance.
  - **Pan / Hand Tool**: Smooth panning across large screenshots alongside mousewheel zoom.
- **Clean Export & File Formats**: High-quality export to PNG, JPEG, BMP, and WebP. Prompting close dialog with Save / Discard / Cancel.
- **Single Canonical Desktop Entry**: Single `io.github.uri_travoski.s-shot-wayland.desktop` entry registered with AppStream compliance, eliminating duplicate entries in "Open With" context menus.

---

## 📦 Installation & Packaging

### Debian / Ubuntu / Linux Mint (`.deb`)
```bash
sudo dpkg -i s-shot-wayland_1.32.0_amd64.deb
# or
sudo apt install ./s-shot-wayland_1.32.0_amd64.deb
```

### Fedora / RHEL / openSUSE (`.rpm`)
```bash
sudo rpm -i s-shot-wayland-1.32.0-1.x86_64.rpm
# or on Fedora
sudo dnf install ./s-shot-wayland-1.32.0-1.x86_64.rpm
```

### Flatpak
Flatpak manifest is provided under `packaging/flatpak/io.github.uri_travoski.s-shot-wayland.yml`.

---

## 🛠️ Building from Source

### Dependencies
- C++20 compliant compiler (`g++` >= 11 or `clang++` >= 14)
- CMake >= 3.16
- Qt 6 (6.2+) with modules:
  - `qt6-base-dev` (Core, Gui, Widgets, Network, DBus, Test)
  - `qt6-svg-dev`
  - `qt6-wayland-dev` / `libqt6waylandclient6`
- D-Bus session bus with `xdg-desktop-portal`

#### Install dependencies on Debian/Ubuntu:
```bash
sudo apt update
sudo apt install -y build-essential cmake qt6-base-dev qt6-svg-dev qt6-wayland xdg-desktop-portal
```

#### Install dependencies on Fedora:
```bash
sudo dnf install -y gcc-c++ cmake qt6-qtbase-devel qt6-qtsvg-devel qt6-qtwayland-devel xdg-desktop-portal
```

### Build Instructions
```bash
git clone https://github.com/uri-travoski/S-Shot-Wayland.git
cd S-Shot-Wayland

# Configure and compile
cmake -B build -S . -DCMAKE_BUILD_TYPE=Release
cmake --build build -j$(nproc)

# Run test suite
./build/test_wayland_editor

# Run application
./build/s-shot-wayland
```

### Building Debian or RPM Packages
```bash
cd build
cpack -G DEB
cpack -G RPM
```

---

## ⌨️ Command Line Usage

```bash
s-shot-wayland [options] [file...]

Options:
  -h, --help        Show help options
  -v, --version     Show version information (1.32)
  -r, --region      Capture a region of the screen
  -f, --fullscreen  Capture the full display
  -c, --colorpicker Pick a color from screen
  -e, --editor      Launch the annotation editor
```

---

## 📄 License

GPLv3. See [LICENSE](LICENSE) for details.
