<div align="center">

```
███████╗██████╗ ██╗  ██╗██╗████████╗
██╔════╝██╔══██╗██║ ██╔╝██║╚══██╔══╝
█████╗  ██████╔╝█████╔╝ ██║   ██║   
██╔══╝  ██╔══██╗██╔═██╗ ██║   ██║   
███████╗██║  ██║██║  ██╗██║   ██║   
╚══════╝╚═╝  ╚═╝╚═╝  ╚═╝╚═╝   ╚═╝  
```

**Real-time AI vision and audio processing — built in C++**

![Status](https://img.shields.io/badge/status-in%20development-amber?style=flat-square)
![Platform](https://img.shields.io/badge/platform-Windows-blue?style=flat-square)
![Language](https://img.shields.io/badge/language-C%2B%2B%20%2F%20CLR-informational?style=flat-square)
![IDE](https://img.shields.io/badge/IDE-Visual%20Studio%202026-purple?style=flat-square)

</div>

---

## Overview

ErKit is a real-time AI-powered desktop application built in C++ using Windows Forms (CLR). It processes live camera input and audio, runs inference on captured frames, and responds using synthesized speech — all within a clean, modern dark interface.

The project is structured to grow modularly: the UI shell is built first, followed by camera integration, AI inference, and audio output.

---

## Current State

| Module | Status |
|---|---|
| UI Shell (Windows Forms) | Complete |
| Camera Feed Panel | In Progress |
| Status Bar | Complete |
| Sidebar Navigation | In Progress |
| Camera Integration (OpenCV) | Planned |
| AI Inference Engine | Planned |
| Speech Output | Planned |

---

## Project Structure

```
ErKit/
├── Main.cpp          — Application entry point
├── MainForm.h        — Main window UI and layout
├── MainForm.cpp      — Form logic
├── MainForm.resx     — Form resources
├── ErKit.vcxproj     — MSVC project file
└── ErKit.slnx        — Solution file
```

---

## Getting Started

### Prerequisites

- Windows 10 / 11
- Visual Studio 2026 (with C++/CLR workload installed)
- .NET Framework 4.7.2+
- Git

### Build & Run

```bash
git clone https://github.com/X3N0L4T/ErKit.git
cd ErKit
```

Open `ErKit.slnx` in Visual Studio, then press **F5** to build and run.

> Make sure **Common Language Runtime Support (/clr)** is enabled under Project Properties → General.

---

## Planned Stack

| Component | Technology |
|---|---|
| UI Framework | Windows Forms (C++/CLR) |
| Camera Input | OpenCV |
| AI Vision | To be decided |
| Speech Output | Windows Speech API (SAPI) |
| Build System | MSVC / Visual Studio |

---

## Roadmap

- [x] Project scaffold and GitHub setup
- [x] Dark UI shell — camera panel, status bar, sidebar
- [ ] Live camera feed via OpenCV
- [ ] Frame capture and preprocessing
- [ ] AI model integration for scene analysis
- [ ] Text-to-speech response pipeline
- [ ] Settings panel and configuration
- [ ] Logging and activity history view

---

## Contributing

This is a personal learning project. Issues and suggestions are welcome — open a GitHub Issue or fork and submit a PR.

---

## License

MIT License — see [`LICENSE`](LICENSE) for details.

---

<div align="center">
<sub>Built from scratch · C++ · Windows · 2026</sub>
</div>
