#ifndef UPDATE_H
#define UPDATE_H

#include <raylib.h>
#include <raymath.h>

/**
 * Reset camera properties.
 */
void camera_reset(Camera2D *camera);

/**
 * Handle grid panning.
 */
void pan(Camera2D *camera);

/**
 * Handle grid zooming.
 */
void zoom(Camera2D *camera);

/**
 * Handle keyboard shortcuts.
 */
void shortcuts(Camera2D *camera);

#endif