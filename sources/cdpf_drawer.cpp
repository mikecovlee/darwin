#define DARWIN_FORCE_BUILTIN
#define DARWIN_DISABLE_LOG
#include "../headers/darwin.hpp"
#include <string>
using namespace darwin;
class cdpf_drawer final {
	picture mPic;
public:
	cdpf_drawer()=default;
	cdpf_drawer(const cdpf_drawer&)=delete;
	~cdpf_drawer()=default;
	void about()
	{

	}
	void open_exsist()
	{

	}
	void new_picture()
	{
		auto pic=runtime.get_drawable();
		sync_clock clock(60);
		std::string buff0,buff1;
		std::string but0("Default"),but1("OK");
		int select=0;
		int limit=5;
		bool run=true;
		while(run) {
			clock.reset();
			if(runtime.is_kb_hit()) {
				char ch=runtime.get_kb_hit();
				switch(ch) {
				case 's':
					if(select<3)
						++select;
					break;
				case 'w':
					if(select>0)
						--select;
					break;
				case 127:
					switch(select) {
					case 0:
						if(!buff0.empty())
							buff0.pop_back();
						break;
					case 1:
						if(!buff1.empty())
							buff1.pop_back();
						break;
					}
					break;
				case '\t':
					if(select<3)
						++select;
					else
						select=0;
					break;
				case '\n':
					switch(select) {
					default:
						if(select<2)
							++select;
						break;
					case 2:
						buff0=std::to_string(pic->get_width()-2);
						buff1=std::to_string(pic->get_height()-2);
						break;
					case 3:
						mPic.resize(std::stoi(buff0),std::stoi(buff1));
						run=false;
						break;
					}
					break;
				default:
					switch(select) {
					case 0:
						if(ch>47&&ch<58&&buff0.size()<limit)
							buff0+=ch;
						break;
					case 1:
						if(ch>47&&ch<58&&buff1.size()<limit)
							buff1+=ch;
						break;
					default:
						mPic.resize(std::stoi(buff0),std::stoi(buff1));
						run=false;
						break;
					}
					break;
				}
			}
			runtime.fit_drawable();
			pic->clear();
			pic->draw_rect(0.25*pic->get_width(),0.25*pic->get_height(),0.5*pic->get_width(),0.5*pic->get_height(),pixel(' ',true,false,colors::black,colors::blue));
			pic->fill_rect(0.25*pic->get_width()+1,0.25*pic->get_height()+1,0.5*pic->get_width()-2,0.5*pic->get_height()-2,pixel(' ',true,false,colors::black,colors::cyan));
			pic->draw_string(0.25*pic->get_width()+1,0.25*pic->get_height(),"CDPF Drawer v1.0-New Picture",pixel(' ',true,false,colors::white,colors::blue));
			pic->draw_string(0.25*pic->get_width()+4,0.25*pic->get_height()+2,"Picture Width:",pixel(' ',true,false,colors::white,colors::cyan));
			pic->draw_string(0.25*pic->get_width()+4,0.25*pic->get_height()+4,"Picture Height:",pixel(' ',true,false,colors::white,colors::cyan));
			pic->draw_line(0.25*pic->get_width()+4,0.25*pic->get_height()+3,0.25*pic->get_width()+4+limit,0.25*pic->get_height()+3,pixel(' ',true,false,colors::white,colors::white));
			pic->draw_line(0.25*pic->get_width()+4,0.25*pic->get_height()+5,0.25*pic->get_width()+4+limit,0.25*pic->get_height()+5,pixel(' ',true,false,colors::white,colors::white));
			pic->draw_string(0.25*pic->get_width()+4,0.25*pic->get_height()+3,buff0,pixel(' ',true,false,colors::black,colors::white));
			pic->draw_string(0.25*pic->get_width()+4,0.25*pic->get_height()+5,buff1,pixel(' ',true,false,colors::black,colors::white));
			if(select==2)
				pic->draw_rect(0.25*pic->get_width()+3,0.75*pic->get_height()-5,but0.size()+2,3,pixel('#',true,false,colors::blue,colors::blue));
			else
				pic->draw_rect(0.25*pic->get_width()+3,0.75*pic->get_height()-5,but0.size()+2,3,pixel(' ',true,false,colors::blue,colors::blue));
			pic->draw_string(0.25*pic->get_width()+4,0.75*pic->get_height()-4,but0,pixel(' ',true,false,colors::white,colors::blue));
			if(select==3)
				pic->draw_rect(0.25*pic->get_width()+3+but0.size()+4,0.75*pic->get_height()-5,but1.size()+2,3,pixel('#',true,false,colors::blue,colors::blue));
			else
				pic->draw_rect(0.25*pic->get_width()+3+but0.size()+4,0.75*pic->get_height()-5,but1.size()+2,3,pixel(' ',true,false,colors::blue,colors::blue));
			pic->draw_string(0.25*pic->get_width()+4+but0.size()+4,0.75*pic->get_height()-4,but1,pixel(' ',true,false,colors::white,colors::blue));
			if(select<2)
				pic->draw_string(0.25*pic->get_width()+2,0.25*pic->get_height()+3+2*select,"->",pixel(' ',true,false,colors::white,colors::cyan));
			runtime.update_drawable();
			clock.sync();
		}
	}
	void start()
	{
		auto pic=runtime.get_drawable();
		sync_clock clock(60);
		int select=0;
		bool run=true;
		while(run) {
			clock.reset();
			if(runtime.is_kb_hit()) {
				switch(runtime.get_kb_hit()) {
				case '\t':
					if(select<3)
						++select;
					else
						select=0;
					break;
				case 's':
					if(select<3)
						++select;
					break;
				case 'w':
					if(select>0)
						--select;
					break;
				case '\n':
					switch(select) {
					case 0:
						new_picture();
						break;
					case 1:
						open_exsist();
						break;
					case 2:
						about();
						break;
					case 3:
						runtime.exit(0);
						break;
					}
					run=false;
					break;
				}
			}
			runtime.fit_drawable();
			pic->clear();
			pic->draw_rect(0.25*pic->get_width(),0.25*pic->get_height(),0.5*pic->get_width(),0.5*pic->get_height(),pixel(' ',true,false,colors::black,colors::blue));
			pic->fill_rect(0.25*pic->get_width()+1,0.25*pic->get_height()+1,0.5*pic->get_width()-2,0.5*pic->get_height()-2,pixel(' ',true,false,colors::black,colors::cyan));
			pic->draw_string(0.25*pic->get_width()+1,0.25*pic->get_height(),"CDPF Drawer v1.0-Welcome",pixel(' ',true,false,colors::white,colors::blue));
			pic->draw_string(0.25*pic->get_width()+4,0.5*(pic->get_height()-6),"Getting start",pixel(' ',true,false,colors::white,colors::red));
			pic->draw_string(0.25*pic->get_width()+4,0.5*(pic->get_height()-6)+2,"New Picture",pixel(' ',true,false,colors::white,colors::cyan));
			pic->draw_string(0.25*pic->get_width()+4,0.5*(pic->get_height()-6)+3,"Open Exsist Picture",pixel(' ',true,false,colors::white,colors::cyan));
			pic->draw_string(0.25*pic->get_width()+4,0.5*(pic->get_height()-6)+4,"About This Software",pixel(' ',true,false,colors::white,colors::cyan));
			pic->draw_string(0.25*pic->get_width()+4,0.5*(pic->get_height()-6)+5,"Exit",pixel(' ',true,false,colors::white,colors::cyan));
			pic->draw_string(0.25*pic->get_width()+2,0.5*(pic->get_height()-6)+2+select,"->",pixel(' ',true,false,colors::white,colors::cyan));
			runtime.update_drawable();
			clock.sync();
		}
	}
	void main()
	{
		auto pic=runtime.get_drawable();
		sync_clock clock(60);
		int mode=0;

		bool fmod=false;
		std::deque<std::array<int,2>> vertex;
		bool run=true;
		mPic.fill(pixel(' ',true,false,colors::black,colors::white));
		int cx(0),cy(0);
		pixel pix('@',true,false,colors::black,colors::white);
		while(run) {
			clock.reset();
			if(runtime.is_kb_hit()) {
				switch(runtime.get_kb_hit()) {
				case 's':
					++cy;
					break;
				case 'w':
					--cy;
					break;
				case 'd':
					++cx;
					break;
				case 'a':
					--cx;
					break;
				case 'q':
					mPic.draw_pixel(0.5*mPic.get_width()+cx+2,0.5*mPic.get_height()+cy,pix);
					break;
				case 'e':
					switch(mode) {
					case 0:
						if(fmod) {
							vertex.push_back({cx,cy});
							mode=11;
							fmod=false;
						} else
							mode=1;
						break;
					case 1:
						mode=0;
						break;
					case 11:
						mPic.draw_line(0.5*mPic.get_width()+2+vertex[0][0],0.5*mPic.get_height()+vertex[0][1],0.5*mPic.get_width()+cx+2,0.5*mPic.get_height()+cy,pix);
						vertex.clear();
						mode=0;
						break;
					}
					break;
				case 'z':
					mode=mode==0?2:0;
					break;
				case 'x':
					mode=mode==0?3:0;
					break;
				case 'c':
					mode=mode==0?4:0;
					break;
				case 'f':
					fmod=fmod?false:true;
					break;
				}
			}
			runtime.fit_drawable();
			switch(mode) {
			case 1:
				mPic.draw_pixel(0.5*mPic.get_width()+cx+2,0.5*mPic.get_height()+cy,pix);
				break;
			}
			pic->clear();
			pic->draw_picture(0.5*(pic->get_width()-mPic.get_width()),0.5*(pic->get_height()-mPic.get_height()),mPic);
			switch(mode) {
			case 2:
				pic->draw_rect(0.25*pic->get_width(),0.25*pic->get_height(),0.5*pic->get_width(),0.5*pic->get_height(),pixel(' ',true,false,colors::black,colors::blue));
				pic->fill_rect(0.25*pic->get_width()+1,0.25*pic->get_height()+1,0.5*pic->get_width()-2,0.5*pic->get_height()-2,pixel(' ',true,false,colors::black,colors::cyan));
				pic->draw_string(0.25*pic->get_width()+1,0.25*pic->get_height(),"CDPF Drawer v1.0-Select Front Color",pixel(' ',true,false,colors::white,colors::blue));
				pic->draw_string(0.25*pic->get_width()+4,0.5*pic->get_height(),"Please Select The Front Color(Key 1~8)",pixel(' ',true,false,colors::white,colors::cyan));
				break;
			case 3:
				pic->draw_rect(0.25*pic->get_width(),0.25*pic->get_height(),0.5*pic->get_width(),0.5*pic->get_height(),pixel(' ',true,false,colors::black,colors::blue));
				pic->fill_rect(0.25*pic->get_width()+1,0.25*pic->get_height()+1,0.5*pic->get_width()-2,0.5*pic->get_height()-2,pixel(' ',true,false,colors::black,colors::cyan));
				pic->draw_string(0.25*pic->get_width()+1,0.25*pic->get_height(),"CDPF Drawer v1.0-Select Front Color",pixel(' ',true,false,colors::white,colors::blue));
				pic->draw_string(0.25*pic->get_width()+4,0.5*pic->get_height(),"Please Select The Back Color(Key 1~8)",pixel(' ',true,false,colors::white,colors::cyan));
				break;
			case 4:
				pic->draw_rect(0.25*pic->get_width(),0.25*pic->get_height(),0.5*pic->get_width(),0.5*pic->get_height(),pixel(' ',true,false,colors::black,colors::blue));
				pic->fill_rect(0.25*pic->get_width()+1,0.25*pic->get_height()+1,0.5*pic->get_width()-2,0.5*pic->get_height()-2,pixel(' ',true,false,colors::black,colors::cyan));
				pic->draw_string(0.25*pic->get_width()+1,0.25*pic->get_height(),"CDPF Drawer v1.0-Select Front Color",pixel(' ',true,false,colors::white,colors::blue));
				pic->draw_string(0.25*pic->get_width()+4,0.5*pic->get_height(),"Please Select The Character",pixel(' ',true,false,colors::white,colors::cyan));
				break;
			case 11:
				pic->draw_line(0.5*pic->get_width()+2+vertex[0][0],0.5*pic->get_height()+vertex[0][1],0.5*pic->get_width()+cx+2,0.5*pic->get_height()+cy,pix);
				break;
			}
			pic->draw_rect(0,0,pic->get_width(),pic->get_height(),pixel(' ',true,false,colors::black,colors::blue));
			pic->draw_string(1,0,"CDPF Drawer v1.0",pixel(' ',true,false,colors::white,colors::blue));
			if(fmod)
				pic->draw_string(1,pic->get_height()-1,"Function Mode",pixel(' ',true,false,colors::white,colors::blue));
			pic->draw_string(0.5*pic->get_width()+cx,0.5*pic->get_height()+cy,"->",pixel(' ',true,false,colors::white,colors::black));
			pic->draw_pixel(0.5*pic->get_width()+cx+2,0.5*pic->get_height()+cy,pix);
			runtime.update_drawable();
			clock.sync();
		}
	}
};
int main()
{
	runtime.load("./darwin.module");
	cdpf_drawer main_prog;
	main_prog.start();
	main_prog.main();
	runtime.exit(0);
	return 0;
}
