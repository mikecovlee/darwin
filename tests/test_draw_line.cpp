#define DARWIN_FORCE_BUILTIN
#include "../headers/darwin.hpp"
#include <unistd.h>
bool KbHit()
{
	bool ret;
	fd_set fds;
	struct timeval tv;
	tv.tv_sec = 0;
	tv.tv_usec = 0;
	FD_ZERO(&fds);
	FD_SET(0, &fds);
	select(1, &fds, 0, 0, &tv);
	ret = FD_ISSET(0, &fds);
	return ret;
}
int GetKbHit()
{
	int ret;
	ret = getchar();
	return ret;
}
int main()
{
	darwin::runtime.load("./darwin.module");
	auto adapter=darwin::runtime.get_adapter();
	auto pic=adapter->get_drawable();
	darwin::sync_clock clock(30);
	std::size_t x0(0),y0(0),x1(1),y1(1);
	while(true) {
		if(KbHit()) {
			switch(GetKbHit()) {
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
			}
		}
		clock.reset();
		adapter->fit_drawable();
		pic->clear();
		pic->draw_line({x0,y0}, {x1,y1},darwin::pixel('@', {true,false}, {darwin::colors::blue,darwin::colors::white}));
		adapter->update_drawable();
		clock.sync();
	}
	return 0;
}