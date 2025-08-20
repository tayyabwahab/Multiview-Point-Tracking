# Multi-View Point Tracking (OBTracer)

A sophisticated Microsoft Foundation Classes (MFC) C++ application designed for **multi-view point object tracking** in surveillance systems. The project was developed to track objects (humans and vehicles) across multiple viewpoints using a dome camera that covers 4 distinct view points.

## Project Overview

The system is designed with the following key capabilities:
- **Multi-View Coverage**: A dome camera covers 4 view-points simultaneously
- **Object Tracking**: Any object entering from any view is assigned a unique ID
- **Cross-View Tracking**: Objects retain the same ID even when moving between different viewpoints
- **Object Classification**: Automatically distinguishes between humans, vehicles, and groups of people
- **Real-time Processing**: Processes video streams in real-time with background modeling

## Core Algorithms & Technical Implementation

### 1. **Background Subtraction & Foreground Detection**
- **Mixture of Gaussians (MOG)**: Implements adaptive background modeling with up to 3 Gaussian models per pixel
  - Dynamic weight updates based on pixel matching
  - Standard deviation thresholds for foreground segmentation
- **Frame Differencing**: Alternative background modeling approach with adaptive thresholds
  - Initial training phase with high learning rate
  - Gradual adaptation for stable background

### 2. **Blob Detection & Labeling**
- **Connected Component Labeling**: Flood-fill algorithm for blob segmentation
- **Size-based Filtering**: Adaptive filtering based on object height and aspect ratio
- **Object Classification**: Automatic categorization into:
  - `PERSON` : Individual humans
  - `VEHICLE`: Vehicles

### 3. **Multi-Object Tracking**
- **Kalman Filtering**: Predictive tracking with state estimation
- **Particle Filtering**: Robust tracking under occlusion and complex scenarios
- **Color Histogram Matching**: RGB color space analysis for object re-identification
- **Position Prediction**: Maintains object trajectory history (60 frames)
- **Merge/Split Detection**: Handles object occlusion and separation scenarios

### 4. **Cross-View Tracking**
- **Unique ID Assignment**: Each object gets persistent identifier across viewpoints
- **View Transition Handling**: Maintains object identity when moving between camera views
- **Spatial Relationship Mapping**: Tracks object positions relative to view boundaries

### 5. **Image Processing Pipeline**
- **Morphological Operations**: Dilation and erosion for noise reduction
- **Median Filtering**: 3x3 kernel for smoothing
- **Edge Detection**: Sobel edge detection for object boundary analysis
- **Color Analysis**: Dominant color extraction and histogram generation

## Key Features
- Dialog-based MFC application (`COBTracerDlg`) named OBTracer
- Video file (AVI) handling via Windows Video for Windows (VfW)
- Uses Windows multimedia (`mmsystem.h`) and VfW (`vfw.h`) APIs
- Includes modules for image processing, labeling, background subtraction (MOG), frame differencing, and display

## Repository Layout
- **Core Algorithm Modules**:
  - `MOG.*`: Mixture of Gaussians background subtraction
  - `Mframedif.*`: Frame differencing background modeling
  - `Labeling.*`: Connected component labeling and blob detection
  - `Tracker.*`: Multi-object tracking with Kalman/particle filtering
  - `Imgproc.*`: Image processing utilities (morphology, filtering)
- **Video & Camera Modules**:
  - `Avi.*`: AVI file handling and processing
  - `CamCtrl.*`: Camera control and capture
  - `Display.*`: Multi-view display management
- **App Framework**: `OBTracer.*`, `OBTracerDlg.*`, `StdAfx.*`, `resource.h`, `.rc`
- **Build Files**: `.dsp`, `.vcxproj`, `.sln`, `.aps`, `.ncb`, `.opt`, `.plg`, filters
- **Demo Asset**: `Demo/Demo.mp4` - Sample tracking demonstration


### Supported OS
- Windows (tested historically with Visual C++/MFC; modern builds recommended with Visual Studio 2019/2022 on Windows 10/11)

### Toolchain
- Visual Studio with C++ and MFC components installed (e.g., Visual Studio 2019/2022)
- Windows SDK matching your Visual Studio installation

### Libraries/SDKs
- Microsoft Foundation Classes (MFC)
- Windows SDK headers/libraries (`windows.h`, `mmsystem.h`)
- Video for Windows (VfW) headers/libraries (`vfw.h`) — usually included with the Windows SDK

### Linker dependencies
Depending on configuration, you may need to link against:
- `winmm.lib` (for `mmsystem.h` APIs)
- `vfw32.lib` (for VfW APIs)
- Standard MFC/ATL libraries as pulled in by your project configuration

## Build Instructions (Windows)
1. Open `OBTracer.sln` or `OBTracer.vcxproj` in Visual Studio.
2. Ensure the MFC workload is installed: Visual Studio Installer > Modify > Desktop development with C++ > MFC and ATL support.
3. Select the desired configuration (Debug/Release, Win32/x64). If the project is legacy Win32, start with Win32.
4. If needed, add linker inputs under Project Properties > Linker > Input:
   - Additional Dependencies: `winmm.lib; vfw32.lib;` (and any others required by your setup)
5. Build the solution (Build > Build Solution).

## Running
- After a successful build, run the generated executable from Visual Studio or the `Debug`/`Release` output folder.
- The application presents a dialog UI with controls for starting/stopping processing and display modes (1/4/9 views), and supports loading video files.

## Notes
- This project uses precompiled headers (`stdafx.h`/`StdAfx.h`). Ensure PCH settings are enabled in project properties if you upgrade the toolset.
- Some strings and APIs are ANSI-based; you may need to set the character set to Multi-Byte in Project Properties if Unicode issues arise.
- The original `ReadMe.txt` is the default AppWizard summary and is included for historical context.

## Linux/macOS
This code depends on MFC and Windows-specific APIs (VfW, WinMM) and is not portable to Linux or macOS without significant refactoring. 