#include <xgfx/drawing.h>
#include <xgfx/window.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>

typedef float vec2[2];

#define GRID_HEIGHT 48
#define GRID_WIDTH 64
#define SPACING 20

const float ke = 9e9; // k_e = 9.0 * SPACING^9 Nm^2/C^2

typedef struct {
    float x;
    float y;
    float charge;
} Charge;

int mouseX = 0;
int mouseY = 0;

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

int main() {
    initWindow(640, 480, "Electric Field");
    while(1) {
        Event event;
        while(checkWindowEvent(&event)) {
            if (event.type == WINDOW_CLOSE) {
                return 0;
            }
            if (event.type == MOUSE_MOVE) {
                mouseX = event.mousemove.x;
                mouseY = event.mousemove.y;
            }
            if (event.type == MOUSE_BUTTON) {
                if (event.mousebutton.button == 1 && event.mousebutton.state == 1) {
                    charges[numCharges] = (Charge){
                        .x = mouseX,
                        .y = mouseY,
                        .charge = 0.00001
                    };
                    numCharges++;
                }
                if (event.mousebutton.button == 3 && event.mousebutton.state == 1) {
                    charges[numCharges] = (Charge){
                        .x = mouseX,
                        .y = mouseY,
                        .charge = -0.00001
                    };
                    numCharges++;
                }
            }
        }

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

                int channel = (fabs(potential) / 10000) * 255;
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
                int channel = (fabs(length) / 10) * 255;
                if (channel > 255) channel = 255;
                line(j * SPACING, i * SPACING, (j * SPACING) + normalizedVector[0], (i * SPACING) + normalizedVector[1], 0xff000000 | (channel << 8));
                circle((j * SPACING) + normalizedVector[0], (i * SPACING) + normalizedVector[1], 2, 0xff000000 | (channel << 8));
            }
        }
        updateWindow();
    }
}