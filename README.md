# Graphing Calculator

An interactive 2D graphing calculator built in C using raylib. Expressions are parsed and evaluated using my own [parsing library](https://github.com/Oppenjaimer/math-parser).

## Features

* **Infinite adaptive grid** that automatically scales and labels itself.
* **Panning** and **zooming** with the mouse or keyboard shortcuts.
* **Cursor coordinate tracking** for more precise data reading.
* Plot **multiple functions** simultaneously with different colors.

## Building

The project uses a `Makefile` and requires `gcc` to compile, as well as [raylib](https://www.raylib.com/) and its dependencies. Additionally, it needs the path to the directory where the [parsing library](https://github.com/Oppenjaimer/math-parser) was compiled (see `PARSER_DIR` in the `Makefile`).

To generate the executable binary, there are two options.

* **Release mode**: Use **make** to build with maximum optimization.
* **Debug mode**: Use **make DEBUG=1** to build with debug symbols and no optimizations.

This will output `bin/mode/plot`, where `mode` is either `release` or `debug`. Object files are placed in `build/mode`.

## Usage

Run the executable followed by the mathematical expressions to graph as quoted strings. Only use `x` as the independent variable.

```bash
./plot "arctan(x)" "x^3" "1.5"
```

## License

This project is available under the MIT License.