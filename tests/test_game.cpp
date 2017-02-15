#define DARWIN_IGNORE_WARNING
#define DARWIN_FORCE_BUILTIN
#include "../headers/darwin.hpp"
#include <cmath>

struct sandbox {
	double gravity=9.8;
	double air_drop=1;
	double delta_time=0.2;
} world;

struct entity {
	double mass=0.1;
	double size=0.01;
	double speed_x=10;
	double speed_y=0;
	double posit_x=0;
	double posit_y=0;
} ball;

void run()
{
	double ax(0),ay(0);
	if(ball.speed_x>0)
		ax=-(world.air_drop*ball.size*ball.speed_x)/ball.mass;
	if(ball.speed_y>0)
		ay=(ball.mass*world.gravity-world.air_drop*ball.size*ball.speed_y)/ball.mass;
	if(ball.speed_y==0)
		ay=(ball.mass*world.gravity)/ball.mass;
	if(ball.speed_y<0)
		ay=(ball.mass*world.gravity+std::abs(world.air_drop*ball.size*ball.speed_y))/ball.mass;
	ball.posit_x+=ball.speed_x*world.delta_time+0.5*ax*std::pow(world.delta_time,2);
	ball.posit_y+=ball.speed_y*world.delta_time+0.5*ay*std::pow(world.delta_time,2);
	ball.speed_x+=ax*world.delta_time;
	ball.speed_y+=ay*world.delta_time;
}

int main()
{
	using namespace darwin;
	runtime.load("./darwin.module");
	auto adapter=runtime.get_adapter();
	auto pic=runtime.get_drawable();
	sync_clock clock(30);
	while(true) {
		clock.reset();
		runtime.fit_drawable();
		pic->clear();
		if(ball.posit_x<0||ball.posit_x>pic->get_width()-1) ball.speed_x=-ball.speed_x;
		if(ball.posit_y<0||ball.posit_y>pic->get_height()-1) ball.speed_y=-ball.speed_y;
		run();
		pic->draw_pixel(ball.posit_x,ball.posit_y,pixel('@', true,false, colors::blue,colors::white));
		runtime.update_drawable();
		clock.sync();
	}
	return 0;
}