#ifndef COMMON_H
#define COMMON_H

// Window parameters
#define WIDTH   1280
#define HEIGHT  720
#define FPS     60

// Camera settings
#define CAMERA_INITIAL_ZOOM     1.0f
#define CAMERA_INITIAL_ROTATION 0.0f
#define CAMERA_INITIAL_TARGET   (Vector2){0, 0}
#define CAMERA_OFFSET_CENTER    (Vector2){WIDTH / 2.0f, HEIGHT / 2.0f}

// Translate config
#define DRAG_SENSITIVITY 1.0f

// Zoom config
#define ZOOM_SENSITIVITY 0.2f
#define ZOOM_MIN_LIMIT   0.33f  // 1/3th scale
#define ZOOM_MAX_LIMIT   16.0f  // 16x scale

// Grid config
#define GRID_SPACING        50.0f
#define GRID_COLOR          (Color){235, 219, 178, 51} // Bright white (20% opacity)

// Colors (Gruvbox Dark)
#define COLOR_BRIGHT_BLACK  (Color){146, 131, 116, 255}
#define COLOR_BRIGHT_BLUE   (Color){131, 165, 152, 255}
#define COLOR_BRIGHT_CYAN   (Color){142, 192, 124, 255}
#define COLOR_BRIGHT_GREEN  (Color){184, 187,  38, 255}
#define COLOR_BRIGHT_PURPLE (Color){211, 134, 155, 255}
#define COLOR_BRIGHT_RED    (Color){251,  73,  52, 255}
#define COLOR_BRIGHT_WHITE  (Color){235, 219, 178, 255}
#define COLOR_BRIGHT_YELLOW (Color){250, 189,  47, 255}
#define COLOR_BLACK         (Color){ 40,  40,  40, 255}
#define COLOR_BLUE          (Color){ 69, 133, 136, 255}
#define COLOR_CYAN          (Color){104, 157, 106, 255}
#define COLOR_GREEN         (Color){152, 151,  26, 255}
#define COLOR_PURPLE        (Color){177,  98, 134, 255}
#define COLOR_RED           (Color){204,  36,  29, 255}
#define COLOR_WHITE         (Color){168, 153, 132, 255}
#define COLOR_YELLOW        (Color){215, 153,  33, 255}

#endif