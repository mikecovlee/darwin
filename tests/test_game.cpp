#include "../headers/darwin.hpp"
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

int bw=20;
int fj=3;

int main()
{
	using namespace darwin;
	runtime.load("./darwin.module");
	auto adapter=runtime.get_adapter();
	auto pic=adapter->get_drawable();
	sync_clock clock(60);
	std::size_t x(0),y(0);
	long bx(0),by(0);
	bool x_add(true),y_add(true);
	int fc=0;
	while(true) {
		++fc;
		if(KbHit()) {
			switch(GetKbHit()) {
			case '2':
				--by;
				break;
			case '8':
				++by;
				break;
			case '4':
				--bx;
				break;
			case '6':
				++bx;
				break;
			}
		}
		clock.reset();
		adapter->fit_drawable();
		std::size_t cx((pic->get_width()-bw)*0.5+bx),cy(pic->get_height()*0.5-1+by);
		pic->clear();
		if(fc==fj) {
			if(x_add) ++x;
			else --x;
			if(y_add) ++y;
			else --y;
		}
		pic->draw_pixel({x,y},pixel(' ', {true,false}, {colors::blue,colors::white}));
		if(fc==fj) {
			if(x==0) x_add=true;
			if(y==0) y_add=true;
			if(x==pic->get_width()-1) x_add=false;
			if(y==pic->get_height()-1) y_add=false;
			if((x==cx-1 || x==cx+bw)&&y==cx)
				x_add=x_add?false:true;
			else if(cx<=x&&x<=cx+bw-1&&(y==cy-1 || y==cy+1))
				y_add=y_add?false:true;
			else if(x==cx-1&&y==cy-1&&x_add&&y_add) {
				x_add=false;
				y_add=false;
			} else if(x==cx-1&&y==cy+1&&x_add&&!y_add) {
				x_add=false;
				y_add=true;
			} else if(x==cx+bw&&y==cy-1&&!x_add&&y_add) {
				x_add=true;
				y_add=false;
			} else if(x==cx+bw&&y==cy+1&&!x_add&&!y_add) {
				x_add=true;
				y_add=true;
			}
			fc=0;
		}
		pic->draw_line({cx,cy}, {cx+bw,cy},pixel('#', {true,false}, {colors::white,colors::white}));
		adapter->update_drawable();
		clock.sync();
	}
	return 0;
}