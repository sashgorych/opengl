//
// Perspective view of a color cube using LookAt() and Ortho()
//
// Colors are assigned to each vertex and then the rasterizer interpolates
//   those colors across the triangles.
//

#include "../Angel/Angel.h"

#include <iostream>
#include <vector>
using namespace std;
typedef Angel::vec4  color4;
typedef Angel::vec4  point4;



//const int NumVertices = 36; //(6 faces)(2 triangles/face)(3 vertices/triangle)

//point4 points[NumVertices];
//color4 colors[NumVertices];


vector<point4> points;
vector<color4> colors;
// Vertices of a unit cube centered at origin, sides aligned with axes
point4 vertices[8] = {
    point4( -0.5, -0.5,  0.5,	1.0),
    point4( -0.5,  0.5,  0.5,	1.0),
    point4(  0.5,  0.5,  0.5,	1.0),
    point4(  0.5, -0.5,  0.5,	1.0),
    point4( -0.5, -0.5, -0.5,	1.0),
    point4( -0.5,  0.5, -0.5,	1.0),
    point4(  0.5,  0.5, -0.5,	1.0),
    point4(  0.5, -0.5, -0.5,	1.0)
};

// RGBA colors
color4 vertex_colors[8] = {
    color4( 0.0, 0.0, 0.0, 1.0 ),  // black
    color4( 1.0, 0.0, 0.0, 1.0 ),  // red
    color4( 1.0, 1.0, 0.0, 1.0 ),  // yellow
    color4( 0.0, 1.0, 0.0, 1.0 ),  // green
    color4( 0.0, 0.0, 1.0, 1.0 ),  // blue
    color4( 1.0, 0.0, 1.0, 1.0 ),  // magenta
    color4( 1.0, 0.7, 0.5, 1.0 ),  // off-white
    color4( 0.0, 1.0, 1.0, 1.0 )   // cyan
};

// Viewing transformation parameters

GLfloat radius = 1.0;
GLfloat theta = 0.1;
GLfloat phi = 0.1;

const GLfloat  dr = 5.0 * DegreesToRadians;

GLuint  model,view;  // model-view matrix uniform shader variable location

// Projection transformation parameters

GLfloat  the_left = -2.0, the_right = 2.0;
GLfloat  bottom = -2.0, top = 2.0;
GLfloat  zNear = 0.1, zFar = 5.0;

GLuint  projection; // projection matrix uniform shader variable location

//----------------------------------------------------------------------------

// quad generates two triangles for each face and assigns colors
//    to the vertices

int Index = 0;

void
quad( int a, int b, int c, int d )
{
	colors.push_back(vertex_colors[a]);
	points.push_back(vertices[a]);
	colors.push_back(vertex_colors[a]);
	points.push_back(vertices[b]);
	colors.push_back(vertex_colors[a]);
	points.push_back(vertices[c]);

	colors.push_back(vertex_colors[a]);
	points.push_back(vertices[a]);
	colors.push_back(vertex_colors[a]);
	points.push_back(vertices[c]);
	colors.push_back(vertex_colors[a]);
	points.push_back(vertices[d]);

}

//----------------------------------------------------------------------------

// generate 12 triangles: 36 vertices and 36 colors
void
colorcube()
{
    quad( 1, 0, 3, 2 );
    quad( 2, 3, 7, 6 );
    quad( 3, 0, 4, 7 );
    quad( 6, 5, 1, 2 );
    quad( 4, 5, 6, 7 );
    quad( 5, 4, 0, 1 );
}

//----------------------------------------------------------------------------

// OpenGL initialization
void
init()
{
    colorcube();

    // Create a vertex array object
    GLuint vao;
    glGenVertexArrays( 1, &vao );
    glBindVertexArray( vao );

    // Create and initialize a buffer object
    GLuint buffer;
    glGenBuffers( 1, &buffer );
    glBindBuffer( GL_ARRAY_BUFFER, buffer );
    glBufferData( GL_ARRAY_BUFFER, sizeof(point4)*points.size() + sizeof(color4)*colors.size(),
		  NULL, GL_STATIC_DRAW );
    glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof(point4)*points.size(), &points[0] );
    glBufferSubData( GL_ARRAY_BUFFER, sizeof(point4)*points.size(), sizeof(color4)*colors.size(), &colors[0] );

    // Load shaders and use the resulting shader program
    GLuint program = InitShader( "../glsl/v_model_view_projection.glsl", "../glsl/fshader41.glsl" );
    glUseProgram( program );

    // set up vertex arrays
    GLuint vPosition = glGetAttribLocation( program, "vPosition" );
    cout << "vPosition = " << vPosition << endl;
    glEnableVertexAttribArray( vPosition );
    glVertexAttribPointer( vPosition, 4, GL_FLOAT, GL_FALSE, 0,
			   BUFFER_OFFSET(0) );

    GLuint vColor = glGetAttribLocation( program, "vColor" ); 
    glEnableVertexAttribArray( vColor );
    glVertexAttribPointer( vColor, 4, GL_FLOAT, GL_FALSE, 0,
			   BUFFER_OFFSET(sizeof(point4)*points.size()) );

    model = glGetUniformLocation( program, "model" );
    view = glGetUniformLocation( program, "view" );
    projection = glGetUniformLocation( program, "projection" );
    
    glEnable( GL_DEPTH_TEST );
    glClearColor( 1.0, 1.0, 1.0, 1.0 ); 
}

//----------------------------------------------------------------------------

void draw_cube(mat4 model_m)
{
    glUniformMatrix4fv( model, 1, GL_TRUE, model_m );
	glDrawArrays( GL_TRIANGLES, 0, points.size() );
}

void draw_arm(mat4 model_m)
{
	cout << "da model_m" << model_m;
	mat4 arm_m=model_m*Scale(2,0.25,0.25);
	cout << endl <<"da arm_m (scaled)" << arm_m;
	
	draw_cube(arm_m);
	arm_m=Translate(-1.3,-0.9,0)*arm_m;
	arm_m=RotateZ(60.0)*arm_m;
	cout << endl << "da arm_m (rot-trans)" << arm_m;
	draw_cube(arm_m);
}

void draw_robot(mat4 model_m)
{
	
	mat4 head_m=model_m*Translate(0,1.4,0);
	head_m*=Scale(0.5,0.5,1);
	draw_cube(head_m);

	mat4 neck_m=model_m*Translate(0,1,0)* Scale(0.15,0.3,1);
	draw_cube(neck_m);
	
	mat4 body_m=model_m*Translate(0,.35,0);
	draw_cube(body_m);
	
	mat4 arm_m=Scale(.2,.2,.2)*model_m;
	arm_m=arm_m*Translate(-1.8,10,0);
	draw_arm(arm_m);
}

void draw_star(mat4 model_m)
{
	
	model_m *=Scale(0.5,1,1);
	model_m *=Translate(1,0,0);
	draw_cube(model_m);
	model_m *=RotateZ(30);
	draw_cube(model_m);
	model_m *=RotateZ(30);
	draw_cube(model_m);
}

void draw_rect(mat4 model_m)
{
	model_m*=Scale(2,0.5,1);
	draw_cube(model_m);
}

float rot_y=0;
void
display( void )
{
//	glEnable(GL_CULL_FACE);
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	cout << "displaying" << endl;
    mat4  p = Ortho( the_left, the_right, bottom, top, zNear, zFar );
    glUniformMatrix4fv( projection, 1, GL_TRUE, p );

    point4  eye( 0,0,2,	 1.0 );
    point4  at( 0.0, 0.0, 0.0, 1.0 );
    vec4    up( 0.0, 1.0, 0.0, 0.0 );

    mat4  view_m = LookAt( eye, at, up );
    glUniformMatrix4fv( view,  1, GL_TRUE, view_m);
    cout << view_m << endl;
/*
    mat4 model_m; // identity
    model_m*=Translate(1,-1,0);
	draw_cube(model_m);
*/
	//draw_robot(mat4());
	//draw_robot(RotateZ(50));

//	draw_cube(mat4());
	mat4 m;
	m*=RotateY(rot_y);
	m*=Translate(-1,0.5,0);
	m*=Scale(0.2,0.2,1);
	m*=Translate(-.5,.5,0);
	draw_rect(m);
	m*=Translate(-1,-1,0);
	m*=RotateZ(90);
	draw_rect(m);
	
	//model_m*=RotateZ(45);
	//draw_cube(m);

//	draw_star(model_m);
    glutSwapBuffers();
}

//----------------------------------------------------------------------------

void
keyboard( unsigned char key, int x, int y )
{
    switch( key ) {
	case 033: // Escape Key
	case 'q': case 'Q':
	    exit( EXIT_SUCCESS );
	    break;

	case 'x': the_left *= 1.1; the_right *= 1.1; break;
	case 'X': the_left *= 0.9; the_right *= 0.9; break;
/*	
	case 'y': bottom *= 1.1; top *= 1.1; break;
	case 'Y': bottom *= 0.9; top *= 0.9; break;
	* 
*/	
	case 'y': rot_y+=10; break;
	case 'Y': rot_y-=10; break;
	case 'z': zNear  *= 1.1; zFar *= 1.1; break;
	case 'Z': zNear *= 0.9; zFar *= 0.9; break;
	case 'r': radius *= 2.0; break;
	case 'R': radius *= 0.5; break;
	case 'o': theta += dr; break;
	case 'O': theta -= dr; break;
	case 'p': phi += dr; break;
	case 'P': phi -= dr; break;

	case ' ':  // reset values to their defaults
	    the_left = -1.0;
	    the_right
	    
	     = 1.0;
	    bottom = -1.0;
	    top = 1.0;
	    zNear = 0.5;
	    zFar = 3.0;

	    radius = 1.0;
	    theta  = 0.0;
	    phi    = 0.0;
	    break;
    }

    glutPostRedisplay();
}

//----------------------------------------------------------------------------

void
reshape( int width, int height )
{
    glViewport( 0, 0, width, height );
}

//----------------------------------------------------------------------------
int
main( int argc, char **argv )
{

	std::cout << "initing " << std::endl;

    glutInit( &argc, argv );
    glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH );
    glutInitWindowSize( 500, 500 );
    glutCreateWindow( "Color Cube" );


    glewInit();

    init();

    glutDisplayFunc( display );
    glutKeyboardFunc( keyboard );
    glutReshapeFunc( reshape );

    glutMainLoop();
    return 0;
}
