#include <xgfx/drawing.h>
#include <xgfx/window.h>
#include <math.h>

typedef float vec2[2];

#define GRID_HEIGHT 25
#define GRID_WIDTH 33
#define SPACING 20

const float ke = 9e9; // k_e = 9.0 * SPACING^9 Nm^2/C^2

typedef struct {
    float x;
    float y;
    float charge;
} Charge;

int numCharges = 0;
Charge charges[30] = {
    {
        .x = 150,
        .y = 150,
        .charge = -0.00001
    },
    {
        .x = 450,
        .y = 350,
        .charge = 0.00005
    }
};

__attribute__((export_name("draw")))
void draw() {
        clear();
        for (int i = 0; i < 480; i++) {
            for (int j = 0; j < 640; j++) {
                float potential = 0;
                for (int k = 0; k < numCharges; k++) {
                    vec2 fieldVectorPosition = {j, i};
                    float dx = fieldVectorPosition[0] - charges[k].x;
                    float dy = fieldVectorPosition[1] - charges[k].y;
                    float r = sqrtf(dx*dx + dy*dy);
                    potential += (ke * charges[k].charge) / r;
                }

                int channel = (fabsf(potential) / 10000) * 255;
                if (channel > 255) channel = 255;
                if (potential > 0) {
                    plot(j, i, 0xff000000 | (channel << 16));
                }
                else {
                    plot(j, i, 0xff000000 | channel);
                }
            }
        }
        for (int i = 0; i < GRID_HEIGHT; i++) {
            for (int j = 0; j < GRID_WIDTH; j++) {

                vec2 fieldVector = {0,0};
                for (int k = 0; k < numCharges; k++) {
                    vec2 fieldVectorPosition = {j * SPACING, i * SPACING};
                    float dx = fieldVectorPosition[0] - charges[k].x;
                    float dy = fieldVectorPosition[1] - charges[k].y;
                    float r = sqrtf(dx*dx + dy*dy);
                    float fieldVectorMagnitude = (ke * charges[k].charge) / (r*r);
                    fieldVector[0] += fieldVectorMagnitude * (dx / r);
                    fieldVector[1] += fieldVectorMagnitude * (dy / r);
                }

                //printf("%f, %f\n", (j * SPACING) + fieldVector[0], (i * SPACING) + fieldVector[1]);
                float length = sqrtf(fieldVector[0]*fieldVector[0] + fieldVector[1]*fieldVector[1]);
                if (length == 0) {
                    continue;
                }
                vec2 normalizedVector = {fieldVector[0]/length*10, fieldVector[1]/length*10};
                int channel = (fabsf(length) / 10) * 255;
                if (channel > 255) channel = 255;
                line(j * SPACING, i * SPACING, (j * SPACING) + normalizedVector[0], (i * SPACING) + normalizedVector[1], 0xff000000 | (channel << 8));
                circle((j * SPACING) + normalizedVector[0], (i * SPACING) + normalizedVector[1], 2, 0xff000000 | (channel << 8));
            }
        }
        updateWindow();
}

__attribute__((export_name("mouseclick")))
void mouseclick(int mouseX, int mouseY, int button) {
    if (button == 0) {
                    charges[numCharges] = (Charge){
                        .x = mouseX,
                        .y = mouseY,
                        .charge = 0.00001
                    };
                    numCharges++;
                }
    if (button == 2) {
                    charges[numCharges] = (Charge){
                        .x = mouseX,
                        .y = mouseY,
                        .charge = -0.00001
                    };
                    numCharges++;
                }
    draw();
}

void circle(int x, int y, int radius, int color) {
  for (int i = -radius; i < radius; i++) {
    for (int j = -radius; j < radius; j++) {
      if (j*j + i*i < radius*radius) {
        plot(x + j, y + i, color);
      }
    }
  }
}

void line(int x0, int y0, int x1, int y1, int color) {
   int dx = abs(x1 - x0);
   int dy = abs(y1 - y0);
   int sx = (x0 < x1) ? 1 : -1;
   int sy = (y0 < y1) ? 1 : -1;
   int err = dx - dy;
   while(1) {
      plot(x0, y0, color);
      if ((x0 == x1) && (y0 == y1)) break;
      int e2 = 2*err;
      if (e2 > -dy) { err -= dy; x0  += sx; }
      if (e2 < dx) { err += dx; y0  += sy; }
   }
}