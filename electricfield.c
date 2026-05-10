#include <xgfx/drawing.h>
#include <xgfx/window.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>

typedef float vec2[2];

#define GRID_HEIGHT 48
#define GRID_WIDTH 64
#define SPACING 20
vec2 vectorField[GRID_HEIGHT][GRID_WIDTH];

const float ke = 9e9; // k_e = 9.0 * SPACING^9 Nm^2/C^2

typedef struct {
    float x;
    float y;
    float charge;
} Charge;

#define CHARGES 2
Charge charges[CHARGES] = {
    {
        .x = 150,
        .y = 151,
        .charge = -0.00001
    },
    {
        .x = 500,
        .y = 351,
        .charge = 0.00001
    }
};

int main() {
    initWindow(640, 480, "Electric Field");
    for (int i = 0; i < GRID_HEIGHT; i++) {
        for (int j = 0; j < GRID_WIDTH; j++) {
            vec2 fieldVector = {0,0};
            for (int k = 0; k < CHARGES; k++) {
                vec2 fieldVectorPosition = {j * SPACING, i * SPACING};
                float dx = fieldVectorPosition[0] - charges[k].x;
                float dy = fieldVectorPosition[1] - charges[k].y;
                printf("%f, %f\n", dx, dy);
                float r = sqrtf(dx*dx + dy*dy);
                float fieldVectorMagnitude = (ke * charges[k].charge) / (r*r);
                fieldVector[0] += fieldVectorMagnitude * (dx / r);
                fieldVector[1] += fieldVectorMagnitude * (dy / r);
            }
            printf("%d, %d: %f, %f\n", j, i, fieldVector[0], fieldVector[1]);
            vectorField[i][j][0] = fieldVector[0];
            vectorField[i][j][1] = fieldVector[1];
        }
    }

    while(1) {
        Event event;
        while(checkWindowEvent(&event)) {
            if (event.type == WINDOW_CLOSE) {
                return 0;
            }
        }

        clear();
        for (int i = 0; i < 480; i++) {
            for (int j = 0; j < 640; j++) {
                float potential = 0;
                for (int k = 0; k < CHARGES; k++) {
                    vec2 fieldVectorPosition = {j, i};
                    float dx = fieldVectorPosition[0] - charges[k].x;
                    float dy = fieldVectorPosition[1] - charges[k].y;
                    float r = sqrtf(dx*dx + dy*dy);
                    potential += (ke * charges[k].charge) / r;
                }
                int channel = (fabs(potential) / 1000) * 255;
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
                //printf("%f, %f\n", (j * SPACING) + vectorField[i][j][0], (i * SPACING) + vectorField[i][j][1]);
                float length = sqrtf(vectorField[i][j][0]*vectorField[i][j][0] + vectorField[i][j][1]*vectorField[i][j][1]);
                vec2 normalizedVector = {vectorField[i][j][0]/length*10, vectorField[i][j][1]/length*10};
                line(j * SPACING, i * SPACING, (j * SPACING) + normalizedVector[0], (i * SPACING) + normalizedVector[1], 0xff00ff00);
                circle((j * SPACING) + normalizedVector[0], (i * SPACING) + normalizedVector[1], 2, 0xff00ff00);
            }
        }
        updateWindow();
    }
}