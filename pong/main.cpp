#include <GL/freeglut.h>
#include <GL/gl.h>
#include <GL/glu.h>

#include <cmath>
#include <iostream>

#include <mmx/vector2.hpp>
#include <mmx/util.hpp>

#include <windows.h>

const int SCREEN_WIDTH  = 640;
const int SCREEN_HEIGHT = 480;
const int SCREEN_FPS    = 60;

/// http://www.gamasutra.com/view/feature/131424/pool_hall_lessons_fast_accurate_.php?page=2

struct color {
	color(float r, float g, float b) : r(r), g(g), b(b) {}

	static const color red;
	static const color green;
	static const color blue;
	static const color yellow;
	static const color cyan;
	static const color purple;
	static const color white;
	static const color black;

	float r, g, b;
};

const color color::red    = color(1.f, 0.f, 0.f);
const color color::green  = color(0.f, 1.f, 0.f);
const color color::blue   = color(0.f, 0.f, 1.f);
const color color::yellow = color(1.f, 1.f, 0.f);
const color color::cyan   = color(0.f, 1.f, 1.f);
const color color::purple = color(.5f, 0.f, .5f);
const color color::white  = color(1.f, 1.f, 1.f);
const color color::black  = color(0.f, 0.f, 0.f);

struct block_t {
	block_t()
		: c(color::white)
		, p{0.f, 0.f}
		, v{0.f, 0.f}
		, s{20.f, 20.f}
	{}
	
	void draw() {
		glColor3f( c.r, c.g, c.b );
		glBegin( GL_QUADS );
			glVertex2f( p.x      , p.y );
			glVertex2f( p.x      , p.y + s.y );
			glVertex2f( p.x + s.x, p.y + s.y );
			glVertex2f( p.x + s.x, p.y );
		glEnd();
	}

	color         c;
	// position, velocity, size
	mmx::vector2f p, v, s;
};

struct paddle_t {
	
	paddle_t(color c) {
		for (unsigned i = 0; i < paddle_number; ++i) {
			paddles[i].v = {0, 0};
			paddles[i].s = {20, 20};
			paddles[i].c = c;
		}
	}
	
	void draw() {
		for (unsigned i = 0; i < paddle_number; ++i) {
			paddles[i].draw();
		}
	}
	
	void handle_keys(unsigned char key) {
		if (key == key_up)
			if (paddles[0].p.y > 0)
				for (unsigned i = 0; i < paddle_number; ++i)
					paddles[i].p += mmx::v2f(0, -20);

		if (key == key_down)
			if (paddles[paddle_number - 1].p.y + paddles[0].s.y < SCREEN_HEIGHT)
				for (unsigned i = 0; i < paddle_number; ++i)
					paddles[i].p += mmx::v2f(0, 20);
	}
	
	bool checkCollision(block_t& other) {
		// is on the same height as paddle and really close
		if (other.p.y >= paddles[0].p.y - paddles[0].s.y &&
		    other.p.y <= paddles[paddle_number - 1].p.y + paddles[0].s.y &&
		    mmx::abs(static_cast<int>(other.p.x - paddles[0].p.x)) <= paddles[0].s.x)
			for (unsigned i = 0; i < paddle_number; ++i)
				if (other.p.x <= paddles[i].p.x + paddles[i].s.x)
					return true;
		return false;
	}
	
	void resolveCollision(block_t& other) {
		// todo: bounce based on block hit
		other.v.x *= -1;
	}

	static const unsigned paddle_number = 7;
	block_t paddles[paddle_number];
	unsigned char key_up;
	unsigned char key_down;
};

struct left_paddle_t : public paddle_t {
	left_paddle_t(color c) 
		: paddle_t(c)
	{
		for (unsigned i = 0; i < paddle_number; ++i) {
			paddles[i].p = {0, 50.f + i * paddles[i].s.x};
		}
	}
};

struct right_paddle_t : public paddle_t {
	right_paddle_t(color c) 
		: paddle_t(c)
	{
		for (unsigned i = 0; i < paddle_number; ++i) {
			paddles[i].p = {SCREEN_WIDTH - paddles[i].s.x, 50.f + i * paddles[i].s.x};
		}
	}
};

left_paddle_t  player1(color::red);
right_paddle_t player2(color::green);

block_t  ball;

unsigned score_left = 0;
unsigned score_right = 0;

void spawn_ball() {
	ball.p.x = mmx::ranged_rand(250, 350);
	ball.p.y = mmx::ranged_rand(250, 350);
	
	ball.v.x = mmx::ranged_rand(-5, 5);
	ball.v.y = mmx::ranged_rand(-5, 5);
}

void draw_score() {
	glColor3f(1.f, 1.f, 1.f);
		
	glRasterPos2f(SCREEN_WIDTH / 2 - 10, 50);
	
	std::string text(mmx::to_string(score_left) + ":" + mmx::to_string(score_right));
	
	glutBitmapString(GLUT_BITMAP_HELVETICA_12, reinterpret_cast<const unsigned char*>(text.c_str()));
}

void update() {
	//Sleep(100);
	
	ball.p += ball.v;
	
	if (player1.checkCollision(ball))
		player1.resolveCollision(ball);
	
	if (player2.checkCollision(ball))
		player2.resolveCollision(ball);
		
	if (ball.p.x > SCREEN_WIDTH) {
		score_left++;
		spawn_ball();
	} else if (ball.p.x + ball.s.x < 0) {
		score_right++;
		spawn_ball();
	}
		
	if (ball.p.y + ball.s.y > SCREEN_HEIGHT || ball.p.y < 0)
		ball.v.y *= -1;
}

bool init_opengl() {
	//Set the viewport
    glViewport( 0.f, 0.f, SCREEN_WIDTH, SCREEN_HEIGHT );
	
    //Initialize Projection Matrix
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    glOrtho( 0.0, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0, 1.0, -1.0 );

    //Initialize Modelview Matrix
    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();

    //Initialize clear color
    glClearColor( 0.f, 0.f, 0.f, 1.f );

    //Check for error
    GLenum error = glGetError();
    if( error != GL_NO_ERROR )
		return false;

	spawn_ball();
	
	player1.key_up = 'w';
	player1.key_down = 's';
	
	player2.key_up = 'e';
	player2.key_down = 'd';
	
    return true;
}

void render() {
    //Clear color buffer
    glClear( GL_COLOR_BUFFER_BIT );

    //Reset modelview matrix
    glLoadIdentity();

    //Move to center of the screen
    //glTranslatef( SCREEN_WIDTH / 2.f, SCREEN_HEIGHT / 2.f, 0.f );
	glTranslatef( 0.f, 0.f, 0.f);
	
	draw_score();
	
	ball.draw();
	
	player1.draw();
	player2.draw();

    //Update screen
    glutSwapBuffers();
}

void handleKeys( unsigned char key, int x, int y ) {
	player1.handle_keys(key);
	player2.handle_keys(key);
	
	if (key == 'r')
		spawn_ball();
}

void runMainLoop( int val ) {
    //Frame logic
    update();
    render();

    //Run frame one more time
    glutTimerFunc( 1000 / SCREEN_FPS, runMainLoop, val );
}

int main( int argc, char* argv[] ) {
	
	/*if (argc == 1) {
		std::cout << "usage: " << argv[0] << " [options]\n"
				  << "options:\n"
				  << "\t -s server mode\n"
				  << "\t -c client mode, ip address must follow\n";
		return 1;
	}*/
	
	//Initialize FreeGLUT
	glutInit( &argc, argv );

	//Create OpenGL 2.1 context
	glutInitContextVersion( 2, 1 );

	//Create Double Buffered Window
	glutInitDisplayMode( GLUT_DOUBLE );
	glutInitWindowSize( SCREEN_WIDTH, SCREEN_HEIGHT );
	glutCreateWindow( "pong" );

	//Do post window/context creation initialization
	init_opengl();
	
	glutKeyboardFunc( handleKeys );

	//Set rendering function
	glutDisplayFunc( render );

	//Set main loop
	glutTimerFunc( 1000 / SCREEN_FPS, runMainLoop, 0 );

	//Start GLUT main loop
	glutMainLoop();

	return 0;
}
