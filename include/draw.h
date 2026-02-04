#ifndef DRAW_H
#define DRAW_H

#include <raylib.h>
#include <raymath.h>

#include "environment.h"
#include "parser.h"

/**
 * Draw infinite grid with dynamic spacing between lines.
 */
void draw_grid(Camera2D *camera, float dynamic_spacing);

/**
 * Draw scale labels for grid with dynamic spacing between lines.
 */
void draw_grid_labels(Camera2D *camera, float dynamic_spacing);

/**
 * Evaluate function expression through symbol table and plot the result using the given color.
 */
void plot_function(Camera2D *camera, Node *expression, SymbolTable *symbol_table, Color color);

/**
 * Display cursor coords in world space.
 */
void display_coords(Camera2D *camera, bool over_legend);

#endif