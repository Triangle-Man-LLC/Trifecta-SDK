# Drivers for Trifecta IMU Devices #

This repository contains driver software for the <b>Intelligent Sensor Fusion</b> IMU/navigation device series. 

![CI](https://github.com/Triangle-Man-LLC/Trifecta-Driver/actions/workflows/static_analyzer.yml/badge.svg)
![CI](https://github.com/Triangle-Man-LLC/Trifecta-Driver/actions/workflows/execute_tests.yml/badge.svg)

![CI](https://github.com/Triangle-Man-LLC/Trifecta-Driver/actions/workflows/build_linux.yml/badge.svg)
![CI](https://github.com/Triangle-Man-LLC/Trifecta-Driver/actions/workflows/build_windows.yml/badge.svg)
![CI](https://github.com/Triangle-Man-LLC/Trifecta-Driver/actions/workflows/build_windows_msvc.yml/badge.svg)
![CI](https://github.com/Triangle-Man-LLC/Trifecta-Driver/actions/workflows/build_esp-idf.yml/badge.svg)
![CI](https://github.com/Triangle-Man-LLC/Trifecta-Driver/actions/workflows/build_android.yml/badge.svg)

![Docs](https://github.com/Triangle-Man-LLC/Trifecta-Driver/actions/workflows/build_docs.yml/badge.svg)

### Installation and Usage ###
Currently supported platforms/languages are <b>ESP-IDF (C/C++)</b>, <b>Linux (C/C++)</b>, <b>Windows (C/C++)</b>. (Platforms/languages with in-progress support: <b>STM32 (C/C++)</b>, <b>Android (NDK)</b>, <b>Python</b>, <b>Micropython</b>, <b>C#</b>.) 

### Porting the Drivers to Another Platform ###

The drivers can be ported to platforms by implementing the relevant methods listed in the <a href = "/include/FS_Trifecta_Interfaces.h">Interfaces Definitions</a> file. 

Any method which is not relevant/supported (e.g. wireless networking methods on STM32 which does not have wireless networking support) can be made to return -1 to indicate the lack of support.

The specific implementation of the `FS_Interfaces.h` file should be placed in the `/Trifecta-Driver/<PLATFORM_NAME>/FS_Interfaces.c` file of the project.

### Documentation ###

The user-facing API documentation lives in <a href = "/documentation">`documentation/</a> (Sphinx + RST) and can be built locally with:

```
python -m pip install -r documentation/requirements.txt
sphinx-build -M html documentation documentation/_build
```

The resulting website is in `documentation/_build/html` (or run `make -C documentation html`). The API reference, packet format, data types, replay/saving guides, and a platform-porting guide are all included.
