#include <GL/freeglut.h>
#include <GL/gl.h>
#include <GL/glu.h>

#include <cmath>
#include <vector>
#include <algorithm>

#include <mmx/vector2.hpp>
#include <mmx/util.hpp>
#include <mmx/timer.hpp>

#ifdef _WIN32
	#include <windows.h>
#else
	#include <thread>
#endif

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
	block_t(mmx::vector2f pos)
		: c(color::white)
		, p(pos)
		, v{0.f, 0.f}
		, s{20.f, 20.f}
		, speed(20.f)
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
	
	void update() {
		p += v * speed;
		
		if (p.x < 0)
			p.x = SCREEN_WIDTH - s.x;
			
		if (p.x + s.x > SCREEN_WIDTH)
			p.x = 0;
			
		if (p.y < 0)
			p.y = SCREEN_HEIGHT - s.y;
			
		if (p.y + s.y > SCREEN_HEIGHT)
			p.y = 0;
	}

	color         c;
	// position, velocity, size
	mmx::vector2f p, v, s;
	float         speed;
};

struct food_t : public block_t {
	food_t(mmx::vector2f v) : block_t(v) {}

	int value;
	bool alive;
};

struct snake {
	snake()
		: level(150)
		, max_level(level)
		, score(0)
		, next_level(50)
		, running(true)
		, food(mmx::v2f(300, 100))
	{		
		body.push_back(block_t(mmx::v2f(120, 100)));
		body.at(0).c = color::red;
		body.at(0).v = {1.f, 0.f};
		
		for (int i = 0; i < 4; ++i)
			body.push_back(block_t(mmx::v2f(100 - i * 20, 100)));
			
		timer.start();
	}
	
	void draw_score() {
		glColor3f(1.f, 0.f, 0.f);
		glRasterPos2f(50, SCREEN_HEIGHT - 20);
		std::string scr("score: " + mmx::to_string(score));
		glutBitmapString(GLUT_BITMAP_HELVETICA_12, reinterpret_cast<const unsigned char*>(scr.c_str()));
	}
	
	void draw_level() {
		glColor3f(1.f, 0.f, 0.f);
		glRasterPos2f(SCREEN_WIDTH - 100, SCREEN_HEIGHT - 20);
		std::string lvl("level: " + mmx::to_string(max_level - level));
		glutBitmapString(GLUT_BITMAP_HELVETICA_12, reinterpret_cast<const unsigned char*>(lvl.c_str()));
	}
	
	void draw_next_level() {
		glColor3f(1.f, 0.f, 0.f);
		glRasterPos2f(SCREEN_WIDTH / 2 - 50, SCREEN_HEIGHT - 20);
		std::string lvl("next: " + mmx::to_string(next_level));
		glutBitmapString(GLUT_BITMAP_HELVETICA_12, reinterpret_cast<const unsigned char*>(lvl.c_str()));
	}
	
	void draw() {
		
		if (running) {
			food.draw();

			for (auto& b : body)
				b.draw();
		} else {
			for (unsigned i = 0; i < body.size(); ++i)
				body[i].v = {0.f, 0.f};
				
			glColor3f(1.f, 0.f, 0.f);
			glRasterPos2f(SCREEN_WIDTH / 2 - 75, 100);
			glutBitmapString(GLUT_BITMAP_TIMES_ROMAN_24, reinterpret_cast<const unsigned char*>("GAME OVER"));
		}
		
		// draw last so it stays on top
		draw_score();
		draw_level();
		draw_next_level();
	}
	
	void try_eat_food() {
		if (food.p == body[0].p)
			eat_food(food.value);
	}
	
	void eat_food(int v) {

		food.alive = false;

		add_to_tail();
		
		score += v;
	}
	
	void update() {
	
		if (score >= next_level) {
			next_level = (next_level * 4) / 3;
			if (--level <= 0)
				level = 1;
		}
			
		auto elapsed = timer.stop().ms();
		
		if (elapsed > 5000 || !food.alive) {
			timer.start();
			spawn_food();
		}

		if (is_over()) 
			running = false;
			
		if (running) {
			move_body_parts();
		
			try_eat_food();

			#ifdef _WIN32
			::Sleep(level);
			#else
			std::this_thread::sleep_for(std::chrono::duration::milliseconds(level));
			#endif
			for (auto& b : body)
				b.update();
		}
	}

	bool is_over() {
		for (unsigned i = 1; i < body.size(); ++i)
			if (body[0].p == body[i].p)
				return true;
		return false;
	}
	
	void move_body_parts() {
		for (unsigned i = body.size() - 1; i > 0; --i)
			body[i].p = body[i - 1].p;
	}
	
	void add_to_tail() {
		body.push_back(block_t(body.back().p));
	}
	
	void remove_from_tail() {
		if (body.size() > 1)
			body.pop_back();
	}
	
	void handle_input(unsigned char key) {
		switch (key) {				
			case 'w':
				if (body.at(0).v != mmx::v2f(0.f, 1.f))
					body.at(0).v = {0.f, -1.f};
				break;
				
			case 'a':
				if (body.at(0).v != mmx::v2f(1.f, 0.f))
					body.at(0).v = {-1.f, 0.f};
				break;
				
			case 's':
				if (body.at(0).v != mmx::v2f(0.f, -1.f))
					body.at(0).v = {0.f, 1.f};
				break;
				
			case 'd':
				if (body.at(0).v != mmx::v2f(-1.f, 0.f))
					body.at(0).v = {1.f, 0.f};
				break;
				
			default: ;
		}
	}
	
	void spawn_food() {

		while (true) {
			int x = mmx::ranged_rand(0, SCREEN_WIDTH - 20) / 20;
			int y = mmx::ranged_rand(0, SCREEN_HEIGHT - 20) / 20;
			
			food.p = mmx::v2f(x * 20, y * 20);
			
			// prevent food from spawning in body
			if (std::all_of(body.cbegin(), body.cend(), [&](const block_t& b){
				if (b.p != food.p)
					return true;
			}))
				break;
		}

		food.alive = true;
		
		switch (mmx::ranged_rand(0, 4)) {
		
			case 0:
				food.c = color::green;
				food.value = next_level / 8;
				break;
				
			case 1:
				food.c = color::blue;
				food.value = next_level / 6;
				break;
				
			case 2:
				food.c = color::yellow;
				food.value = next_level / 4;
				break;
				
			case 3:
				food.c = color::cyan;
				food.value = next_level / 3;
				break;
				
			case 4:
				food.c = color::purple;
				food.value = next_level / 2;
				break;
		}
	}

	std::vector<block_t> body;
	unsigned level; // also delay for sleep
	unsigned max_level;
	unsigned score;
	unsigned next_level;
	mmx::timer timer;
	bool running;
	food_t food;
};

snake S;

void update() {
	S.update();
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

	//draw_grid();

	S.draw();
	
    //Update screen
    glutSwapBuffers();
}

void handleKeys( unsigned char key, int x, int y ) {
	S.handle_input(key);
}

void runMainLoop( int val ) {
    //Frame logic
    update();
    render();

    //Run frame one more time
    glutTimerFunc( 1000 / SCREEN_FPS, runMainLoop, val );
}

int main( int argc, char* args[] ) {
	
	//Initialize FreeGLUT
	glutInit( &argc, args );

	//Create OpenGL 2.1 context
	glutInitContextVersion( 2, 1 );

	//Create Double Buffered Window
	glutInitDisplayMode( GLUT_DOUBLE );
	glutInitWindowSize( SCREEN_WIDTH, SCREEN_HEIGHT );
	glutCreateWindow( "snake snake snake" );

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
