#pragma once
/*
* Covariant Darwin Universal Character Graphics Library
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*
* Copyright (C) 2017 Michael Lee(李登淳)
* Email: China-LDC@outlook.com
* Github: https://github.com/mikecovlee
* Website: http://ldc.atd3.cn
*
* Library Version: 17.2.0
*
* Marco List:
* Library Version: __Darwin
*
* See MarcoList.md to learn more about Covariant Darwin UCGL Marcos.
* See Reference.md to learn more about Covariant Darwin UCGL.
*/

#ifndef __cplusplus
#error Darwin UCGL need C++ Compiler
#endif

#if __cplusplus < 201103L
#error Darwin UCGL need C++11 or later standard.
#endif

#define __Darwin 170200L

#include <functional>
#include <exception>
#include <typeindex>
#include <typeinfo>
#include <cstdlib>
#include <cstddef>
#include <fstream>
#include <chrono>
#include <memory>
#include <thread>
#include <cstdio>
#include <string>
#include <array>
#include <cmath>
#include <deque>
#include <csignal>
#include <dlfcn.h>
#include <unistd.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <sys/select.h>

namespace darwin {
	typedef unsigned long timer_t;
	class timer;
}
class darwin::timer final {
	static std::chrono::time_point < std::chrono::high_resolution_clock > m_timer;
public:
	enum class time_unit {
		nano_sec, micro_sec, milli_sec, second, minute
	};
	static void reset()
	{
		m_timer = std::chrono::high_resolution_clock::now();
	}
	static timer_t time(time_unit unit=time_unit::milli_sec)
	{
		switch (unit) {
		case time_unit::nano_sec:
			return std::chrono::duration_cast < std::chrono::nanoseconds >(std::chrono::high_resolution_clock::now() - m_timer).count();
		case time_unit::micro_sec:
			return std::chrono::duration_cast < std::chrono::microseconds >(std::chrono::high_resolution_clock::now() - m_timer).count();
		case time_unit::milli_sec:
			return std::chrono::duration_cast < std::chrono::milliseconds >(std::chrono::high_resolution_clock::now() - m_timer).count();
		case time_unit::second:
			return std::chrono::duration_cast < std::chrono::seconds >(std::chrono::high_resolution_clock::now() - m_timer).count();
		case time_unit::minute:
			return std::chrono::duration_cast < std::chrono::minutes >(std::chrono::high_resolution_clock::now() - m_timer).count();
		}
		return 0;
	}
	static void delay(timer_t time, time_unit unit=time_unit::milli_sec)
	{
		switch (unit) {
		case time_unit::nano_sec:
			std::this_thread::sleep_for(std::chrono::nanoseconds(time));
			break;
		case time_unit::micro_sec:
			std::this_thread::sleep_for(std::chrono::microseconds(time));
			break;
		case time_unit::milli_sec:
			std::this_thread::sleep_for(std::chrono::milliseconds(time));
			break;
		case time_unit::second:
			std::this_thread::sleep_for(std::chrono::seconds(time));
			break;
		case time_unit::minute:
			std::this_thread::sleep_for(std::chrono::minutes(time));
			break;
		}
	}
	static timer_t measure(const std::function<void()>& func, time_unit unit=time_unit::milli_sec)
	{
		timer_t begin(0),end(0);
		begin=time(unit);
		func();
		end=time(unit);
		return end-begin;
	}
};
std::chrono::time_point < std::chrono::high_resolution_clock > darwin::timer::m_timer(std::chrono::high_resolution_clock::now());

namespace darwin {
	class outfs final {
		FILE* mfp=nullptr;
	public:
		outfs()=default;
		outfs(const char* path):mfp(fopen(path,"w")) {}
		outfs(const std::string& path):mfp(fopen(path.c_str(),"w")) {}
		~outfs()
		{
			if(this->mfp!=nullptr) fclose(this->mfp);
		}
		outfs& operator=(const outfs&)=delete;
		operator bool() const noexcept
		{
			return this->mfp!=nullptr;
		}
		bool usable() const noexcept
		{
			return this->mfp!=nullptr;
		}
		void open(const char* path)
		{
			if(this->mfp==nullptr) {
				this->mfp=fopen(path,"w");
			}
		}
		void open(const std::string& path)
		{
			if(this->mfp==nullptr) {
				this->mfp=fopen(path.c_str(),"w");
			}
		}
		void close()
		{
			if(this->mfp!=nullptr)
				fclose(this->mfp);
			this->mfp=nullptr;
		}
		template<typename...ArgsT> void printf(const char* format,ArgsT...args)
		{
			fprintf(this->mfp,format,args...);
		}
		void flush()
		{
			if(this->mfp!=nullptr) {
				fflush(this->mfp);
			}
		}
	};
#ifndef DARWIN_DISABLE_LOG
	class debugger final {
		static outfs out;
	public:
#ifdef DARWIN_STRICT_CHECK
		static constexpr bool strict=true;
#else
		static constexpr bool strict=false;
#endif
#ifdef DARWIN_IGNORE_WARNING
		static constexpr bool ignore=true;
#else
		static constexpr bool ignore=false;
#endif
		static void log(const char* file,int line,const char* func,const char* msg)
		{
			out.printf("[Darwin Log(%ums)]:In function \"%s\"(%s:%d):%s\n",timer::time(),func,file,line,msg);
		}
		static void warning(const char* file,int line,const char* func,const char* msg)
		{
			if(ignore)
				return;
			out.printf("[Darwin Warning(%ums)]:In function \"%s\"(%s:%d):%s\n",timer::time(),func,file,line,msg);
			if(strict) {
				out.flush();
				std::terminate();
			}
		}
		static void error(const char* file,int line,const char* func,const char* msg)
		{
			out.printf("[Darwin Error(%ums)]:In function \"%s\"(%s:%d):%s\n",timer::time(),func,file,line,msg);
			out.flush();
			std::terminate();
		}
		static void log_path(const char* path)
		{
			out.close();
			out.open(path);
		}
	};
	outfs debugger::out("./darwin_runtime.log");
}
#define Darwin_Log(msg) darwin::debugger::log(__FILE__,__LINE__,__func__,msg);
#define Darwin_Warning(msg) darwin::debugger::warning(__FILE__,__LINE__,__func__,msg);
#define Darwin_Error(msg) darwin::debugger::error(__FILE__,__LINE__,__func__,msg);
#define Darwin_Set_Log_Path(path) darwin::debugger::log_path(path);
#else
}
#define Darwin_Log(msg)
#define Darwin_Warning(msg)
#define Darwin_Error(msg) std::terminate();
#define Darwin_Set_Log_Path(path)
#endif

namespace darwin {
	enum class status {
		null,ready,busy,leisure,error
	};
	enum class results {
		null,success,failure
	};
	enum class colors {
		white,black,red,green,blue,pink,yellow,cyan
	};
	enum class attris {
		bright,underline
	};
	enum class commands {
		echo_on,echo_off,reset_cursor,reset_attri,clrscr
	};
	class pixel final {
		char mChar=' ';
		std::array<bool,2> mAttris= {{false,false}};
		std::array<colors,2> mColors= {{colors::white,colors::black}};
	public:
		pixel()=default;
		pixel(const pixel&)=default;
		pixel(char ch,bool bright,bool underline,colors fcolor,colors bcolor):mChar(ch),mAttris(
		{
			bright,underline
		}),mColors({fcolor,bcolor}) {}
		~pixel()=default;
		void set_char(char c)
		{
			mChar=c;
		}
		char get_char() const
		{
			return mChar;
		}
		void set_front_color(colors c)
		{
			mColors[0]=c;
		}
		colors get_front_color() const
		{
			return mColors[0];
		}
		void set_back_color(colors c)
		{
			mColors[1]=c;
		}
		colors get_back_color() const
		{
			return mColors[1];
		}
		void set_colors(const std::array<colors,2>& c)
		{
			mColors=c;
		}
		const std::array<colors,2>& get_colors() const
		{
			return mColors;
		}
		void set_bright(bool value)
		{
			mAttris[0]=value;
		}
		bool is_bright() const
		{
			return mAttris[0];
		}
		void set_underline(bool value)
		{
			mAttris[1]=value;
		}
		bool is_underline() const
		{
			return mAttris[1];
		}
		void set_attris(const std::array<bool,2>& a)
		{
			mAttris=a;
		}
		const std::array<bool,2>& get_attris() const
		{
			return mAttris;
		}
	};
	class drawable {
	public:
		static double draw_line_precision;
		drawable()=default;
		drawable(const drawable&)=default;
		virtual ~drawable()=default;
		virtual std::type_index get_type() const final
		{
			return typeid(*this);
		}
		virtual std::shared_ptr<drawable> clone() noexcept
		{
			return nullptr;
		}
		virtual bool usable() const noexcept=0;
		virtual void clear()=0;
		virtual void fill(const pixel&)=0;
		virtual void resize(std::size_t,std::size_t)=0;
		virtual std::size_t get_width() const=0;
		virtual std::size_t get_height() const=0;
		virtual const pixel& get_pixel(std::size_t,std::size_t) const=0;
		virtual void draw_pixel(int,int,const pixel&)=0;
		virtual void draw_line(int p0x,int p0y,int p1x,int p1y,const pixel& pix)
		{
			if(!this->usable())
				Darwin_Error("Use of not available object.");
			if(p0x<0||p0y<0||p1x<0||p1y<0||p0x>this->get_width()-1||p0y>this->get_height()-1||p1x>this->get_width()-1||p1y>this->get_height()-1)
				Darwin_Warning("Out of range.");
			long w(p1x-p0x),h(p1y-p0y);
			double distance(std::sqrt(std::pow(w,2)+std::pow(h,2))*draw_line_precision);
			for(double c=0; c<=1; c+=1.0/distance)
				this->draw_pixel(static_cast<int>(p0x+w*c),static_cast<int>(p0y+h*c),pix);
			this->draw_pixel(p0x,p0y,pix);
			this->draw_pixel(p1x,p1y,pix);
		}
		virtual void draw_rect(int x,int y,int w,int h,const pixel& pix)
		{
			if(!this->usable())
				Darwin_Error("Use of not available object.");
			if(x<0||y<0||x>this->get_width()-1||y>this->get_height()-1||x+w>this->get_width()||y+h>this->get_height())
				Darwin_Warning("Out of range.");
			for(int i=0; i<w; w>0?++i:--i) {
				this->draw_pixel(x+i,y,pix);
				this->draw_pixel(x+i,y+h-1,pix);
			}
			for(int i=1; i<h-1; h>0?++i:--i) {
				this->draw_pixel(x,y+i,pix);
				this->draw_pixel(x+w-1,y+i,pix);
			}
		}
		virtual void fill_rect(int x,int y,int w,int h,const pixel& pix)
		{
			if(!this->usable())
				Darwin_Error("Use of not available object.");
			if(x<0||y<0||x>this->get_width()-1||y>this->get_height()-1||x+w>this->get_width()||y+h>this->get_height())
				Darwin_Warning("Out of range.");
			for(int cy=0; cy<h; h>0?++cy:--cy)
				for(int cx=0; cx<w; w>0?++cx:--cx)
					this->draw_pixel(x+cx,y+cy,pix);
		}
		virtual void draw_triangle(int x1,int y1,int x2,int y2,int x3,int y3,const pixel& pix)
		{
			if(!this->usable())
				Darwin_Error("Use of not available object.");
			this->draw_line(x1,y1,x2,y2,pix);
			this->draw_line(x2,y2,x3,y3,pix);
			this->draw_line(x3,y3,x1,y1,pix);
		}
		virtual void fill_triangle(int x1,int y1,int x2,int y2,int x3,int y3,const pixel& pix)
		{
			if(!this->usable())
				Darwin_Error("Use of not available object.");
			int v1x(x2-x1),v1y(y2-y1),v2x(x3-x2),v2y(y3-y2);
			if(v1x*v2y-v2x*v1y==0)
				Darwin_Warning("Three points in a line.");
			if(y2<y1) {
				std::swap(y1,y2);
				std::swap(x1,x2);
			}
			if(y3<y2) {
				std::swap(y2,y3);
				std::swap(x2,x3);
			}
			if(y2<y1) {
				std::swap(y1,y2);
				std::swap(x1,x2);
			}
			if(y1==y2) {
				double k1(double(x3-x1)/double(y3-y1)),k2(double(x3-x2)/double(y3-y2));
				for(int y=0; y<=y3-y2; ++y)
					this->draw_line(x1+k1*y,y1+y,x2+k2*y,y2+y,pix);
			} else if(y2==y3) {
				double k1(double(x3-x1)/double(y3-y1)),k2(double(x2-x1)/double(y2-y1));
				for(int y=0; y<=y2-y1; ++y)
					this->draw_line(x1+k1*y,y1+y,x1+k2*y,y1+y,pix);
			} else {
				double k1(double(x3-x1)/double(y3-y1)),k2(double(x3-x2)/double(y3-y2)),k3(double(x2-x1)/double(y2-y1));
				for(int y=0; y<=y3-y1; ++y) {
					if(y<y2-y1)
						this->draw_line(x1+k1*y,y1+y,x1+k3*y,y1+y,pix);
					else
						this->draw_line(x1+k1*y,y1+y,x2+k2*(y-(y2-y1)),y1+y,pix);
				}
			}
		}
		virtual void draw_string(int x,int y,const std::string& str,const pixel& pix)
		{
			if(!this->usable())
				Darwin_Error("Use of not available object.");
			if(x<0||y<0||x>this->get_width()-1||y>this->get_height()-1||x+str.size()>this->get_width())
				Darwin_Warning("Out of range.");
			pixel p=pix;
			for(int i=0; i<str.size(); ++i) {
				p.set_char(str.at(i));
				this->draw_pixel(x+i,y,p);
			}
		}
		virtual void draw_picture(int col,int row,const drawable& pic)
		{
			if(!this->usable()||!pic.usable())
				Darwin_Error("Use of not available object.");
			if(col<0||row<0||col>this->get_width()-1||row>this->get_height()-1)
				Darwin_Warning("Out of range.");
			int y0(row>=0?row:0),y1(row>=0?0:-row);
			while(y0<this->get_height()&&y1<pic.get_height()) {
				int x0(col>=0?col:0),x1(col>=0?0:-col);
				while(x0<this->get_width()&&x1<pic.get_width()) {
					this->draw_pixel(x0,y0,pic.get_pixel(x1,y1));
					++x0;
					++x1;
				}
				++y0;
				++y1;
			}
		}
	};
	double drawable::draw_line_precision=1.5;
}

namespace darwin {
	class platform_adapter {
	public:
		platform_adapter()=default;
		platform_adapter(const platform_adapter&)=delete;
		virtual ~platform_adapter()=default;
		virtual status get_state() const noexcept=0;
		virtual results init() noexcept=0;
		virtual results stop() noexcept=0;
		virtual results exec_commands(commands) noexcept=0;
		virtual bool is_kb_hit() noexcept=0;
		virtual int get_kb_hit() noexcept=0;
		virtual results fit_drawable() noexcept=0;
		virtual drawable* get_drawable() noexcept=0;
		virtual results update_drawable() noexcept=0;
	};
	class module_adapter {
	public:
		module_adapter()=default;
		module_adapter(const module_adapter&)=delete;
		virtual ~module_adapter()=default;
		virtual status get_state() const noexcept=0;
		virtual results load_module(const std::string&) noexcept=0;
		virtual results free_module() noexcept=0;
		virtual platform_adapter* get_platform_adapter() noexcept=0;
	};
	typedef platform_adapter*(*module_enterance)();
	const char* module_enterance_name="COV_DARWIN_MODULE_MAIN";
}

namespace darwin {
	class picture:public drawable {
	protected:
		std::size_t mWidth,mHeight;
		pixel* mImage=nullptr;
		void copy(std::size_t w,std::size_t h,pixel* const img)
		{
			if(img!=nullptr) {
				delete[] this->mImage;
				this->mImage=new pixel[w*h];
				this->mWidth=w;
				this->mHeight=h;
				for(std::size_t i=0; i<this->mWidth*this->mHeight; ++i)
					this->mImage[i]=img[i];
			}
		}
	public:
		picture():mWidth(0),mHeight(0),mImage(nullptr) {}
		picture(std::size_t w,std::size_t h):mImage(new pixel[h*w]),mWidth(w),mHeight(h) {}
		picture(std::size_t w,std::size_t h,const pixel& pix):mImage(new pixel[h*w]),mWidth(w),mHeight(h)
		{
			for(pixel* it=this->mImage; it!=this->mImage+w*h; ++it) *it=pix;
		}
		picture(const picture& img):mWidth(0),mHeight(0),mImage(nullptr)
		{
			copy(img.mWidth,img.mHeight,img.mImage);
		}
		picture(picture&& img):mWidth(0),mHeight(0),mImage(nullptr)
		{
			copy(img.mWidth,img.mHeight,img.mImage);
		}
		virtual ~picture()
		{
			delete[] this->mImage;
		}
		picture& operator=(const picture& img)
		{
			if(&img!=this)
				this->copy(img.mWidth,img.mHeight,img.mImage);
			return *this;
		}
		picture& operator=(picture&& img)
		{
			if(&img!=this)
				this->copy(img.mWidth,img.mHeight,img.mImage);
			return *this;
		}
		virtual std::shared_ptr<drawable> clone() noexcept override
		{
			return std::make_shared<picture>(*this);
		}
		virtual bool usable() const noexcept override
		{
			return this->mImage!=nullptr;
		}
		virtual std::size_t get_width() const override
		{
			return this->mWidth;
		}
		virtual std::size_t get_height() const override
		{
			return this->mHeight;
		}
		virtual void resize(std::size_t w,std::size_t h) override
		{
			if(w==this->mWidth&&h==this->mHeight)
				return;
			delete[] this->mImage;
			this->mImage=new pixel[h*w];
			this->mWidth=w;
			this->mHeight=h;
		}
		virtual void fill(const pixel& pix) override
		{
			if(this->mImage==nullptr)
				Darwin_Error("Use of not available object.");
			for(pixel* it=this->mImage; it!=this->mImage+this->mWidth*this->mHeight; ++it) *it=pix;
		}
		virtual void clear() override
		{
			if(this->mImage!=nullptr) {
				delete[] this->mImage;
				this->mImage=new pixel[mHeight*mWidth];
			}
		}
		virtual const pixel& get_pixel(std::size_t x,std::size_t y) const override
		{
			if(this->mImage==nullptr)
				Darwin_Error("Use of not available object.");
			if(x>this->mWidth-1||y>this->mHeight-1)
				Darwin_Error("Out of range.");
			return this->mImage[y*this->mWidth+x];
		}
		virtual void draw_pixel(int x,int y,const pixel& pix) override
		{
			if(this->mImage==nullptr)
				Darwin_Error("Use of not available object.");
			if(x<0||y<0||x>this->mWidth-1||y>this->mHeight-1) {
				Darwin_Warning("Out of range.");
				return;
			}
			this->mImage[y*this->mWidth+x]=pix;
		}
	};
	bool serial_picture(drawable* pic,std::deque<char>& dat)
	{
		if(pic==nullptr) return false;
		if(pic->get_width()>9999||pic->get_height()>9999) return false;
		static std::string tmp;
		dat.clear();
		tmp=std::to_string(pic->get_width());
		for(int count=4-tmp.size(); count>0; --count)
			dat.push_back('0');
		for(auto& ch:tmp)
			dat.push_back(ch);
		tmp=std::to_string(pic->get_height());
		for(int count=4-tmp.size(); count>0; --count)
			dat.push_back('0');
		for(auto& ch:tmp)
			dat.push_back(ch);
		for(std::size_t y=0; y<pic->get_height(); ++y) {
			for(std::size_t x=0; x<pic->get_width(); ++x) {
				const pixel& pix=pic->get_pixel(x,y);
				dat.push_back(pix.get_char());
				if(pix.is_bright())
					dat.push_back('0');
				else
					dat.push_back('1');
				if(pix.is_underline())
					dat.push_back('0');
				else
					dat.push_back('1');
				switch(pix.get_front_color()) {
				case colors::white:
					dat.push_back('1');
					break;
				case colors::black:
					dat.push_back('2');
					break;
				case colors::red:
					dat.push_back('3');
					break;
				case colors::green:
					dat.push_back('4');
					break;
				case colors::blue:
					dat.push_back('5');
					break;
				case colors::pink:
					dat.push_back('6');
					break;
				case colors::yellow:
					dat.push_back('7');
					break;
				case colors::cyan:
					dat.push_back('8');
					break;
				}
				switch(pix.get_back_color()) {
				case colors::white:
					dat.push_back('1');
					break;
				case colors::black:
					dat.push_back('2');
					break;
				case colors::red:
					dat.push_back('3');
					break;
				case colors::green:
					dat.push_back('4');
					break;
				case colors::blue:
					dat.push_back('5');
					break;
				case colors::pink:
					dat.push_back('6');
					break;
				case colors::yellow:
					dat.push_back('7');
					break;
				case colors::cyan:
					dat.push_back('8');
					break;
				}
			}
		}
		return true;
	}
	bool unserial_picture(drawable* pic,const std::deque<char>& dat)
	{
		if(pic==nullptr) return false;
		std::string tmp;
		tmp= {dat[0],dat[1],dat[2],dat[3]};
		std::size_t w=std::stoul(tmp);
		tmp= {dat[4],dat[5],dat[6],dat[7]};
		std::size_t h=std::stoul(tmp);
		pic->resize(w,h);
		pixel pix;
		int x(0),y(0);
		for(std::size_t i=8; i<dat.size(); i+=5) {
			pix.set_char(dat[i]);
			if(dat[i+1]=='0')
				pix.set_bright(true);
			else
				pix.set_bright(false);
			if(dat[i+2]=='0')
				pix.set_underline(true);
			else
				pix.set_underline(false);
			switch(dat[i+3]) {
			case '1':
				pix.set_front_color(colors::white);
				break;
			case '2':
				pix.set_front_color(colors::black);
				break;
			case '3':
				pix.set_front_color(colors::red);
				break;
			case '4':
				pix.set_front_color(colors::green);
				break;
			case '5':
				pix.set_front_color(colors::blue);
				break;
			case '6':
				pix.set_front_color(colors::pink);
				break;
			case '7':
				pix.set_front_color(colors::yellow);
				break;
			case '8':
				pix.set_front_color(colors::cyan);
				break;
			}
			switch(dat[i+4]) {
			case '1':
				pix.set_back_color(colors::white);
				break;
			case '2':
				pix.set_back_color(colors::black);
				break;
			case '3':
				pix.set_back_color(colors::red);
				break;
			case '4':
				pix.set_back_color(colors::green);
				break;
			case '5':
				pix.set_back_color(colors::blue);
				break;
			case '6':
				pix.set_back_color(colors::pink);
				break;
			case '7':
				pix.set_back_color(colors::yellow);
				break;
			case '8':
				pix.set_back_color(colors::cyan);
				break;
			}
			pic->draw_pixel(x,y,pix);
			if(x==w-1) {
				x=0;
				++y;
			} else
				++x;
		}
		return true;
	}
}

namespace darwin {
	class sync_clock final {
	private:
		timer_t mBegin;
		std::size_t mFreq;
	public:
		sync_clock():mBegin(timer::time()),mFreq(60) {}
		sync_clock(std::size_t freq):mBegin(timer::time()),mFreq(freq) {}
		~sync_clock()=default;
		std::size_t get_freq() const
		{
			return mFreq;
		}
		void set_freq(std::size_t freq)
		{
			mFreq=freq;
		}
		void reset()
		{
			mBegin=timer::time();
		}
		void sync()
		{
			timer_t spend=timer::time()-mBegin;
			timer_t period=1000/mFreq;
			if(period>spend)
				timer::delay(period-spend);
		}
	};
	class darwin_rt final {
	protected:
		timer_t m_time_out=1000;
		module_adapter* m_module=nullptr;
		platform_adapter* m_platform=nullptr;
		bool wait_for_module();
		bool wait_for_platform();
	public:
		darwin_rt()=delete;
		darwin_rt(module_adapter* module):m_module(module) {}
		darwin_rt(const darwin_rt&)=delete;
		darwin_rt(darwin_rt&&) noexcept=delete;
		~darwin_rt();
		void load(const std::string&);
		void exit(int);
		status get_state() const noexcept
		{
			if(m_module==nullptr) return status::error;
			if(m_platform==nullptr) return status::leisure;
			if(m_module->get_state()==status::busy || m_platform->get_state()==status::busy) return status::busy;
			if(m_module->get_state()==status::ready&&m_platform->get_state()==status::ready) return status::ready;
			return status::null;
		}
		void set_time_out(timer_t tl) noexcept
		{
			m_time_out=tl;
		}
		virtual bool is_kb_hit() noexcept
		{
			if(m_platform==nullptr) Darwin_Error("Adapter is not ready.");
			return m_platform->is_kb_hit();
		}
		virtual int get_kb_hit() noexcept
		{
			if(m_platform==nullptr) Darwin_Error("Adapter is not ready.");
			return m_platform->get_kb_hit();
		}
		results fit_drawable() noexcept
		{
			if(m_platform==nullptr) return results::failure;
			return m_platform->fit_drawable();
		}
		drawable* get_drawable() noexcept
		{
			if(m_platform==nullptr) return nullptr;
			return m_platform->get_drawable();
		}
		results update_drawable() noexcept
		{
			if(m_platform==nullptr) return results::failure;
			return m_platform->update_drawable();
		}
	};
}
bool darwin::darwin_rt::wait_for_module()
{
	if(m_module==nullptr) return false;
	for(timer_t nt=timer::time(); m_module->get_state()==status::busy&&timer::time()-nt<=m_time_out;);
	if(m_module->get_state()==status::busy) return false;
	else return true;
}
bool darwin::darwin_rt::wait_for_platform()
{
	if(m_platform==nullptr) return false;
	for(timer_t nt=timer::time(); m_platform->get_state()==status::busy&&timer::time()-nt<=m_time_out;);
	if(m_platform->get_state()==status::busy) return false;
	else return true;
}
darwin::darwin_rt::~darwin_rt()
{
	if(m_platform!=nullptr)
		if(wait_for_platform()&&m_platform->get_state()==status::ready)
			m_platform->stop();
	if(m_module!=nullptr)
		if(wait_for_module()&&m_module->get_state()==status::ready)
			m_module->free_module();
}
void darwin::darwin_rt::load(const std::string& file)
{
	if(get_state()!=status::leisure) Darwin_Error("Adapter Busy.");
	if(wait_for_module()&&m_module->get_state()==status::leisure) {
		if(m_module->load_module(file)==results::failure) Darwin_Error("Adapter returns failure.");
	} else
		Darwin_Error("Adapter Busy.");
	m_platform=m_module->get_platform_adapter();
	m_platform->init();
}
void darwin::darwin_rt::exit(int code=0)
{
	if(m_platform!=nullptr)
		if(wait_for_platform()&&m_platform->get_state()==status::ready)
			m_platform->stop();
	if(m_module!=nullptr)
		if(wait_for_module()&&m_module->get_state()==status::ready)
			m_module->free_module();
	m_platform=nullptr;
	m_module=nullptr;
	std::exit(code);
}

namespace conio {
	static int terminal_width()
	{
		struct winsize size;
		ioctl(STDIN_FILENO, TIOCGWINSZ, &size);
		return size.ws_col;
	}
	static int terminal_height()
	{
		struct winsize size;
		ioctl(STDIN_FILENO, TIOCGWINSZ, &size);
		return size.ws_row-1;
	}
	static void gotoxy(int x, int y)
	{
		printf("\x1B[%d;%df", y, x);
	}
	static void echo(bool in)
	{
		struct termios oldt, newt;
		tcgetattr(0, &oldt);
		newt = oldt;
		if (in) {
			printf("\33[?25h");
			newt.c_lflag |= ECHO;
			newt.c_lflag |= ICANON;
		} else {
			printf("\33[?25l");
			newt.c_lflag &= ~ECHO;
			newt.c_lflag &= ~ICANON;
		}
		tcsetattr(0, TCSANOW, &newt);
	}
	static void reset()
	{
		printf("\e[37;40;0m");
	}
	static void clrscr()
	{
		printf("\x1B[2J\x1B[0;0f");
	}
	static int getch()
	{
		int ch;
		ch = getchar();
		return ch;
	}
	static int kbhit()
	{
		int ret;
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
}

namespace darwin {
	class unix_adapter:public platform_adapter {
		bool mReady=false;
		picture mDrawable;
	public:
		unix_adapter()=default;
		virtual ~unix_adapter()=default;
		virtual status get_state() const noexcept override
		{
			if(mReady)
				return status::ready;
			else
				return status::leisure;
		}
		virtual results init() noexcept override
		{
			conio::reset();
			conio::clrscr();
			conio::echo(false);
			mReady = true;
			return results::success;
		}
		virtual results stop() noexcept override
		{
			conio::reset();
			conio::clrscr();
			conio::echo(true);
			mReady = false;
			return results::success;
		}
		virtual results exec_commands(commands c) noexcept override
		{
			switch(c) {
			case commands::echo_on:
				conio::echo(true);
				break;
			case commands::echo_off:
				conio::echo(false);
				break;
			case commands::reset_cursor:
				conio::gotoxy(0,0);
				break;
			case commands::reset_attri:
				conio::reset();
				break;
			case commands::clrscr:
				conio::clrscr();
				break;
			}
			return results::success;
		}
		virtual bool is_kb_hit() noexcept
		{
			return conio::kbhit();
		}
		virtual int get_kb_hit() noexcept
		{
			return conio::getch();
		}
		virtual results fit_drawable() noexcept override
		{
			mDrawable.resize(conio::terminal_width(),conio::terminal_height());
			return results::success;
		}
		virtual drawable* get_drawable() noexcept override
		{
			return &mDrawable;
		}
		virtual results update_drawable() noexcept override
		{
			conio::gotoxy(0,0);
			int sw(conio::terminal_width()),sh(conio::terminal_height());
			for(std::size_t y=0; y<mDrawable.get_height()&&y<sh; ++y) {
				for(std::size_t x=0; x<mDrawable.get_width()&&x<sw; ++x) {
					const pixel& pix=mDrawable.get_pixel(x,y);
					std::string cmd="\e[0m\e[";
					switch(pix.get_front_color()) {
					case colors::white:
						cmd+="37;";
						break;
					case colors::black:
						cmd+="30;";
						break;
					case colors::red:
						cmd+="31;";
						break;
					case colors::green:
						cmd+="32;";
						break;
					case colors::blue:
						cmd+="34;";
						break;
					case colors::pink:
						cmd+="35;";
						break;
					case colors::yellow:
						cmd+="33;";
						break;
					case colors::cyan:
						cmd+="36;";
						break;
					default:
						return results::failure;
						break;
					}
					switch(pix.get_back_color()) {
					case colors::white:
						cmd+="47";
						break;
					case colors::black:
						cmd+="40";
						break;
					case colors::red:
						cmd+="41";
						break;
					case colors::green:
						cmd+="42";
						break;
					case colors::blue:
						cmd+="44";
						break;
					case colors::pink:
						cmd+="45";
						break;
					case colors::yellow:
						cmd+="43";
						break;
					case colors::cyan:
						cmd+="46";
						break;
					default:
						return results::failure;
						break;
					}
					if(pix.is_bright())
						cmd+=";1";
					if(pix.is_underline())
						cmd+=";4";
					cmd+="m";
					printf("%s%c",cmd.c_str(),pix.get_char());
				}
				conio::reset();
				printf("\n");
			}
			fflush(stdout);
			return results::success;
		}
	} dunixadapter;
	platform_adapter* module_resource()
	{
		return &dunixadapter;
	}
}

namespace darwin {
	class unix_module_adapter:public module_adapter {
		void* m_handle=nullptr;
		platform_adapter* m_adapter=nullptr;
		static void force_exit(int);
		static void handle_segfault(int);
	public:
		unix_module_adapter()=default;
		virtual ~unix_module_adapter()=default;
		virtual status get_state() const noexcept override
		{
			if(m_adapter!=nullptr)
				return status::ready;
			else
				return status::leisure;
		}
		virtual results load_module(const std::string& path) noexcept override
		{
			signal(SIGSEGV,handle_segfault);
			signal(SIGINT,force_exit);
			signal(SIGABRT,force_exit);
			m_adapter=module_resource();
			if(m_adapter==nullptr) return results::failure;
			return results::success;
		}
		virtual results free_module() noexcept override
		{
			signal(SIGSEGV,nullptr);
			signal(SIGINT,nullptr);
			signal(SIGABRT,nullptr);
			m_adapter=nullptr;
			return results::success;
		}
		virtual platform_adapter* get_platform_adapter() noexcept override
		{
			return this->m_adapter;
		}
	} dunixmodule;
	static darwin_rt runtime(&dunixmodule);
}
void darwin::unix_module_adapter::force_exit(int flag)
{
	printf("Darwin have been exited safety.\n");
	runtime.exit(0);
}
void darwin::unix_module_adapter::handle_segfault(int flag)
{
	printf("Your program have some problem about the Segmentation Fault.Please check your program after we terminate this program.\n");
	printf("Darwin have been exited safety.\n");
	runtime.exit(1);
}

namespace darwin {
	static std::string screen_shot_path="./darwin_screen_shot.cdpf";
	void print_screen()
	{
		static std::deque<char> file_buffer;
		serial_picture(runtime.get_drawable(),file_buffer);
		outfs out(screen_shot_path);
		for(auto&it:file_buffer)
			out.printf("%c",it);
		out.flush();
	}
}