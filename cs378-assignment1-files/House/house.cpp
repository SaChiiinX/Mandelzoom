/*-----------------------------------------------------------------------*/
/*  CS-378                  Computer Graphics             Tom Ellman     */
/*-----------------------------------------------------------------------*/
/*                          American Dream House                         */
/*-----------------------------------------------------------------------*/

#include <cstdlib>
#include <GL/glut.h>
#include <cfloat>

using namespace std;


#define INITIAL_WIN_W 800
#define INITIAL_WIN_H 800
#define INITIAL_WIN_X 150
#define INITIAL_WIN_Y 50


void drawHouse( )
{
  // Clear the window.
  glClear(GL_COLOR_BUFFER_BIT);

  // Draw the base of the house.
  glBegin( GL_LINE_LOOP );
  glVertex2i( 200, 200 );
  glVertex2i( 200, 500 );
  glVertex2i( 500, 500 );
  glVertex2i( 500, 200 );
  glEnd( );

  // Draw the roof of the house.
  glBegin( GL_LINE_STRIP );
  glVertex2i( 200, 500 );
  glVertex2i( 350, 630 );
  glVertex2i( 500, 500 );
  glEnd( );

  // Draw the door of the house.
  glBegin( GL_POINTS );
  for (int i=0; i<125; i++) glVertex2i(320,200+i);
  for (int i=0; i<60; i++) glVertex2i(320+i,325);
  for (int i=0; i<125; i++) glVertex2i(380,200+i);
  glEnd( );

  // Draw the antenna on the house.
  glBegin( GL_LINES );
  glVertex2i( 200, 500 );  glVertex2i( 200, 600 );
  glVertex2i( 175, 600 );  glVertex2i( 225, 600 );
  glVertex2i( 175, 575 );  glVertex2i( 225, 575 );
  glVertex2i( 175, 550 );  glVertex2i( 225, 550 );
  glEnd( );

  // Draw the base of the garage.
  glBegin( GL_LINE_LOOP );
  glVertex2i( 500, 200 );
  glVertex2i( 500, 350 );
  glVertex2i( 650, 350 );
  glVertex2i( 650, 200 );
  glEnd( );

  // Draw the roof of the garage.
  glBegin( GL_LINE_STRIP );
  glVertex2i( 500, 350 );
  glVertex2i( 575, 425 );
  glVertex2i( 650, 350 );
  glEnd( );

  // Set polygon fill/line mode. 
  // Forward facing polygons are filled. 
  // Backward facing polygons are outlined. 
  glPolygonMode(GL_FRONT,GL_FILL);
  glPolygonMode(GL_BACK,GL_LINE);  

  // Draw the windows of the house.
  // Generate vertices clockwise: Polygon faces backward.
  glBegin(GL_POLYGON);
  glVertex2i( 225, 375 );
  glVertex2i( 225, 475 );
  glVertex2i( 325, 475 );
  glVertex2i( 325, 375 );
  glEnd();
  // Generate vertices counter-clockwise: Polygon faces forward.
  glBegin(GL_POLYGON);
  glVertex2i( 375, 375 );
  glVertex2i( 475, 375 );
  glVertex2i( 475, 475 );
  glVertex2i( 375, 475 );
  glEnd();

  glFlush();
}

void reshape(int w, int h)
// Callback for processing reshape events.
{
  glViewport(0,0, w, h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(0.0, w, 0.0, h);
}


void escExit( GLubyte key, int, int )
// Callback for processing keyboard events.
{ if ( key == 27 /* ESC */ ) std::exit( 0 ); }



int main()
{
   // Mask floating point exceptions.
   _control87(MCW_EM,MCW_EM);

  // Choose RGB display mode for normal screen window.
  glutInitDisplayMode(GLUT_RGB);

  // Set initial window size, position, and title.
  glutInitWindowSize( INITIAL_WIN_W, INITIAL_WIN_H );
  glutInitWindowPosition( INITIAL_WIN_X, INITIAL_WIN_Y );
  glutCreateWindow("American Dream House");

  // You don't (yet) want to know what this does.
  glMatrixMode( GL_PROJECTION );
  glLoadIdentity( );
  gluOrtho2D( 0.0, (double) INITIAL_WIN_W, 0.0, (double) INITIAL_WIN_H );
  glMatrixMode( GL_MODELVIEW );
  glLoadIdentity( );

  // This is a hack.
  glTranslatef( 0.375, 0.375, 0.0 );

  // Set the color for clearing the normal screen window.
  glClearColor( 1.0, 1.0, 1.0, 0.0 );

  // Set the color for drawing the house.
  glColor3f(0.0, 0.0, 1.0);

  // Set the callbacks for the normal screen window.
  glutDisplayFunc( drawHouse );

  // Set the callback for reshape events.
  glutReshapeFunc(reshape);

  // Set the callback for keyboard events.
  glutKeyboardFunc( escExit );

  // Start the GLUT main loop.
  glutMainLoop( );

}
