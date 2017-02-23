#define DARWIN_FORCE_BUILTIN
#include "../headers/darwin.hpp"
#include <string>
#include <random>
#include <array>
#include <deque>
#include <ctime>
using position_type=std::array<int,2>;
darwin::pixel head_pix('#',true,false,darwin::colors::white,darwin::colors::black);
darwin::pixel body_pix('+',true,false,darwin::colors::white,darwin::colors::black);
darwin::pixel food_pix('@',true,false,darwin::colors::white,darwin::colors::black);
std::deque<position_type> snake_body;
position_type snake_head= {0,0};
position_type food= {0,0};
int heading=2;
/*
1=left
2=right
-1=up
-2=down
*/
int init_long=5;
bool god_mode=false;
bool cross_wall=false;
int hard=3;
int score=0;

void die()
{
	auto pic=darwin::runtime.get_drawable();
	darwin::sync_clock clock(30);
	std::string str0="You die!Your score is "+std::to_string(score);
	std::string str1="Press any key to continue...";
	while(true) {
		clock.reset();
		if(darwin::runtime.is_kb_hit()) {
			darwin::runtime.get_kb_hit();
			break;
		}
		pic->clear();
		pic->fill(darwin::pixel(' ',true,false,darwin::colors::white,darwin::colors::red));
		pic->draw_string(0.5*(pic->get_width()-str0.size()),0.5*pic->get_height(),str0,darwin::pixel(' ',true,false,darwin::colors::white,darwin::colors::red));
		pic->draw_string(0.5*(pic->get_width()-str1.size()),pic->get_height()-1,str1,darwin::pixel(' ',true,false,darwin::colors::white,darwin::colors::red));
		darwin::runtime.update_drawable();
		clock.sync();
	}
}
void gen_food()
{
	auto pic=darwin::runtime.get_drawable();
	bool is_fit=true;
	while(is_fit) {
		food[0]=darwin::rand<int>(0,pic->get_width()-1);
		food[1]=darwin::rand<int>(0,pic->get_height()-1);
		is_fit=false;
		for(auto& p:snake_body) {
			if(p[0]==food[0]&&p[1]==food[1])
				is_fit=true;
		}
	}
}
void start()
{
	auto pic=darwin::runtime.get_drawable();
	darwin::sync_clock clock(30);
	snake_head= {init_long,0.5*pic->get_height()};
	for(int i=0; i<init_long-1; ++i)
		snake_body.push_back({snake_head[0]-i,snake_head[1]});
	gen_food();
	int frame=0;
	while(true) {
		clock.reset();
		if(darwin::runtime.is_kb_hit()) {
			switch(darwin::runtime.get_kb_hit()) {
			case 'w':
				if(god_mode||heading!=-2)
					heading=-1;
				break;
			case 's':
				if(god_mode||heading!=-1)
					heading=-2;
				break;
			case 'a':
				if(god_mode||heading!=2)
					heading=1;
				break;
			case 'd':
				if(god_mode||heading!=1)
					heading=2;
				break;
			}
		}
		if(frame==hard) {
			switch(heading) {
			case 1:
				--snake_head[0];
				break;
			case 2:
				++snake_head[0];
				break;
			case -1:
				--snake_head[1];
				break;
			case -2:
				++snake_head[1];
				break;
			}
			if(cross_wall) {
				if(snake_head[0]<0)
					snake_head[0]+=pic->get_width()-1;
				else if(snake_head[0]>pic->get_width()-1)
					snake_head[0]-=pic->get_width()-1;
				else if(snake_head[1]<0)
					snake_head[1]+=pic->get_height()-1;
				else if(snake_head[1]>pic->get_height()-1)
					snake_head[1]-=pic->get_height()-1;
			} else if(snake_head[0]<0||snake_head[0]>pic->get_width()-1||snake_head[1]<0||snake_head[1]>pic->get_height()-1) {
				die();
				return;
			}
			if(!god_mode) {
				for(auto& p:snake_body) {
					if(p[0]==snake_head[0]&&p[1]==snake_head[1]) {
						die();
						return;
					}
				}
			}
			snake_body.push_front(snake_head);
			if(snake_head[0]==food[0]&&snake_head[1]==food[1]) {
				gen_food();
				score+=10*(1.0/hard);
			} else
				snake_body.pop_back();
			frame=0;
		}
		pic->clear();
		for(auto& p:snake_body)
			pic->draw_pixel(p[0],p[1],body_pix);
		pic->draw_pixel(snake_head[0],snake_head[1],head_pix);
		pic->draw_pixel(food[0],food[1],food_pix);
		darwin::runtime.update_drawable();
		++frame;
		clock.sync();
	}
}
int main()
{
	darwin::timer::delay(1000);
	darwin::runtime.load("./darwin.module");
	darwin::runtime.fit_drawable();
	while(true) {
		snake_body.clear();
		heading=2;
		start();
	}
	return 0;
}