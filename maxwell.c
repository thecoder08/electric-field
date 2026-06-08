#include <string.h>
#include <xgfx/drawing.h>
#include <xgfx/window.h>
#include <math.h>
#include <stdlib.h>

typedef double vec3[3];

#define X 64
#define Y 48
#define Z 20
#define SPACING 20

// vector fields
vec3 eField[X][Y][Z];
vec3 bField[X][Y][Z];
vec3 eFieldNew[X][Y][Z];
vec3 bFieldNew[X][Y][Z];
vec3 jField[X][Y][Z];

double mu0 = 0.01;
double e0 = 0.01;
double deltaT = 0.001;

void add(vec3 a, vec3 b, vec3 v) {
    v[0] = a[0] + b[0];
    v[1] = a[1] + b[1];
    v[2] = a[2] + b[2];
}

void sub(vec3 a, vec3 b, vec3 v) {
    v[0] = a[0] - b[0];
    v[1] = a[1] - b[1];
    v[2] = a[2] - b[2];
}

void mul(vec3 a, double f, vec3 v) {
    v[0] = a[0] * f;
    v[1] = a[1] * f;
    v[2] = a[2] * f;
}

void gradient(vec3 field[X][Y][Z], int x, int y, int z, vec3 grad[3]) {
    // partial derivative w.r.t x
    if (x == 0) {
        sub(field[x+1][y][z], field[x][y][z], grad[0]);
    }
    else if (x == X-1) {
        sub(field[x][y][z], field[x-1][y][z], grad[0]);
    }
    else {
        sub(field[x+1][y][z], field[x-1][y][z], grad[0]);
        mul(grad[0], 0.5, grad[0]);
    }
    // partial derivative w.r.t y
    if (y == 0) {
        sub(field[x][y+1][z], field[x][y][z], grad[1]);
    }
    else if (y == Y-1) {
        sub(field[x][y][z], field[x][y-1][z], grad[1]);
    }
    else {
        sub(field[x][y+1][z], field[x][y-1][z], grad[1]);
        mul(grad[1], 0.5, grad[1]);
    }
    // partial derivative w.r.t z
    if (z == 0) {
        sub(field[x][y][z+1], field[x][y][z], grad[2]);
    }
    else if (z == Z-1) {
        sub(field[x][y][z], field[x][y][z-1], grad[2]);
    }
    else {
        sub(field[x][y][z+1], field[x][y][z-1], grad[2]);
        mul(grad[1], 0.5, grad[1]);
    }
}


int main() {
    srand(1234); // seed
    initWindow(1280, 960, "Maxwell's Equations");

    // initialize vector fields
    for (int x = 0; x < X; x++) {
        for (int y = 0; y < Y; y++) {
            for (int z = 0; z < Z; z++) {
                eField[x][y][z][0] = (double)rand()/RAND_MAX;
                eField[x][y][z][1] = (double)rand()/RAND_MAX;
                eField[x][y][z][2] = (double)rand()/RAND_MAX;
                bField[x][y][z][0] = (double)rand()/RAND_MAX;
                bField[x][y][z][1] = (double)rand()/RAND_MAX;
                bField[x][y][z][2] = (double)rand()/RAND_MAX;
                jField[x][y][z][0] = 0;
                jField[x][y][z][1] = 0;
                jField[x][y][z][2] = 0;
            }
        }
        
    }
    while(1) {
        Event event;
        while(checkWindowEvent(&event)) {
            if (event.type == WINDOW_CLOSE) {
                return 0;
            }
            /*if (event.type == MOUSE_MOVE) {
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
            }*/
        }

        for (int x = 0; x < X; x++) {
            for (int y = 0; y < Y; y++) {
                for (int z = 0; z < Z; z++) {
                    // compute partial derivatives (gradient) of fields
                    vec3 gradE[3];
                    gradient(eField, x, y, z, gradE);
                    vec3 gradB[3];
                    gradient(bField, x, y, z, gradB);
                    // compute curl of fields
                    vec3 curlE;
                    curlE[0] = gradE[1][2] - gradE[2][1];
                    curlE[1] = gradE[2][0] - gradE[0][2];
                    curlE[2] = gradE[0][1] - gradE[1][0];
                    vec3 curlB;
                    curlB[0] = gradB[1][2] - gradB[2][1];
                    curlB[1] = gradB[2][0] - gradB[0][2];
                    curlB[2] = gradB[0][1] - gradB[1][0];
                    // compute time derivatives of fields
                    vec3 dBdt;
                    mul(curlE, -1, dBdt);
                    vec3 curlBmu0;
                    mul(curlB, 1/mu0, curlBmu0);
                    vec3 dEdt;
                    sub(curlBmu0, jField[x][y][z], dEdt);
                    mul(dEdt, 1/e0, dEdt);
                    // compute new state of fields
                    vec3 dE;
                    mul(dEdt, deltaT, dE);
                    add(eField[x][y][z], dE, eFieldNew[x][y][z]);
                    vec3 dB;
                    mul(dBdt, deltaT, dB);
                    add(bField[x][y][z], dB, bFieldNew[x][y][z]);
                }
            }
        }
        // copy new state of fields
        memcpy(eField, eFieldNew, sizeof(eField));
        memcpy(bField, bFieldNew, sizeof(bField));

        clear();
        for (int i = 0; i < Y; i++) {
            for (int j = 0; j < X; j++) {
                // draw electric field vector -> YELLOW arrows
                vec3 fieldVector;
                memcpy(fieldVector, eField[j][i][10], sizeof(fieldVector));

                //printf("%f, %f\n", (j * SPACING) + fieldVector[0], (i * SPACING) + fieldVector[1]);
                float length = sqrtf(fieldVector[0]*fieldVector[0] + fieldVector[1]*fieldVector[1] + fieldVector[2]*fieldVector[2]);
                if (length == 0) {
                    continue;
                }
                vec3 normalizedVector = {fieldVector[0]/length*(SPACING/2), fieldVector[1]/length*(SPACING/2)};
                int channel = (fabsf(length) / 10) * 255;
                if (channel > 255) channel = 255;
                line(j * SPACING, i * SPACING, (j * SPACING) + normalizedVector[0], (i * SPACING) + normalizedVector[1], 0xff000000 | (channel << 8) | (channel << 16));
                circle((j * SPACING) + normalizedVector[0], (i * SPACING) + normalizedVector[1], 2, 0xff000000 | (channel << 8) | (channel << 16));

                // draw magnetic field vector -> BLUE arrows          
                memcpy(fieldVector, bField[j][i][10], sizeof(fieldVector));

                //printf("%f, %f\n", (j * SPACING) + fieldVector[0], (i * SPACING) + fieldVector[1]);
                length = sqrtf(fieldVector[0]*fieldVector[0] + fieldVector[1]*fieldVector[1] + fieldVector[2]*fieldVector[2]);
                if (length == 0) {
                    continue;
                }
                normalizedVector[0] = fieldVector[0]/length*(SPACING/2);
                normalizedVector[1] = fieldVector[1]/length*(SPACING/2);
                channel = (fabsf(length) / 10) * 255;
                if (channel > 255) channel = 255;
                line(j * SPACING, i * SPACING, (j * SPACING) + normalizedVector[0], (i * SPACING) + normalizedVector[1], 0xff000000 | channel);
                circle((j * SPACING) + normalizedVector[0], (i * SPACING) + normalizedVector[1], 2, 0xff000000 | channel);
            }
        }
        updateWindow();
    }
}