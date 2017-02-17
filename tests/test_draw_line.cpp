#define DARWIN_FORCE_BUILTIN
#include "../headers/darwin.hpp"
int main()
{
	darwin::runtime.load("./darwin.module");
	auto pic=darwin::runtime.get_drawable();
	darwin::sync_clock clock(30);
	int x0(0),y0(0),x1(1),y1(1);
	while(true) {
		if(darwin::runtime.is_kb_hit()) {
			switch(darwin::runtime.get_kb_hit()) {
			case 'w':
				--y0;
				break;
			case 's':
				++y0;
				break;
			case 'a':
				--x0;
				break;
			case 'd':
				++x0;
				break;
			case 'i':
				--y1;
				break;
			case 'k':
				++y1;
				break;
			case 'j':
				--x1;
				break;
			case 'l':
				++x1;
				break;
			case 'c':
				darwin::print_screen();
				break;
			}
		}
		clock.reset();
		darwin::runtime.fit_drawable();
		pic->clear();
		pic->draw_line(x0,y0,x1,y1,darwin::pixel('@',true,false,darwin::colors::blue,darwin::colors::white));
		darwin::runtime.update_drawable();
		clock.sync();
	}
	return 0;
}