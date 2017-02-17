#define DARWIN_FORCE_BUILTIN
#include "../headers/darwin.hpp"
int main()
{
	darwin::runtime.load("./darwin.module");
	auto pic=darwin::runtime.get_drawable();
	darwin::sync_clock clock(30);
	int x0(0),y0(0),x1(0),y1(4),x2(4),y2(4);
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
			case 'g':
				--y2;
				break;
			case 'v':
				++y2;
				break;
			case 'c':
				--x2;
				break;
			case 'b':
				++x2;
				break;
			}
		}
		clock.reset();
		darwin::runtime.fit_drawable();
		pic->clear();
		pic->fill_triangle(x0,y0,x1,y1,x2,y2,darwin::pixel('#',true,false,darwin::colors::white,darwin::colors::white));
		darwin::runtime.update_drawable();
		clock.sync();
	}
	return 0;
}