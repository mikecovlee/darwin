#define DARWIN_FORCE_BUILTIN
#include "../headers/darwin.hpp"
#include <string>
int main()
{
	darwin::runtime.load("./darwin.module");
	auto pic=darwin::runtime.get_drawable();
	std::string greeting="Hello,World!";
	darwin::pixel p(' ', true,false, darwin::colors::red,darwin::colors::white);
	darwin::sync_clock clock(30);
	while(true) {
		clock.reset();
		darwin::runtime.fit_drawable();
		p.set_char(' ');
		pic->fill(p);
		for(std::size_t x=0; x<greeting.size()&&x<pic->get_width(); ++x) {
			p.set_char(greeting[x]);
			pic->draw_pixel(x,0,p);
		}
		darwin::runtime.update_drawable();
		clock.sync();
	}
	return 0;
}