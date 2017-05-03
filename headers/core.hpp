#pragma once
/*
* Covariant Darwin Universal Character Interface Generation 2
* Copyright (C) 2017 Michael Lee
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
*/
#include <array>
#include <cmath>
#include <string>
#include <typeinfo>
#include <stdexcept>
#include "./dll.hpp"
namespace darwin {
    enum class colors {
		white,black,red,green,blue,pink,yellow,cyan
	};
    class pixel final {
		char mChar=' ';
		std::array<colors,2> mColors{colors::white,colors::black};
	public:
		pixel()=default;
		pixel(const pixel&)=default;
		pixel(char ch,colors fcolor=colors::white,colors bcolor=colors::black):mChar(ch),mColors({fcolor,bcolor})
		{
			if(ch<=31||ch>=127)
				mChar='\?';
		}
		~pixel()=default;
		void set_char(char c)
		{
			if(c>31&&c<127)
				mChar=c;
			else
				mChar='\?';
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
	};
	class canvas {
	public:
		static double draw_line_precision;
		canvas()=default;
		canvas(const canvas&)=default;
		virtual ~canvas()=default;
		virtual const std::type_info& get_type() const final
		{
			return typeid(*this);
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
				throw std::logic_error("Use of not available object.");
			/*if(p0x<0||p0y<0||p1x<0||p1y<0||p0x>this->get_width()-1||p0y>this->get_height()-1||p1x>this->get_width()-1||p1y>this->get_height()-1)
				Darwin_Warning("Out of range.");*/
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
				throw std::logic_error("Use of not available object.");
			/*if(x<0||y<0||x>this->get_width()-1||y>this->get_height()-1||x+w>this->get_width()||y+h>this->get_height())
				Darwin_Warning("Out of range.");*/
			for(int i=0; i!=w; w>0?++i:--i) {
				this->draw_pixel(x+i,y,pix);
				this->draw_pixel(x+i,y+(h>0?h-1:h+1),pix);
			}
			for(int i=0; i!=h; h>0?++i:--i) {
				this->draw_pixel(x,y+i,pix);
				this->draw_pixel(x+(w>0?w-1:w+1),y+i,pix);
			}
		}
		virtual void fill_rect(int x,int y,int w,int h,const pixel& pix)
		{
			if(!this->usable())
				throw std::logic_error("Use of not available object.");
			/*if(x<0||y<0||x>this->get_width()-1||y>this->get_height()-1||x+w>this->get_width()||y+h>this->get_height())
				Darwin_Warning("Out of range.");*/
			for(int cy=0; cy!=h; h>0?++cy:--cy)
				for(int cx=0; cx!=w; w>0?++cx:--cx)
					this->draw_pixel(x+cx,y+cy,pix);
		}
		virtual void draw_triangle(int x1,int y1,int x2,int y2,int x3,int y3,const pixel& pix)
		{
			if(!this->usable())
				throw std::logic_error("Use of not available object.");
			this->draw_line(x1,y1,x2,y2,pix);
			this->draw_line(x2,y2,x3,y3,pix);
			this->draw_line(x3,y3,x1,y1,pix);
		}
		virtual void fill_triangle(int x1,int y1,int x2,int y2,int x3,int y3,const pixel& pix)
		{
			if(!this->usable())
				throw std::logic_error("Use of not available object.");
			int v1x(x2-x1),v1y(y2-y1),v2x(x3-x2),v2y(y3-y2);
			/*if(v1x*v2y-v2x*v1y==0)
				Darwin_Warning("Three points in a line.");*/
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
				throw std::logic_error("Use of not available object.");
			/*if(x<0||y<0||x>this->get_width()-1||y>this->get_height()-1||x+str.size()>this->get_width())
				Darwin_Warning("Out of range.");*/
			pixel p=pix;
			for(int i=0; i<str.size(); ++i) {
				p.set_char(str.at(i));
				this->draw_pixel(x+i,y,p);
			}
		}
		virtual void draw_picture(int col,int row,const canvas& pic)
		{
			if(!this->usable()||!pic.usable())
				throw std::logic_error("Use of not available object.");
			/*if(col<0||row<0||col>this->get_width()-1||row>this->get_height()-1)
				Darwin_Warning("Out of range.");*/
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
	double canvas::draw_line_precision=1.5;
    class runtime final{
    public:
        runtime()=default;
        runtime(const runtime&)=delete;
        runtime& operator=(const runtime&)=delete;
        ~runtime();
        bool load(const std::string&);
        void join();
        void exit();
        std::size_t get_width() const;
        std::size_t get_height() const;
        bool is_kb_hit() const;
        int get_kb_hit() const;
        void update(const canvas&);
        void log(const std::string&);
    };
}
