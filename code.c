#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <math.h>
#define PI 3.1415926535
#define P2 PI/2
#define P3 3*PI/2
#define DR 0.0174533 //One degree in radians

//Player positioning
float px, py, pdx, pdy, pa;

void drawPlayer(){
    glColor3f(1, 1, 0);
    glPointSize(8);
    glBegin(GL_POINTS);
    glVertex2i(px,py);
    glEnd();

    glLineWidth(3);
    glBegin(GL_LINES);
    glVertex2i(px, py);
    glVertex2i(px + pdx*5, py + pdy * 5);
    glEnd();
}


//Map
int mapX=8, mapY = 8, mapS = 64;
int map[] = 
{
    1,1,1,1,1,1,1,1,
    1,0,0,1,0,0,0,1,
    1,0,0,1,0,0,0,1,
    1,0,0,1,1,0,0,1,
    1,0,0,0,0,0,0,1,
    1,0,0,0,0,0,0,1,
    1,0,0,0,0,1,1,1,
    1,1,1,1,1,1,1,1,
};

void drawMap2D(){
    int x,y,xo,yo;

    for(y = 0; y < mapY; y++){
        for(x = 0; x < mapX; x++){
            if(map[y*mapX+x] == 1) { glColor3f(1,1,1); } else { glColor3f(0,0,0); }
            xo = x*mapS; yo=y*mapS;
            glBegin(GL_QUADS);
            glVertex2i(xo   +1, yo    +1);
            glVertex2i(xo   +1, yo + mapS-1);
            glVertex2i(xo + mapS-1, yo + mapS-1);
            glVertex2i(xo + mapS-1, yo    +1);
            glEnd();
        }
    }
}

float dist(float ax, float ay, float bx, float by, float ang)
{
    return ( sqrt((bx-ax) * (bx-ax) + (by-ay) * (by-ay)) );
}

//Draw the rays
void drawRays2D() {
    int r, mx, my, mp, dof;
    float rx, ry, ra, xo, yo;
    float disH, disV, hx, hy, vx, vy, disT;
    float aTan, nTan;

    // Normalize the player angle (pa) between 0 and 2 * PI
    ra = pa - DR*30;
    if(ra < 0) { ra += 2 * PI; }
    if(ra < 0) { ra -= 3 * PI; }
    if (ra < 0) ra += 2 * PI;
    if (ra > 2 * PI) ra -= 2 * PI;

    for (r = 0; r < 60; r++) { // Loop through each ray (adjust for more rays as needed)

        //-- Horizontal Line Check --
        dof = 0;
        disH = 10000000; // Arbitrarily large default value
        hx = px;
        hy = py;
        aTan = -1 / tan(ra);

        if (ra > PI) { // Looking up
            ry = (((int)py >> 6) << 6) - 0.0001;
            rx = (py - ry) * aTan + px;
            yo = -64;
            xo = -yo * aTan;
        } else if (ra < PI) { // Looking down
            ry = (((int)py >> 6) << 6) + 64;
            rx = (py - ry) * aTan + px;
            yo = 64;
            xo = -yo * aTan;
        } else { // Looking directly left or right
            rx = px;
            ry = py;
            dof = 8;
        }

        while (dof < 8) {
            mx = (int)(rx) >> 6;
            my = (int)(ry) >> 6;
            mp = my * mapX + mx;

            if (mp > 0 && mp < mapX * mapY && map[mp] == 1) { // Wall hit
                hx = rx;
                hy = ry;
                disH = dist(px, py, hx, hy, ra);
                dof = 8;
            } else { // No wall, keep looking
                rx += xo;
                ry += yo;
                dof++;
            }
        }

        //-- Vertical Line Check --
        dof = 0;
        disV = 10000000; // Arbitrarily large default value
        vx = px;
        vy = py;
        nTan = -tan(ra);

        if (ra > P2 && ra < P3) { // Looking left
            rx = (((int)px >> 6) << 6) - 0.0001;
            ry = (px - rx) * nTan + py;
            xo = -64;
            yo = -xo * nTan;
        } else if (ra < P2 || ra > P3) { // Looking right
            rx = (((int)px >> 6) << 6) + 64;
            ry = (px - rx) * nTan + py;
            xo = 64;
            yo = -xo * nTan;
        } else { // Looking directly up or down
            rx = px;
            ry = py;
            dof = 8;
        }

        while (dof < 8) {
            mx = (int)(rx) >> 6;
            my = (int)(ry) >> 6;
            mp = my * mapX + mx;

            if (mp > 0 && mp < mapX * mapY && map[mp] == 1) { // Wall hit
                vx = rx;
                vy = ry;
                disV = dist(px, py, vx, vy, ra);
                dof = 8;
            } else { // No wall, keep looking
                rx += xo;
                ry += yo;
                dof++;
            }
        }

        //-- Determine Closest Hit --
        if (disV < disH) {
            rx = vx;
            ry = vy;
            disT = disV;
            glColor3f(0.9, 0, 0);
        } else {
            rx = hx;
            ry = hy;
            disT = disH;
            glColor3f(0.7, 0, 0);
        }

        //-- Draw Ray --
        glLineWidth(99);           // Set line width
        glBegin(GL_LINES);
        glVertex2i(px, py);       // Start at player position
        glVertex2i(rx, ry);       // End at wall hit
        glEnd();

         //--Draw the 3D walls--
        float ca = pa - ra;
        if(ca < 0) { ca += 2*PI; }
        if(ca < 2*PI) { ca -= 2*PI; }
        disT = disT * cos(ca);              //Fix fisheye effect

        float lineH = (mapS * 320) / disT;
        if(lineH > 320) { lineH = 320; }    //line height
        float lineO = 160 - lineH/2;        //Line offset

        glLineWidth(10);
        glBegin(GL_LINES);
        glVertex2i(r * 8 + 530, lineO);
        glVertex2i(r * 8 + 530, lineH + lineO);
        glEnd();

        ra += DR;
        if(ra < 0) { ra += 2 * PI; }
        if(ra < 0) { ra -= 3 * PI; }
        if (ra < 0) ra += 2 * PI;
        if (ra > 2 * PI) ra -= 2 * PI;
    }
}


void display(){

    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    drawMap2D();
    drawPlayer();
    drawRays2D();
    glutSwapBuffers();
}

void buttons(unsigned char key, int x, int y){
    if(key == 'w') {px += pdx; py += pdy;} 
    if(key == 'a') {pa -= 0.1; if(pa < 0){pa += 2*PI;} pdx=cos(pa)*5; pdy=sin(pa)*5;} 
    if(key == 's') {px -= pdx; py -= pdy;}
    if(key == 'd') { pa += 0.1; if (pa > 2 * PI) { pa -= 2 * PI; } pdx = cos(pa) * 5; pdy = sin(pa) * 5; }
    glutPostRedisplay();
}

void init(){
    glClearColor(0.3, 0.3, 0.3,0);
    gluOrtho2D(0, 1024, 512, 0);
    px=300; py=300; pdx = cos(pa) * 5; pdy = sin(pa) * 5;
}

int main(int argc, char* argv[]){
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowSize(1024, 512);
    glutCreateWindow("Raycasting Engine");
    init();
    glutDisplayFunc(display);
    glutKeyboardFunc(buttons);
    glutMainLoop();
}