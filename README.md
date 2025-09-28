# FreeGLUT OpenGL Demo: Fixed Pipeline vs Shader

This demo reproduces your scenario:
- Without shaders, the triangle is blue via vertex-colour tracking (fixed pipeline `glColorMaterial`).
- With shaders, the triangle is teal using a uniform `vColor` and optional fog.

## Build

Requirements: `freeglut`, `glew`, OpenGL development headers, and `g++`.

```bash
make
```

Run:

```bash
./demo
```

## Controls
- S: toggle shader (OFF=blue, ON=teal with fog support)
- F: cycle fog mode (0 none, 1 linear, 2 exp, 3 exp2)
- +/-: adjust fog density
- ESC: quit

## Files
- `main.cpp` — rendering code and shaders
- `Makefile` — build script