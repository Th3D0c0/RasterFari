# RasterFari


# Building RasterFari

This project uses CMake to manage the build process. Windows and Linux compilers handle build configurations (Debug vs. Release) differently, so the commands vary slightly depending on your operating system. 

## Prerequisites

**Windows:**
* **CMake:** Installed and added to your system PATH.
* **Compiler:** Visual Studio Build Tools (ensure the "Desktop development with C++" workload is selected during installation to provide the MSVC compiler).

**Linux (Tested on Arch-based System):**
* **Build Tools:** `sudo pacman -S cmake base-devel`
* **Required Libraries:** X11 and OpenGL development packages.

---

## Linux Build Instructions

**1. Configure the project (Release mode):**
```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release && cmake --build build && cd build && ./RasterFari

**2. Configure the project (Debug mode):**
```bash
cmake -B build -DCMAKE_BUILD_TYPE=Debug && cmake --build build && cd build && ./RasterFari

---

## Windows Build Instructions

**1. Configure the project (Release mode):**
```bash
cmake -B build 
cmake --build build --config Release
cd build
./RasterFari

**2. Configure the project (Debug mode):**
```bash
cmake -B build 
cmake --build build --config Debug
cd build
./RasterFari
