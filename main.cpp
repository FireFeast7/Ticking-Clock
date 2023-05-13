//HEADERS
#include <stdio.h>
#include <math.h>
#include <GL/glut.h>
#include <GL/gl.h>
#include <time.h>
#include <sys/timeb.h>
#include <string.h>
#include <Windows.h>
#include <iostream>
#include <gl/freeglut.h>
#pragma comment(lib, "winmm.lib")


/************************************************************************************************************/

//CONSTANTS
#define HOUR_HAND_LENGTH 50.0
#define MINUTE_HAND_LENGTH 64.7106781187
#define SECOND_HAND_LENGTH 78.1024967591
#define CLOCK_RADIUS 80.0
#define PI 3.14159
#define CLOCK_VOL 100.00
#define ANGLE_ONE_MIN PI/30.0

/************************************************************************************************************/

//STRUCTURE DEFINING POINT
typedef struct Point {
    double x , y;
}point;

//ANGLES FOR EACH HAND
double secondAngle = 0 , minuteAngle = 0 , hourAngle = 0;

/************************************************************************************************************/

//FUNCTION TO INITIALIZE STUFF
void init(void) {
	gluOrtho2D(-100 , 100 , -100 , 100); //geometry which is inside the volume of the box is "visible" on the viewport
	glClearColor(0.0 , 0.0 , 0.0 , 0.0);// set all primary colors to 0
	glMatrixMode(GL_PROJECTION);// represents camera lens(aperture,near.far-field)
	glShadeModel (GL_SMOOTH);//smooth shading
	glLoadIdentity();
}

/************************************************************************************************************/

//FUNCTION TO DRAW A POINT
void drawPoint(point p) {
	glBegin(GL_POINTS);
		glVertex2f(p.x , p.y);//draws a point representing the digits on clock
	glEnd();
}


/************************************************************************************************************/

//FUNCTION TO DRAW A LINE
void drawLine(double x , double y , double angle) {// to draw the hands of clock
	glVertex2f(x , x);
	glVertex2f(y*cos(angle) , y*sin(angle) );
}

/************************************************************************************************************/

void addDate() {
    time_t rawtime;
	struct tm * timeinfo;// structure defined in the time.
    time ( &rawtime );//an arithmetic type that represents a date and time
    timeinfo = localtime ( &rawtime );//local time convert the time in seconds since the Epoch pointed to by timer into a broken-down time, expressed as a local time.
    char * s = asctime (timeinfo);//The asctime() function converts the given calendar time of structure tm to a character representation i.e human readable form
	char y[20];
	int count_space = 0 , i;
	for(i=0 ; i<strlen(s) ; ++i) {
	  	if(s[i]==' ') {
	  		count_space++;
	  		y[i] = ',';
	  	}
	  	else{
	  		y[i] = s[i];
	  	}
	  	if(count_space==3)
	  		break;
	}
	y[i] = '\0';
	glColor3f(0 , 1.0 , 0.0);
	glRasterPos2f(-5 , 90);
	for(i=0 ; y[i]!='\0' ; ++i) {
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24 , y[i]);//automatically sets the OpenGL unpack pixel storage modes it needs appropriately and saves and restores the previous modes before returning;
	}

}

/************************************************************************************************************/

//FUNCTION TO DRAW THE BASIC CLOCK STRUCTURE
void drawMarks(void) {
	point sp , ep , p;
	int count = 0;
	double i = PI/6.0;
	sp.x = 0 , sp.y = 0;
	ep.x = 0 , ep.y = CLOCK_RADIUS;
	glPointSize(10.0); // diameter of points
	drawPoint(sp);
	for(i=0 ; i<=2*PI ; i+=PI/30.0) {
		if(i==PI/2.0) {
			p.x = CLOCK_RADIUS;
		}
		else if(i==3*PI/2.0) {
			p.x = -CLOCK_RADIUS;
		}
		else {
			p.x = ep.y*sin(i);
		}
		p.y = ep.y*cos(i);
		if(count%5==0) {
			glPointSize(7.0);// for 3,6,9,12 points in clock
			drawPoint(p);
		}
		else {
			glPointSize(3.0);// for the rest of the points
			drawPoint(p);
		}
		count++;
	}
}

/************************************************************************************************************/

//MAIN DRAW FUNCTION
void drawClock(void) {
	glClear(GL_COLOR_BUFFER_BIT);//starts each frame with clean slate
	glColor3f(1 , 0 , 0); //RGB
	glLineWidth(1.0);
	drawMarks();
	addDate();
	glLineWidth(5.0);
	glColor3f(1.0f, 0.0f, 1.0f);
	glBegin(GL_LINES);
	    drawLine(0.0 , HOUR_HAND_LENGTH , -hourAngle+PI/2);
	glEnd();

	glLineWidth(3.0);

	glColor3f(0.0f, 0.0f, 1.0f);
	glBegin(GL_LINES);
		drawLine(0.0 , MINUTE_HAND_LENGTH , -minuteAngle+PI/2);
	glEnd();

	glLineWidth(1.0);
	glColor3f(0.0f, 1.0f, 1.0f);
	glBegin(GL_LINES);
	    drawLine(0.0 , SECOND_HAND_LENGTH , -secondAngle+PI/2);
	glEnd();
	glFlush();//empties all these buffers, causing all issued commands to be executed as quickly as they are accepted by the actual rendering engine
	glutSwapBuffers();//updates the new buffer with new values of the time along with visual image
}

/************************************************************************************************************/

//FUNCTION TO MAINTAIN ASPECT RATIO WHEN SCREEN IS RESIZED
void reshape(int w , int h) {
	double aspectRatio;
	//to avoid division by 0
	if(h==0) {
		h=1	;
	}
	glViewport(0 , 0 , (GLsizei)w , (GLsizei)h );//specifies the affine transformation of x and y from normalized device coordinates to window coordinates
	glMatrixMode(GL_PROJECTION);// Applies subsequent matrix operations to the projection matrix stack.
	glLoadIdentity();

	aspectRatio = (double)w/(double)h;
	if (w <= h)
    glOrtho (-CLOCK_VOL , CLOCK_VOL , -CLOCK_VOL / aspectRatio , CLOCK_VOL / aspectRatio , 1.0, -1.0);
  else
    glOrtho (-CLOCK_VOL * aspectRatio , CLOCK_VOL * aspectRatio, -CLOCK_VOL, CLOCK_VOL , 1.0, -1.0);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
}

/************************************************************************************************************/

//TIMER FUNCTION
void redraw(int msec) {
	struct timeb tb;
	time_t tim=time(0);
	struct tm* t;
	t=localtime(&tim);
	ftime(&tb);

	secondAngle = (double)(t->tm_sec+ (double)tb.millitm/1000.0)/30.0 * PI;
	minuteAngle = (double)(t->tm_min)/30.0 * PI + secondAngle/60.0;
	hourAngle = (double)(t->tm_hour > 12 ? t->tm_hour-12 : t->tm_hour)/6.0 * PI+ minuteAngle/12.0;

	glutPostRedisplay();//after redraw function is called , for the screen to be not struck on the previous screen , this function is called .
	glutTimerFunc(15, redraw , 1); //call redraw function in every 15 milliseconds
}


/************************************************************************************************************/

//DRIVER FUNCTION
int main(int argc , char ** argv) {
 //   PlaySound(TEXT("ClockTick.wav"), NULL, SND_LOOP | SND_ASYNC);
glutInit(&argc , argv);//A pointer to the program's unmodified argc variable from main
glutInitDisplayMode(GLUT_SINGLE|GLUT_RGB);//sets the initial display mode
	glutInitWindowSize(500 , 500);
	glutInitWindowPosition(100 , 100);
	glutCreateWindow("Analog Clock");
	init();
	glutDisplayFunc(drawClock);
	glutReshapeFunc(reshape);
	glutTimerFunc(15 , redraw , 1);
	glutMainLoop();
	return 0;
}
