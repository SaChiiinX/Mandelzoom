/*--------------------------------------------------------*/
/*  CS-378           Computer Graphics         Tom Ellman */
/*--------------------------------------------------------*/
/*  Rubber Bands XOR Demo                                 */
/*--------------------------------------------------------*/

#include <cstdlib>
#include <GL/glut.h>
#include <cmath>
#include <cfloat>
#include <iostream>

using namespace std;

// Defining default values for window size, shape and location.
#define INITIAL_WIN_W 800
#define INITIAL_WIN_H 800
#define INITIAL_WIN_X 150
#define INITIAL_WIN_Y 50


// Variable for use in rubberbanding.
int xAnchor, yAnchor, xStretch, yStretch;
bool rubberBanding = false;
bool toClear = true; 

// Variables for keeping track of the screen window dimensions.
int windowHeight, windowWidth;

void clearPicture()
{
    if (toClear) {
        glClear(GL_COLOR_BUFFER_BIT);
        glBegin(GL_LINES);
        glEnd();
        glFlush();
        toClear = false;
    }
}

void drawLine(int xOld, int yOld, int xNew, int yNew )
// Draw a line from (xOld,yOld) to (xNew,yNew).
{
  glBegin( GL_LINES );
  glVertex2i( xOld, yOld );
  glVertex2i( xNew, yNew );
  glEnd();
  glFlush();
}



void drawRubberBand(int xA, int yA, int xS, int yS)
{
    glEnable(GL_COLOR_LOGIC_OP);
    glLogicOp(GL_XOR);
    glBegin( GL_LINES );
    glVertex2i( xA, yA );
    glVertex2i( xS, yS );
    glEnd();
    glDisable(GL_COLOR_LOGIC_OP);
    glFlush();
}

void rubberBand( int x, int y )
// Callback for processing mouse motion.
{
  if ( rubberBanding )
  {
    drawRubberBand(xAnchor,yAnchor,xStretch,yStretch);
    y = windowHeight - y;
    xStretch = x;
    yStretch = y;
    drawRubberBand(xAnchor,yAnchor,xStretch,yStretch);
    glFlush();
  }
}

  void reshape( int w, int h )
  // Callback for processing reshape events.
  {
    windowWidth = w;
    windowHeight = h;
    glViewport( 0, 0, ( GLsizei )w, ( GLsizei )h );
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    gluOrtho2D( 0.0, ( GLdouble )w, 0.0, ( GLdouble )h );
  }


  void escExit( GLubyte key, int, int )
  // Callback for processing keyboard events.
  {
    if ( key == 27 /* ESC */ ) std::exit( 0 );
  }


  void mainMenu( int item )
  // Callback for processing main menu.
  {
    switch ( item )
    {
    case 1: toClear = true; glutPostRedisplay();  break;
      case 2: std::exit( 0 );
    }
  }



  void setMenus()
  // Function for creating menus.
  {


    glutCreateMenu( mainMenu );
    glutAddMenuEntry( "Clear", 1 );
    glutAddMenuEntry( "Exit", 2 );
    glutAttachMenu( GLUT_MIDDLE_BUTTON );
  }


  void processLeftDown( int x, int y )
  // Function for processing mouse left botton down events.
  {
    if (!rubberBanding)
    {
      int xNew = x;
      int yNew = windowHeight - y;
      xAnchor = xNew;
      yAnchor = yNew;
      xStretch = xNew;
      yStretch = yNew;
	  rubberBanding = true;
      drawRubberBand(xAnchor,yAnchor,xStretch,yStretch);
    }
  }



  void processLeftUp( int x, int y )
  // Function for processing mouse left botton up events.
  {
    if (rubberBanding)
    {
      int xNew, yNew;
      drawRubberBand(xAnchor,yAnchor,xStretch,yStretch);
      rubberBanding = false;
      xNew = x;
      yNew = windowHeight - y;
      drawLine(xAnchor, yAnchor, xNew, yNew);
      glFlush();
    }
  }


  void mouse( int button, int state, int x, int y )
  // Function for processing mouse events.
  {
    if ( button == GLUT_LEFT_BUTTON )
      switch ( state )
      {
        case GLUT_DOWN: processLeftDown( x, y ); break;
        case GLUT_UP: processLeftUp( x, y ); break;
      }
  }


  int main( int argc, char * argv[] )
  {
    // Mask floating point exceptions.
    _control87( MCW_EM, MCW_EM );

    // Initialize glut with command line parameters.
    glutInit( & argc, argv );

    // Choose RGB display mode for normal screen window.
    glutInitDisplayMode( GLUT_RGB );

    // Set initial window size, position, and title.
    glutInitWindowSize( INITIAL_WIN_W, INITIAL_WIN_H );
    glutInitWindowPosition( INITIAL_WIN_X, INITIAL_WIN_Y );
    windowWidth = INITIAL_WIN_W;
    windowHeight = INITIAL_WIN_H;
    glutCreateWindow( "Rubber Bands Demo" );

    // You don't (yet) want to know what this does.
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    gluOrtho2D( 0.0, ( double )INITIAL_WIN_W, 0.0, ( double )INITIAL_WIN_H ), glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();
    glTranslatef( 0.375, 0.375, 0.0 );



    // Set the callbacks for the normal screen window.
    glutDisplayFunc( clearPicture );
    glutMouseFunc( mouse );
    glutReshapeFunc( reshape );
    glutKeyboardFunc( escExit );
    glutMotionFunc( rubberBand );
    glutPassiveMotionFunc( rubberBand );

    // Set the drawing color.
    glColor3f(1.0,1.0,1.0);

    // Set the color for clearing the window.
    glClearColor( 0.0, 0.0, 0.0, 0.0 );

    // Set up the menus.
    setMenus();

    glutMainLoop();
    return 0;
  }


