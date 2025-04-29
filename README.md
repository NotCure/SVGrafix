
# SVGrafix

A lightweight C++ SVG parser and renderer built on OpenGL.
It reads an SVG file, constructs a simple DOM, extracts element attributes, and renders basic shapes (e.g. <rect>, <circle>) to the screen.

## 📝 Note

This repository currently uses **OpenGL** as a proof of concept.  
A full Vulkan port is planned once the core parsing and rendering pipeline is stable.


## 🧾 Requirments
- C++ Standard: C++17 or later

- **Libraries:**

    - GLFW (window/context/input)

    - GLAD (OpenGL loader)

    - OpenGL 3.3 Core Profile

- CMake 3.15+ 
