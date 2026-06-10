# Graphing Calculator

An interactive 2D graphing calculator built in C++ using [raylib](https://www.raylib.com/). Expressions are parsed and evaluated with [TinyExpr++](https://github.com/Blake-Madden/tinyexpr-plusplus), and the GUI relies on [Dear ImGui](https://github.com/ocornut/imgui) via [rlImGui](https://github.com/raylib-extras/rlImGui).

![Screenshot](https://github.com/Oppenjaimer/graphing-calculator/blob/cpp-rewrite/images/screenshot.png?raw=true)

## Features

* **Infinite adaptive grid** that automatically scales and labels itself.
* **Panning** and **zooming** with the mouse or keyboard shortcuts.
* **Cursor coordinate tracking** for more precise data reading.
* Plotting of **multiple functions** simultaneously with different colors.
* **Interactive legend** for toggling visibility, editing, or removing existing functions, alongside a text input to add new ones.

## Building

The project uses a `Makefile` and requires `g++` to compile, as well as [raylib](https://www.raylib.com/). Make sure to clone the repository with the `--recursive` option to include the submodules inside `external/`.

To generate the executable binary, there are two options.

* **Release mode**: Use `make` to build with maximum optimization.
* **Debug mode**: Use `make DEBUG=1` to build with debug symbols and no optimizations.

This will output `bin/mode/plot`, where `mode` is either `release` or `debug`. Object files are placed in `build/mode`.

## Usage

Run the executable followed by the mathematical expressions to graph as quoted strings. Only use `x` as the independent variable.

```bash
./plot "arctan(x)" "x^3" "1.5"
```

Alternatively, you may run the executable without any arguments and add functions using the GUI instead.

## License

This project is available under the MIT License.
