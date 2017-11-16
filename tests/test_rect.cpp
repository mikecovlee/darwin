#define DARWIN_FORCE_BUILTIN
#include "../darwin/darwin.hpp"
#include <iostream>
#include <string>
int main()
{
	std::string str;
	std::getline(std::cin,str);
	darwin::runtime.load("./darwin.module");
	auto pic=darwin::runtime.get_drawable();
	darwin::pixel pix(' ',true,false,darwin::colors::black,darwin::colors::white);
	darwin::sync_clock clock(60);
	while(true) {
		clock.reset();
		darwin::runtime.fit_drawable();
		pic->draw_rect(1,1,pic->get_width()-2,pic->get_height()-2,pix);
		pic->fill_rect(2,2,pic->get_width()-4,pic->get_height()-4,darwin::pixel(' ',true,false,darwin::colors::black,darwin::colors::cyan));
		pic->draw_string(0.5*(pic->get_width()-str.size()),0.5*pic->get_height(),str,pix);
		darwin::runtime.update_drawable();
		clock.sync();
	}
	darwin::runtime.exit();
	return 0;
}
