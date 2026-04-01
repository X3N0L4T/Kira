<div align="center">

```
██╗  ██╗██╗██████╗  █████╗ 
██║ ██╔╝██║██╔══██╗██╔══██╗
█████╔╝ ██║██████╔╝███████║
██╔═██╗ ██║██╔══██╗██╔══██║
██║  ██╗██║██║  ██║██║  ██║
╚═╝  ╚═╝╚═╝╚═╝  ╚═╝╚═╝  ╚═╝
```

**An offline AI with real-time vision — built in C++ for future embodiment**

![Status](https://img.shields.io/badge/status-in%20development-amber?style=flat-square)
![Platform](https://img.shields.io/badge/platform-Windows-blue?style=flat-square)
![Language](https://img.shields.io/badge/language-C%2B%2B%20%2F%20CLR-informational?style=flat-square)
![IDE](https://img.shields.io/badge/IDE-Visual%20Studio%202026-purple?style=flat-square)
![AI](https://img.shields.io/badge/AI-YOLOv4%20%2F%20Offline-green?style=flat-square)

</div>

---

## Overview

Kira is an offline AI desktop application built in C++ using Windows Forms (CLR). She processes live camera input, detects and identifies objects in real time using YOLOv4, generates natural language descriptions of what she sees, and will eventually respond using synthesized speech.

The project is designed with future hardware embodiment in mind — the vision and reasoning systems being built here are intended to transfer into a physical robotic platform.

---

## Current State

| Module | Status |
|---|---|
| UI Shell (Windows Forms) | ✅ Complete |
| Sidebar Navigation | ✅ Complete |
| Activity Log | ✅ Complete |
| Settings Panel | ✅ Complete |
| Camera Feed (OpenCV) | ✅ Complete |
| AI Object Detection (YOLOv4) | 🔧 In Progress |
| Scene Description / Reasoning | ⏳ Planned |
| Speech Output (SAPI) | ⏳ Planned |
| Robotic Embodiment Interface | ⏳ Future |

---

## Project Structure

```
Kira/
├── Main.cpp            — Application entry point
├── MainForm.h          — Main window, UI layout, camera + AI logic
├── MainForm.cpp        — Form implementation
├── MainForm.resx       — Form resources
├── Kira.vcxproj        — MSVC project file
└── Kira.slnx           — Solution file
```

---

## Getting Started

### Prerequisites

- Windows 10 / 11
- Visual Studio 2026 (with C++/CLR workload)
- .NET Framework 4.7.2+
- vcpkg with `opencv4:x64-windows`
- YOLOv4 model files (see below)

### Clone & Build

```bash
git clone https://github.com/X3N0L4T/Kira.git
cd Kira
```

Open `Kira.slnx` in Visual Studio and press **F5**.

### YOLOv4 Model Files

Download and place in `C:\Kira\models\`:

| File | Source |
|---|---|
| `yolov4.weights` | [Download (~250MB)](https://github.com/AlexeyAB/darknet/releases/download/darknet_yolo_v3_optimal/yolov4.weights) |
| `yolov4.cfg` | [Download](https://raw.githubusercontent.com/AlexeyAB/darknet/master/cfg/yolov4.cfg) |
| `coco.names` | [Download](https://raw.githubusercontent.com/AlexeyAB/darknet/master/data/coco.names) |

> Model files are not included in the repo due to size.

---

## Tech Stack

| Component | Technology |
|---|---|
| UI Framework | Windows Forms (C++/CLR) |
| Camera Input | OpenCV 4.12.0 |
| Object Detection | YOLOv4 via OpenCV DNN |
| Speech Output | Windows Speech API (SAPI) — planned |
| Build System | MSVC / Visual Studio 2026 |
| Package Manager | vcpkg |

---

## Roadmap

- [x] Project scaffold and GitHub setup
- [x] Dark UI shell — camera panel, status bar, sidebar
- [x] Live camera feed via OpenCV
- [x] Sidebar navigation — logs, settings, about
- [x] Async camera device detection
- [x] YOLOv4 object detection on live feed
- [x] Natural language scene description
- [x] Overlay detection results on camera feed
- [ ] Text-to-speech response pipeline
- [ ] Persistent memory / context awareness
- [ ] Robotic embodiment interface layer

---

## Contributing

This is a personal learning project. Issues and suggestions are welcome — open a GitHub Issue or fork and submit a PR.

---

## License

MIT License — see [`LICENSE`](LICENSE) for details.

---

<div align="center">
<sub>Kira · Built from scratch · C++ · Windows · 2026</sub>
</div>