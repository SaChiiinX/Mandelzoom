/*-------------------------------------------------------*/
/*  CS-378                  Computer Graphics              Tom Ellman    */
/*-----------------------------------------------------------------------*/
/*  mandelzoom.cpp   Draw a picture of the Mandelbrot set.               */
/*-----------------------------------------------------------------------*/

#include <cstdlib>
#include <GL/glut.h>
#include <iostream>
#include <cmath>
#include <list>
#include <cfloat>

using namespace std;

// Initial position of upper left corner of window.
#define INITIAL_WIN_X 150
#define INITIAL_WIN_Y 50

// Data structure to represent a region in the complex plane.
struct rectangle
{
	double xmin;
	double ymin;
	double xmax;
	double ymax;

	rectangle(double xmn, double ymn, double xmx, double ymx)
		: xmin(xmn), ymin(ymn), xmax(xmx), ymax(ymx)
	{}
};

// Data structure tosave history of regions viewed.
// Statically allocated and initialized at start up.
list<rectangle*>  rectList;
list<rectangle*>::iterator  rectListIter;

// Variables represeting the view region requested by user.
double xmin, xmax, ymin, ymax;

// Data structure for RGB triples. Could have been a struct.
class rgbType
{
public:
	rgbType() : red(0.0), green(0.0), blue(0.0) {}
	rgbType(double r, double g, double b) : red(r), green(g), blue(b) {}
	double red, green, blue;
};

// Keeping track of the screen window dimensions.
int windowHeight, windowWidth;

// The variable "image" holds the current mandelzoom image.
// Will be dynamically allocated. 
GLfloat* image;

// The variable "table" holds the iteration count of
// each complex number in the selected region.
// Will be dynamically allocated. 
int** table;

// Allocate and initialize tables. 
void initTables(int newW, int newH)
{
	// The image is stored as a 1D dynamic array. 
	image = new GLfloat[3 * newW * newH];
	// The iteration count table is a 2D dynamic array. 
	table = new int* [newW];
	for (int i = 0; i < newW; i++) table[i] = new int[newH];
}

// Delete old tables.
void deleteTables()
{
	// Delete entire 1D array at once. 
	delete image;
	// First delete each column of table. 
	for (int i = 0; i < windowWidth; i++) delete table[i];
	// Then delete the table itself.
	delete table;
}

// Flag to control recomputation of pixmap.
bool recompute = true;

// Keeping track of the start and end points of the rubber band.
int xAnchor, yAnchor, xStretch, yStretch;

// Variables for use in rubberbanding.
bool rubberBanding = false, bandOn = false;

// Intializes the colors depending on how close a pixel is to the madelbrot set
void initColors(rgbType *colors){
	int i;
	for(i = 0; i < 1000; i++){
		if (i < 300){
			colors[i] = rgbType(255.0,255.0,255.0);
		}else if (i < 500){
			colors[i] = rgbType(0.0,255.0,0.0);
		}else if (i < 700){
			colors[i] = rgbType(0.0,0.0,255.0);
		}else{
			colors[i] = rgbType(255.0,0.0,0.0);
		}
	}
	colors[i] = rgbType();
}

// Determines whether the pixel is in the madelbrot set and how close it is
int convergenceIndex(double sX, double sY) {
	int i;
	double x = 0.0;
	double y = 0.0;
	double tempx;
	for (i = 1; i <= 1000; i++) {
		tempx = x * x - y * y + sX;
		y = 2 * x * y + sY;
		x = tempx;
		if ((x * x + y * y) > 4) {
			break;
		}
	}
	return i;
}

void updateImage(rgbType* colors) {
	int colorIndex;
	for (int i = 0; i < windowWidth; i++) {
		for (int j = 0; j < windowHeight; j++) {
			colorIndex = table[i][j];

			image[3*(i * windowWidth + j)] = colors[colorIndex].red;
			image[3*(i * windowWidth + j + 1)] = colors[colorIndex].green;
			image[3*(i * windowWidth + j + 2)] = colors[colorIndex].blue;
		}
	}
}

void restoreImage()

// Call back function that draws the image.  
void drawFractal() {
	if (!recompute) {
		return;
	}

	glClear(GL_COLOR_BUFFER_BIT);

	// intializes memory for array of rgb types
	rgbType* colors = malloc(1001 * rgbType);
	// intializes all possible colors in the window
	initColors(colors);

	double sX;
	double sY;

	initTables(windowWidth, windowHeight);

	for (int u = 0; u < windowWidth; u++) {
		for (int v = 0; v < windowHeight; v++) {
			sX = (xmin + u * ((xmax - xmin) / (windowWidth - 1)));
			sY = (ymin + v * ((ymax - ymin) / (windowHeight - 1)));
			table[u][v] = convergenceIndex(sX, sY);
		}
	}

	updateImage(colors);
	glFlush();
	recompute = false;
}

void reshape(int w, int h)
// Callback for processing reshape events.
{
	if (w > 0 && h > 0 && (w != windowWidth || h != windowHeight))
	{
		double xMid = (xmin + xmax)/2.0;
		double yMid = (ymin + ymax)/2.0;

		double xPix = w / 2;
		double yPix = h / 2;

		double pixPerX = windowWidth/(xmax-xmin);
		double pixPerY = windowHeight/(ymax-ymin);
		deleteTables();
		initTables(w, h);
		windowWidth = w;
		windowHeight = h;

		xmin = (xMid - xPix)/pixPerX;
		xmax = (xMid + xPix)/pixPerX;
		ymin = (yMid - yPix)/pixPerY;
		ymax = (yMid + yPix)/pixPerY;
		recompute = true;
	}
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0.0, (GLdouble)w, 0.0, (GLdouble)h);
}


// Get the current image from OpenGL and save it in our image variable. 
void saveImage()
{
	glReadPixels(0, 0, windowWidth, windowHeight, GL_RGB, GL_FLOAT, image);
}

// Get the current image from our image variable and send it to OpenGL.
void restoreImage()
{
	glRasterPos2i(0, 0);
	glDrawPixels(windowWidth, windowHeight, GL_RGB, GL_FLOAT, image);
}

// Draw a rectangle defined by four integer parameters.
void drawRectangle(int xA, int yA, int xS, int yS)
{
	glBegin(GL_LINE_LOOP);
	glVertex2i(xA, yA);
	glVertex2i(xS, yA);
	glVertex2i(xS, yS);
	glVertex2i(xA, yS);
	glEnd();
}

void drawRubberBand(int xA, int yA, int xS, int yS)
// Draw the rubber band in XOR mode.
{
	glEnable(GL_COLOR_LOGIC_OP);
	glLogicOp(GL_XOR);
	drawRectangle(xA, yA, xS, yS);
	glDisable(GL_COLOR_LOGIC_OP);
	glFlush();
}

void rubberBand(int x, int y)
// Callback for processing mouse motion.
{
	if (rubberBanding)
	{
		y = windowHeight - y;
		glColor3f(1.0, 1.0, 1.0);
		// If the band is on the screen, remove it. 
		if (bandOn) drawRubberBand(xAnchor, yAnchor, xStretch, yStretch);
		xStretch = x;
		yStretch = y;
		// Draw the rubber band at its new position.
		drawRubberBand(xAnchor, yAnchor, xStretch, yStretch);
		bandOn = true;
	}
}

void escExit(GLubyte key, int, int)
// Callback for processing keyboard events.
{
	if (key == 27 /* ESC */) std::exit(0);
}

void mouse(int button, int state, int x, int y)
// Routine for processing mouse events.
{
	if (button == GLUT_MIDDLE_BUTTON) return;
	y = windowHeight - y;
	switch (state)
	{
	case GLUT_DOWN:
	{
		if (!rubberBanding)
		{
			xAnchor = x;
			yAnchor = y;
			xStretch = x;
			yStretch = y;
			drawRubberBand(xAnchor, yAnchor, xStretch, yStretch);
			bandOn = true;
			rubberBanding = true;
			break;
		}
	}
	case GLUT_UP:
	{
		if (rubberBanding)
		{
			// Remove the rubber band currently on the screen. 
			drawRubberBand(xAnchor, yAnchor, xStretch, yStretch);
			glClear(GL_COLOR_BUFFER_BIT);
			double maxX = max(xAnchor, xStretch);
			double minX = min(xAnchor, xStretch);
			double maxY = max(yAnchor, yStretch);
			double minY = min(yAnchor, yStretch);
			double xd = maxX - minX;
			double yd = maxY - minY;
			double aR = yd / xd;
			double aW = windowHeight/windowWidth;
			double buffer;

			if (aR > aW) {
				ymin = yAnchor/windowHeight;
				ymax = yStretch/windowHeight;
				// Offset for x to match aspect ratio
				buffer = (yd/aW) - xd;
				if(xStretch < xAnchor){
					xmin = (xStretch - buffer)/windowWidth;
					xmax = xAnchor/windowWidth;
				}else{
					xmax = (xStretch + buffer)/windowWidth;
					xmin = xAnchor/windowWidth;
				}
			}
			else {
				xmin = xAnchor/windowWidth;
				xmax = xAnchor/windowWidth;
				// Offset for y to match aspect ratio
				buffer = (aW*xd) - yd;

				if(yStretch < yAnchor){
					ymin = (yStretch - buffer)/windowHeight;
					ymax = yAnchor/windowHeight;
				}else{
					ymax = (yStretch + buffer)/windowHeight;
					ymin = yStretch/windowHeight;
				}
			}
			cout << "region" << xmin << ", " << xmax << ", " << ymin ", " << ymax << "\n";
			recompute = true;
			bandOn = false;
			rubberBanding = false;
			glutPostRedisplay();
			break;
		}
	}
	}
}


void mainMenu(int item)
// Callback for processing main menu.
{
	switch (item)
	{
	case 1: // Push
		break;
	case 2: // Pop
		break;
	case 3: std::exit(0);
	}
}

void setMenu()
// Routine for creating menus.
{
	glutCreateMenu(mainMenu);
	glutAddMenuEntry("Push", 1);
	glutAddMenuEntry("Pop", 2);
	glutAddMenuEntry("Exit", 3);
	glutAttachMenu(GLUT_MIDDLE_BUTTON);
}

int main(int argc, char* argv[])
{
	// Process command line parameters and convert
	// them from string to int or float.
	/*xmin = atof(argv[1]);
	xmax = atof(argv[2]);
	ymin = atof(argv[3]);
	ymax = atof(argv[4]);

	windowWidth = atoi(argv[5]);
	windowHeight = atoi(argv[6]);
	*/

	xmin = -2.0;
	xmax = 0.5;
	ymin = -1.25;
	ymax = 1.25;

	windowWidth = 400;
	windowHeight = 400;

	// Initialize the dynamically allocated tables after
	// main function has begun. 
	initTables(windowWidth, windowHeight);

	// Push initial view rectangle onto rectList. Set rectListIter
	// to reference the first and only rectangle on the list. 
	rectList.push_front(new rectangle(xmin, ymin, xmax, ymax));
	rectListIter = rectList.begin();


	// Mask floating point exceptions.
	_control87(MCW_EM, MCW_EM);

	// Initialize glut with command line parameters.
	glutInit(&argc, argv);

	// Choose RGB display mode for normal screen window.
	glutInitDisplayMode(GLUT_RGB);

	// Set initial window size, position, and title.
	glutInitWindowSize(windowWidth, windowHeight);
	glutInitWindowPosition(INITIAL_WIN_X, INITIAL_WIN_Y);
	glutCreateWindow("Mandelzoom");

	// You don't (yet) want to know what this does.
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0.0, (double)windowWidth, 0.0, (double)windowHeight),
		glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0.375, 0.375, 0.0);

	// Set the color for clearing the normal screen window.
	glClearColor(0.0, 0.0, 0.0, 0.0);

	// Set the callbacks for the normal screen window.
	glutDisplayFunc(drawFractal);
	glutMouseFunc(mouse);
	glutMotionFunc(rubberBand);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(escExit);

	setMenu();

	glutMainLoop();

	return 0;
}