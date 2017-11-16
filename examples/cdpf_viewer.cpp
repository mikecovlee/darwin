#define DARWIN_FORCE_BUILTIN
#define DARWIN_DISABLE_LOG
#include "../darwin/darwin.hpp"
#include <fstream>
#include <cstdio>
int main(int arg_size,char** arg_array)
{
	if(arg_size<2) {
		printf("Wrong size of arguments.\n");
		return -1;
	}
	using namespace darwin;
	std::ifstream infs(arg_array[1]);
	if(!infs) {
		printf("File open error.\n");
		return -1;
	}
	std::deque<char> buff;
	for(char c; infs.get(c); buff.push_back(c));
	picture pic;
	runtime.load("./darwin.module");
	unserial_picture(&pic,buff);
	std::string head="Covariant Darwin Picture File Viewer 1.0";
	std::string help="Q:Exit W:Up S:Down A:Left D:Right";
	std::string info="File:\""+std::string(arg_array[1])+"\" Width:"+std::to_string(pic.get_width())+"pix Height:"+std::to_string(pic.get_height())+"pix";
	auto dpic=runtime.get_drawable();
	int x(0),y(0);
	bool running=true;
	sync_clock clock(30);
	while(running) {
		if(runtime.is_kb_hit()) {
			switch(runtime.get_kb_hit()) {
			case 'w':
				--y;
				break;
			case 's':
				++y;
				break;
			case 'a':
				--x;
				break;
			case 'd':
				++x;
				break;
			case 'q':
				running=false;
				break;
			}
		}
		clock.reset();
		runtime.fit_drawable();
		dpic->fill(pixel(' ',true,false,colors::white,colors::white));
		dpic->draw_picture((dpic->get_width()-pic.get_width())/2+x,(dpic->get_height()-pic.get_height())/2+y,pic);
		dpic->draw_line(0,0,dpic->get_width()-1,0,pixel(' ',true,false,colors::blue,colors::blue));
		dpic->draw_line(0,1,dpic->get_width()-1,1,pixel(' ',true,false,colors::cyan,colors::cyan));
		dpic->draw_line(0,dpic->get_height()-1,dpic->get_width()-1,dpic->get_height()-1,pixel(' ',true,false,colors::blue,colors::blue));
		dpic->draw_string(0,0,head,pixel(' ',true,false,colors::white,colors::blue));
		dpic->draw_string(0,1,help,pixel(' ',true,false,colors::white,colors::cyan));
		dpic->draw_string(0,dpic->get_height()-1,info,pixel(' ',true,false,colors::white,colors::blue));
		runtime.update_drawable();
		clock.sync();
	}
	runtime.exit();
	return 0;
}
