/***********************************************************
             CSC418/2504, Fall 2009
  
                 penguin.cpp
                 
       Simple demo program using OpenGL and the glut/glui 
       libraries

  
    Instructions:
        Please read the assignment page to determine 
        exactly what needs to be implemented.  Then read 
        over this file and become acquainted with its 
        design.

        Add source code where it appears appropriate. In
        particular, see lines marked 'TODO'.

        You should not need to change the overall structure
        of the program. However it should be clear what
        your changes do, and you should use sufficient comments
        to explain your code.  While the point of the assignment
        is to draw and animate the character, you will
        also be marked based on your design.

***********************************************************/

#ifdef _WIN32
#include <windows.h>
#endif

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <GL/glui.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#ifndef _WIN32
#include <unistd.h>
#else
void usleep(unsigned int nanosec)
{
    Sleep(nanosec / 1000);
}
#endif


// *************** GLOBAL VARIABLES *************************


const float PI = 3.14159;

// --------------- USER INTERFACE VARIABLES -----------------

// Window settings
int windowID;               // Glut window ID (for display)
GLUI *glui;                 // Glui window (for controls)
int Win[2];                 // window (x,y) size


// ---------------- ANIMATION VARIABLES ---------------------

// Animation settings
int animate_mode = 0;       // 0 = no anim, 1 = animate
int animation_frame = 0;      // Specify current frame of animation

// Joint parameters
const float JOINT_MIN = -45.0f;
const float JOINT_MAX =  45.0f;

const float LAG_MIN = -30.0f;
const float LAG_MAX =  60.0f;

const float M_MIN = -2.0f;
const float M_MAX = 0.0f;

const float M_BV_MIN = -30.0f;
const float M_BV_MAX = 30.0f;

const float M_BH_MIN = -200.0f;
const float M_BH_MAX = 200.0f;

float joint_rot = 0.0f;

//////////////////////////////////////////////////////
// TODO: Add additional joint parameters here
//////////////////////////////////////////////////////
float joint_rot_LL = 0.0f;
float joint_rot_LF = 0.0f;

float joint_rot_RL = 0.0f;
float joint_rot_RF = 0.0f;

float joint_rot_H = 0.0f;

float joint_move_M = 0.0f;

float joint_move_BV = 0.0f;
float joint_move_BH = 0.0f;


// ***********  FUNCTION HEADER DECLARATIONS ****************


// Initialization functions
void initGlut(char* winName);
void initGlui();
void initGl();


// Callbacks for handling events in glut
void myReshape(int w, int h);
void animate();
void display(void);

// Callback for handling events in glui
void GLUI_Control(int id);


// Functions to help draw the object
void drawSquare(float size);
void drawtrapezoid(float a, float b, float h);

void drawBody();
void drawCircle(float r);
void drawHead();
void drawMouth(float a, float b);

float nmb(float a);

// Return the current system clock (in seconds)
double getTime();


// ******************** FUNCTIONS ************************



// main() function
// Initializes the user interface (and any user variables)
// then hands over control to the event handler, which calls 
// display() whenever the GL window needs to be redrawn.
int main(int argc, char** argv)
{

    // Process program arguments
    if(argc != 3) {
        printf("Usage: demo [width] [height]\n");
        printf("Using 300x200 window by default...\n");
        Win[0] = 300;
        Win[1] = 200;
    } else {
        Win[0] = atoi(argv[1]);
        Win[1] = atoi(argv[2]);
    }


    // Initialize glut, glui, and opengl
    glutInit(&argc, argv);
    initGlut(argv[0]);
    initGlui();
    initGl();

    // Invoke the standard GLUT main event loop
    glutMainLoop();

    return 0;         // never reached
}


// Initialize glut and create a window with the specified caption 
void initGlut(char* winName)
{
    // Set video mode: double-buffered, color, depth-buffered
    glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

    // Create window
    glutInitWindowPosition (0, 0);
    glutInitWindowSize(Win[0],Win[1]);
    windowID = glutCreateWindow(winName);

    // Setup callback functions to handle events
    glutReshapeFunc(myReshape); // Call myReshape whenever window resized
    glutDisplayFunc(display);   // Call display whenever new frame needed 
}


// Quit button handler.  Called when the "quit" button is pressed.
void quitButton(int)
{
  exit(0);
}

// Animate button handler.  Called when the "animate" checkbox is pressed.
void animateButton(int)
{
  // synchronize variables that GLUT uses
  glui->sync_live();

  animation_frame = 0;
  if(animate_mode == 1) {
    // start animation
    GLUI_Master.set_glutIdleFunc(animate);
  } else {
    // stop animation
    GLUI_Master.set_glutIdleFunc(NULL);
  }
}

// Initialize GLUI and the user interface
void initGlui()
{
    GLUI_Master.set_glutIdleFunc(NULL);

    // Create GLUI window
    glui = GLUI_Master.create_glui("Glui Window", 0, Win[0]+10, 0);

    // Create a control to specify the rotation of the joint
    GLUI_Spinner *joint_spinner
        = glui->add_spinner("Joint_A", GLUI_SPINNER_FLOAT, &joint_rot);
    joint_spinner->set_speed(0.5);
    joint_spinner->set_float_limits(JOINT_MIN, JOINT_MAX, GLUI_LIMIT_CLAMP);

    ///////////////////////////////////////////////////////////
    // TODO: 
    //   Add controls for additional joints here
    ///////////////////////////////////////////////////////////
    // control pannel of joint of left leg
    GLUI_Spinner *joint_LL_spinner
        = glui->add_spinner("Joint_LL", GLUI_SPINNER_FLOAT, &joint_rot_LL);
    joint_LL_spinner->set_speed(1.0);
    joint_LL_spinner->set_float_limits(LAG_MIN, LAG_MAX, GLUI_LIMIT_CLAMP);
    
    // control pannel of joint of left foot
    GLUI_Spinner *joint_LF_spinner
        = glui->add_spinner("Joint_LF", GLUI_SPINNER_FLOAT, &joint_rot_LF);
    joint_LF_spinner->set_speed(1.0);
    joint_LF_spinner->set_float_limits(JOINT_MIN + 20, JOINT_MAX + 20, GLUI_LIMIT_CLAMP);
    
    // control pannel of joint of right leg
    GLUI_Spinner *joint_RL_spinner
        = glui->add_spinner("Joint_RL", GLUI_SPINNER_FLOAT, &joint_rot_RL);
    joint_RL_spinner->set_speed(-1.0);
    joint_RL_spinner->set_float_limits(LAG_MIN, LAG_MAX, GLUI_LIMIT_CLAMP);
    
    // control pannel of joint of right foot
    GLUI_Spinner *joint_RF_spinner
        = glui->add_spinner("Joint_RF", GLUI_SPINNER_FLOAT, &joint_rot_RF);
    joint_RF_spinner->set_speed(-1.0);
    joint_RF_spinner->set_float_limits(JOINT_MIN + 20, JOINT_MAX + 20, GLUI_LIMIT_CLAMP);
    
    // control pannel of joint of head
    GLUI_Spinner *joint_H_spinner
        = glui->add_spinner("Joint_H", GLUI_SPINNER_FLOAT, &joint_rot_H);
    joint_H_spinner->set_speed(0.4);
    joint_H_spinner->set_float_limits(JOINT_MIN + 20, JOINT_MAX - 20, GLUI_LIMIT_CLAMP);
    
    // control pannel of joint of mouth
    GLUI_Spinner *joint_M_spinner
        = glui->add_spinner("Joint_M", GLUI_SPINNER_FLOAT, &joint_move_M);
    joint_M_spinner->set_speed(1.0);
    joint_M_spinner->set_float_limits(M_MIN, M_MAX, GLUI_LIMIT_CLAMP);
    
    // control pannel of vertical joint of body
    GLUI_Spinner *joint_M_BV_spinner
        = glui->add_spinner("Joint_M_BV", GLUI_SPINNER_FLOAT, &joint_move_BV);
    joint_M_BV_spinner->set_speed(2.0);
    joint_M_BV_spinner->set_float_limits(M_BV_MIN, M_BV_MAX, GLUI_LIMIT_CLAMP);
    
    // control pannel of horizontal joint of body
    GLUI_Spinner *joint_M_BH_spinner
        = glui->add_spinner("Joint_M_BH", GLUI_SPINNER_FLOAT, &joint_move_BH);
    joint_M_BH_spinner->set_speed(0.2);
    joint_M_BH_spinner->set_float_limits(M_BH_MIN, M_BH_MAX, GLUI_LIMIT_CLAMP);

    // Add button to specify animation mode 
    glui->add_separator();
    glui->add_checkbox("Animate", &animate_mode, 0, animateButton);

    // Add "Quit" button
    glui->add_separator();
    glui->add_button("Quit", 0, quitButton);

    // Set the main window to be the "active" window
    glui->set_main_gfx_window(windowID);
}


// Performs most of the OpenGL intialization
void initGl(void)
{
    // glClearColor (red, green, blue, alpha)
    // Ignore the meaning of the 'alpha' value for now
    glClearColor(0.7f,0.7f,0.9f,1.0f);
}




// Callback idle function for animating the scene
void animate()
{
    // Update geometry
    const double joint_rot_speed = 0.5;
    double joint_rot_t = (sin(animation_frame*joint_rot_speed) + 1.0) / 2.0;
    joint_rot = joint_rot_t * JOINT_MIN + (1 - joint_rot_t) * JOINT_MAX;
    
    ///////////////////////////////////////////////////////////
    // TODO:
    //   Modify this function animate the character's joints
    //   Note: Nothing should be drawn in this function!  OpenGL drawing
    //   should only happen in the display() callback.
    ///////////////////////////////////////////////////////////
    
    // left leg
    const double joint_rot_LL_speed = 1.0;
    double joint_rot_LL_t = (sin(animation_frame*joint_rot_LL_speed) + 1.0) / 2.0;
    joint_rot_LL = joint_rot_LL_t * LAG_MIN + (1 - joint_rot_LL_t) * LAG_MAX;
    // left foot
    const double joint_rot_LF_speed = 1.0;
    double joint_rot_LF_t = (sin(animation_frame*joint_rot_LF_speed) + 1.0) / 2.0;
    joint_rot_LF = joint_rot_LF_t * (JOINT_MIN + 45) + (1 - joint_rot_LF_t) * (JOINT_MAX + 20);
    // right leg
    const double joint_rot_RL_speed = -1.0;
    double joint_rot_RL_t = (sin(animation_frame*joint_rot_RL_speed) + 1.0) / 2.0;
    joint_rot_RL = joint_rot_RL_t * LAG_MIN + (1 - joint_rot_RL_t) * LAG_MAX;
    // left foot
    const double joint_rot_RF_speed = -1.0;
    double joint_rot_RF_t = (sin(animation_frame*joint_rot_RF_speed) + 1.0) / 2.0;
    joint_rot_RF = joint_rot_RF_t * (JOINT_MIN + 45) + (1 - joint_rot_RF_t) * (JOINT_MAX + 20);
    // head
    const double joint_rot_H_speed = 0.4;
    double joint_rot_H_t = (sin(animation_frame*joint_rot_H_speed + (1 * PI)) + 1.0) / 2.0;
    joint_rot_H = joint_rot_H_t * (JOINT_MIN + 20) + (1 - joint_rot_H_t) * (JOINT_MAX - 20);
    // mouth
    const double joint_move_M_speed = 1.0;
    double joint_move_M_t = (sin(animation_frame*joint_move_M_speed + (1 * PI))+ 1.0) / 2.0;
    joint_move_M = joint_move_M_t * (M_MIN) + (1 - joint_move_M_t) * (M_MAX);
    // body vertical
    const double joint_move_BV_speed = 2.0;
    double joint_move_BV_t = (sin(animation_frame*joint_move_BV_speed)+ 1.0) / 2.0;
    joint_move_BV = joint_move_BV_t * (M_BV_MIN) + (1 - joint_move_BV_t) * (M_BV_MAX);
    // body horizontal
    const double joint_move_BH_speed = 0.2;
    double joint_move_BH_t = (sin(animation_frame*joint_move_BH_speed)+ 1.0) / 2.0;
    joint_move_BH = joint_move_BH_t * (M_BH_MIN) + (1 - joint_move_BH_t) * (M_BH_MAX);

    // Update user interface
    glui->sync_live();

    // Tell glut window to update itself.  This will cause the display()
    // callback to be called, which renders the object (once you've written
    // the callback).
    glutSetWindow(windowID);
    glutPostRedisplay();

    // increment the frame number.
    animation_frame++;

    // Wait 50 ms between frames (20 frames per second)
    usleep(50000);
}


// Handles the window being resized by updating the viewport
// and projection matrices
void myReshape(int w, int h)
{
    // Setup projection matrix for new window
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-w/2, w/2, -h/2, h/2);

    // Update OpenGL viewport and internal variables
    glViewport(0,0, w,h);
    Win[0] = w;
    Win[1] = h;
}



// display callback
//
// This gets called by the event handler to draw
// the scene, so this is where you need to build
// your scene -- make your changes and additions here.
// All rendering happens in this function.  For Assignment 1,
// updates to geometry should happen in the "animate" function.
// Pink: glColor3f(1, 0.7529411764705882, 0.796078431372549);
void display(void)
{
    // glClearColor (red, green, blue, alpha)
    // Ignore the meaning of the 'alpha' value for now
    glClearColor(0.7f,0.7f,0.9f,1.0f);

    // OK, now clear the screen with the background colour
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  

    // Setup the model-view transformation matrix
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    ///////////////////////////////////////////////////////////
    // TODO:
    //   Modify this function draw the scene
    //   This should include function calls to pieces that
    //   apply the appropriate transformation matrice and
    //   render the individual body parts.
    ///////////////////////////////////////////////////////////

    // Draw our hinged object
    const float BODY_WIDTH = 35.0f;
    const float BODY_LENGTH = 60.0f;
    const float ARM_LENGTH = 100.0f;
    const float ARM_WIDTH = 40.0f;
    
    const float LEG_LENGTH = 80.0f;
    const float LEG_WIDTH = 20.0f;
    
    const float HEAD_LENGTH = 80.0f;
    const float HEAD_WIDTH = 100.0f;
    
    const float MOUTH_LENGTH = 10.0f;
    const float MOUTH_WIDTH = 60.0f;
    
    
    //basic matrix of the whole graph
    glPushMatrix();
		//horizontal movement of the whole penguin
		glTranslatef(joint_move_BH, 0, 0);
		//vertical movement of the whole penguin
		glTranslatef(0, joint_move_BV, 0);
		glPushMatrix();
			// Push the current transformation matrix on the stack
			glPushMatrix();	
				// Draw the 'body'
				glPushMatrix();
					// Scale square to size of body
					glTranslatef(0, -20, 0);
					glScalef(BODY_WIDTH, BODY_LENGTH, 1.0);

					// Set the colour to green
					glColor3f(0.0, 1.0, 0.0);

					// Draw the square for the body
					drawBody();
					
				glPopMatrix();
				
				// Draw the 'ARM'

				// Move the arm to the joint hinge
				glTranslatef(BODY_WIDTH/2, BODY_LENGTH + ARM_WIDTH + 25, 0.0);

				// Rotate along the hinge
				glRotatef(joint_rot, 0.0, 0.0, 1.0);

				// Scale the size of the arm
				glScalef(ARM_WIDTH, ARM_LENGTH, 1.0);

				// Move to center location of arm, under previous rotation
				glTranslatef(0, -0.4, 0.0);

				// Draw the square for the arm
				glColor3f(1.0, 1.0, 0);
				drawtrapezoid(1.0, 0.5, 1.0);
			glPopMatrix();
			
			//draw a circle at position of rotation center
			glTranslatef(BODY_WIDTH/2, BODY_LENGTH + ARM_WIDTH + 25, 0.0);
			glScalef(5, 5, 0.0);
			glColor3f(0.0, 0.0, 1.0);
			drawCircle(1.0);
		glPopMatrix();
		
		// Start to Draw the right leg
		glPushMatrix();
			//set the rotation center
			glTranslatef(BODY_WIDTH, -BODY_LENGTH, 0);
			glRotatef(joint_rot_RL, 0.0, 0.0, 1.0);
			
			// Draw the leg
			glTranslatef(0 , -LEG_LENGTH / 2 + 10, 0);
			glScalef(LEG_WIDTH, LEG_LENGTH, 1.0);
			glColor3f(1, 0, 0);
			drawSquare(1.0);
			
			// Draw a circle at the position of rotation center
			glScalef(1.0 / LEG_WIDTH, 1.0 / LEG_LENGTH, 1.0);
			glTranslatef(0 , LEG_LENGTH / 2 - 10, 0);
			glScalef(5, 5, 0.0);
			glColor3f(0, 0, 1);
			drawCircle(1.0);
			glScalef(0.2, 0.2, 0.0);
			
			// Start to draw foot part	
			glPushMatrix();
				//set the rotation center of foot
				glTranslatef(0, -LEG_LENGTH + 20, 0);
				glRotatef(joint_rot_RF, 0.0, 0.0, 1.0);
				
				//draw the foot
				glTranslatef(-LEG_LENGTH / 2 + 10, 0, 0);
				glScalef(LEG_LENGTH, LEG_WIDTH, 1.0);
				glColor3f(1, 0, 1);
				drawSquare(1.0);
				
				//draw a circle at the postion of rotation center
				glScalef(1.0 / LEG_LENGTH, 1.0 / LEG_WIDTH, 1.0);
				glTranslatef(LEG_LENGTH / 2 - 10 , 0, 0);
				glScalef(5, 5, 0.0);
				glColor3f(0, 0, 1);
				drawCircle(1.0);
			glPopMatrix();
		glPopMatrix();
		
		// Start to draw the left leg
		glPushMatrix();
			//set the rotation center
			glTranslatef(-BODY_WIDTH , -BODY_LENGTH, 0);
			glRotatef(joint_rot_LL, 0.0, 0.0, 1.0);
			
			// Draw the leg
			glTranslatef(0 , -LEG_LENGTH / 2 + 10, 0);
			glScalef(LEG_WIDTH, LEG_LENGTH, 1.0);
			glColor3f(1, 0, 0);
			drawSquare(1.0);
			
			// Draw a circle at the position of rotation center
			glScalef(1.0 / LEG_WIDTH, 1.0 / LEG_LENGTH, 1.0);
			glTranslatef(0 , LEG_LENGTH / 2 - 10, 0);
			glScalef(5, 5, 0.0);
			glColor3f(0, 0, 1);
			drawCircle(1.0);
			glScalef(0.2, 0.2, 0.0);
			
			// Start to draw foot part
			glPushMatrix();
				//set the rotation center of foot
				glTranslatef(0, -LEG_LENGTH + 20, 0);
				glRotatef(joint_rot_LF, 0.0, 0.0, 1.0);
				
				//draw the foot
				glTranslatef(-LEG_LENGTH / 2 + 10, 0, 0);
				glScalef(LEG_LENGTH, LEG_WIDTH, 1.0);
				glColor3f(1, 0, 1);
				drawSquare(1.0);
				
				//draw a circle at the postion of rotation center
				glScalef(1.0 / LEG_LENGTH, 1.0 / LEG_WIDTH, 1.0);
				glTranslatef(LEG_LENGTH / 2 - 10 , 0, 0);
				glScalef(5, 5, 0.0);
				glColor3f(0, 0, 1);
				drawCircle(1.0);
			glPopMatrix();
		glPopMatrix();
		
		// Start to draw the head
		glPushMatrix();
			glTranslatef(0.0, 4 * BODY_LENGTH + 10, 0.0);
			//set the rotation center of head
			glRotatef(joint_rot_H, 0.0, 0.0, 1.0);
			glTranslatef(0.0, -10, 0.0);
			//draw the head
			glScalef(HEAD_WIDTH, HEAD_LENGTH, 1.0);
			glColor3f(0.5, 0.5, 0.5);
			drawHead();
			
			// draw the rotation center of head
			glScalef(1 / HEAD_WIDTH, 1 / HEAD_LENGTH, 0.0);
			glTranslatef(0.0, 10.0, 0.0);
			glScalef(5, 5, 1.0);
			glColor3f(0.0, 0.0, 1.0);
			drawCircle(1.0);
			glScalef(0.2, 0.2, 1.0);
			
			//start to draw eye part
			glPushMatrix();
				//draw white of eye
				glTranslatef(-HEAD_WIDTH * 0.3, HEAD_LENGTH * 0.7 - 10, 0.0);
				glScalef(10.0, 10.0, 1.0);
				glColor3f(1.0, 1.0, 1.0);
				drawCircle(1.0);
				
				//draw black of eye
				glScalef(0.5, 0.5, 1.0);
				glColor3f(0.0, 0.0, 0.0);
				drawCircle(1.0);
			glPopMatrix();
			
			//start to draw the mouth part
			//draw the upper part of mouth
			glTranslatef(-HEAD_WIDTH, HEAD_LENGTH * 0.2, 0.0);
			glScalef(-MOUTH_WIDTH, MOUTH_LENGTH, 1.0);
			glColor3f(1.0, 1.0, 0.0);
			drawMouth(1.0, 0.6);
			
			//start to draw the lower mouth part
			glPushMatrix();
				//set lower part of mouth's move center
				glTranslatef(0, joint_move_M, 0);
				glColor3f(1.0, 1.0, 0.0);
				//draw the lower part of mouth
				drawMouth(-0.8, -0.4);
			glPopMatrix();
		glPopMatrix();
	
	glPopMatrix();
	
	// Execute any GL functions that are in the queue just to be safe
	glFlush();

	// Now, show the frame buffer that we just drew into.
	// (this prevents flickering).
	glutSwapBuffers();
}


// Draw a square of the specified size, centered at the current location
void drawSquare(float width)
{
    // Draw the square
    glBegin(GL_POLYGON);
    glVertex2d(-width/2, -width/2);
    glVertex2d(width/2, -width/2);
    glVertex2d(width/2, width/2);
    glVertex2d(-width/2, width/2);
    glEnd();
}

// Draw a trapezoid of the specified a, b and height. centered at the current location 
void drawtrapezoid(float a, float b, float h){
	glBegin(GL_POLYGON);
	glVertex2d(-b/2, -h/2);
	glVertex2d(b/2, -h/2);
	glVertex2d(a/2, h/2);
	glVertex2d(-a/2, h/2);
	glEnd();
}

// Draw the penguin'body
void drawBody(){
	glBegin(GL_POLYGON);
	glVertex2d(-1.0, -1.0);
	glVertex2d(1.0, -1.1);
	glVertex2d(3.0, 0.0);
	glVertex2d(1.0, 4.5);
	glVertex2d(-1.0, 4.5);
	glVertex2d(-3.0, 0.0);
	glEnd();
}

// Draw a circle with given radiate
void drawCircle(float r){
	int n = 20;
	glBegin(GL_POLYGON);
	for(int i=0; i<n; i++){
		glVertex2f(r * cos(2 * PI / n * i), r * sin(2 * PI / n * i));
	}
	glEnd();
}

// Draw the penguin's head
void drawHead(){
	glBegin(GL_POLYGON);
	glVertex2d(-0.5, 0);
	glVertex2d(0.5, 0);
	glVertex2d(0.45, 0.8);
	glVertex2d(-0.25, 1.0);
	glVertex2d(-0.45, 0.8);
	glEnd();
}

// Draw the penguin's mouth
void drawMouth(float a, float b){
	glBegin(GL_POLYGON);
	glVertex2d(0.0, 0.0);
	glVertex2d(0.0, b);
	glVertex2d(-1, a);
	glVertex2d(-1, 0.0);
	glEnd();
}
