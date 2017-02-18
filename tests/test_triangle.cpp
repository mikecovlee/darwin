#define DARWIN_FORCE_BUILTIN
#define DARWIN_DISABLE_LOG
#include "../headers/darwin.hpp"
int main()
{
	darwin::runtime.load("./darwin.module");
	auto pic=darwin::runtime.get_drawable();
	darwin::runtime.fit_drawable();
	darwin::sync_clock clock(30);
	std::deque<std::array<int,2>> points= {{0,0},{int(pic->get_width()-1),0},{int(0.5*pic->get_width()),int(0.5*pic->get_height())}};
	int focus=2;
	while(true) {
		if(darwin::runtime.is_kb_hit()) {
			switch(darwin::runtime.get_kb_hit()) {
			case 'w':
				--points[focus][1];
				break;
			case 's':
				++points[focus][1];
				break;
			case 'a':
				--points[focus][0];
				break;
			case 'd':
				++points[focus][0];
				break;
			case '1':
				focus=0;
				break;
			case '2':
				focus=1;
				break;
			case '3':
				focus=2;
				break;
			}
		}
		clock.reset();
		darwin::runtime.fit_drawable();
		pic->clear();
		pic->fill_triangle(points[0][0],points[0][1],points[1][0],points[1][1],points[2][0],points[2][1],darwin::pixel(' ',true,false,darwin::colors::black,darwin::colors::white));
		pic->draw_pixel(points[focus][0],points[focus][1],darwin::pixel('#',true,false,darwin::colors::black,darwin::colors::white));
		darwin::runtime.update_drawable();
		clock.sync();
	}
	return 0;
}
