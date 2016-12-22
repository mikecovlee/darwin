#pragma once
#include <cstddef>
#include <fstream>
#include <cmath>
#include <deque>
#include "./core.hpp"
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
		// 重新设置尺寸
		// resize将令您失去所有数据，请注意备份
		virtual void resize(std::size_t w,std::size_t h) override
		{
			if(w==this->mWidth&&h==this->mHeight) return;
			delete[] this->mImage;
			this->mImage=new pixel[h*w];
			this->mWidth=w;
			this->mHeight=h;
		}
		// 填充图像
		virtual void fill(const pixel& pix) override
		{
			if(this->mImage==nullptr)
				throw std::logic_error(__func__);
			for(pixel* it=this->mImage; it!=this->mImage+this->mWidth*this->mHeight; ++it) *it=pix;
		}
		// 清空图像
		virtual void clear() override
		{
			if(this->mImage!=nullptr) {
				delete[] this->mImage;
				this->mImage=new pixel[mHeight*mWidth];
			}
		}
		// 通过二维坐标访问图像
		virtual const pixel& get_pixel(std::size_t x,std::size_t y) const override
		{
			if(this->mImage==nullptr)
				throw std::logic_error(__func__);
			if(x>this->mWidth-1||y>this->mHeight-1)
				throw std::out_of_range(__func__);
			return this->mImage[y*this->mWidth+x];
		}
		// 单点绘制函数
		virtual void draw_pixel(int x,int y,const pixel& pix) override
		{
			if(this->mImage==nullptr)
				throw std::logic_error(__func__);
			if(debug_mod&&(x<0||y<0||x>this->mWidth-1||y>this->mHeight-1))
				throw std::out_of_range(__func__);
			this->mImage[y*this->mWidth+x]=pix;
		}
		// 多点绘制函数，可设置为连接各个点
		void draw_pixels(const std::deque<std::array<std::size_t,2>>& points,const pixel& pix,bool connect_points=false)
		{
			if(!connect_points) {
				for(auto &it:points)
					this->draw_pixel(it[0],it[1],pix);
			} else {
				auto p0=points.begin();
				auto p1=++points.begin();
				for(; p1!=points.end(); ++p0,++p1) {
					double x1(p0->at(0)),x2(p1->at(0)),y1(p0->at(1)),y2(p1->at(1)),w(x2-x1),h(y2-y1),a(0);
					if(w>=h)
						a=w;
					else
						a=h;
					a=std::abs(a);
					for(double c=0; c<=1; c+=1.0/a) {
						this->draw_pixel(int(x1+c*w),int(y1+c*h),pix);
					}
				}
			}
		}
	};
	bool serial_picture(const drawable& pic,std::deque<char>& dat)
	{
		if(pic.get_width()>9999||pic.get_height()>9999) return false;
		static std::string tmp;
		dat.clear();
		tmp=std::to_string(pic.get_width());
		for(int count=4-tmp.size();count>0;--count)
			dat.push_back('0');
		for(auto& ch:tmp)
			dat.push_back(ch);
		tmp=std::to_string(pic.get_height());
		for(int count=4-tmp.size();count>0;--count)
			dat.push_back('0');
		for(auto& ch:tmp)
			dat.push_back(ch);
		for(std::size_t y=0;y<pic.get_height();++y)
		{
			for(std::size_t x=0;x<pic.get_width();++x)
			{
				const pixel& pix=pic.get_pixel({x,y});
				if(pix.is_bright())
					dat.push_back('0');
				else
					dat.push_back('1');
				if(pix.is_underline())
					dat.push_back('0');
				else
					dat.push_back('1');
				switch(pix.get_front_color())
				{
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
				switch(pix.get_back_color())
				{
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
				dat.push_back(pix.get_char());
			}
		}
		return true;
	}
	void print_screen(const drawable& pic)
	{
		static std::deque<char> file_buffer;
		darwin::serial_picture(pic,file_buffer);
		std::ofstream out("./darwin_screen_shot.cdpf");
		for(auto&it:file_buffer)
			out<<it;
	}
}