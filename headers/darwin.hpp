#pragma once
#include "./timer.hpp"
#include "./core.hpp"
#include "./adapter.hpp"
#include <string>
#include <cstdlib>
#include <cmath>
#include <deque>
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
				for(int i=0; i<this->mWidth*this->mHeight; ++i)
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
		virtual const pixel& get_pixel(const std::array<std::size_t,2>& posit) const override
		{
			if(this->mImage==nullptr)
				throw std::logic_error(__func__);
			if(posit[0]<0||posit[1]<0||posit[0]>this->mWidth-1||posit[1]>this->mHeight-1)
				throw std::out_of_range(__func__);
			return this->mImage[posit[1]*this->mWidth+posit[0]];
		}
		// 单点绘制函数
		virtual void draw_pixel(const std::array<std::size_t,2>& posit,const pixel& pix) override
		{
			if(this->mImage==nullptr)
				throw std::logic_error(__func__);
			if(posit[0]<0||posit[1]<0||posit[0]>this->mWidth-1||posit[1]>this->mHeight-1)
				throw std::out_of_range(__func__);
			this->mImage[posit[1]*this->mWidth+posit[0]]=pix;
		}
		// 多点绘制函数，可设置为连接各个点
		void draw_pixels(const std::deque<std::array<std::size_t,2>>& points,const pixel& pix,bool connect_points=false)
		{
			if(!connect_points) {
				for(auto &it:points)
					this->draw_pixel(it,pix);
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
						this->draw_pixel({std::size_t(x1+c*w),std::size_t(y1+c*h)},pix);
					}
				}
			}
		}
		// 绘制图像函数
		virtual void draw_picture(const std::array<std::size_t,2>& posit,const drawable& img) override
		{
			std::size_t col(posit[0]),row(posit[1]);
			if(this->mImage==nullptr)
				throw std::logic_error(__func__);
			if(col<0||row<0||col>this->mWidth-1||row>this->mHeight-1)
				throw std::out_of_range(__func__);
			for(std::size_t r=row; r<this->mHeight&&r-row<img.get_height(); ++r)
				for(std::size_t c=col; c<this->mWidth&&c-col<img.get_width(); ++c)
					this->mImage[r*this->mWidth+c]=img.get_pixel({r-row,c-col});
		}
	};
	class darwin final {
	protected:
		timer_t m_time_out=1000;
		dll_adapter* m_dll=nullptr;
		platform_adapter* m_platform=nullptr;
		bool wait_for_dll();
		bool wait_for_platform();
	public:
		darwin()=delete;
		darwin(dll_adapter* dll):m_dll(dll) {}
		darwin(const darwin&)=delete;
		darwin(darwin&&) noexcept=delete;
		~darwin();
		void load(const std::string&);
		void exit(int);
		status get_state() const noexcept
		{
			if(m_dll==nullptr) return status::error;
			if(m_platform==nullptr) return status::leisure;
			if(m_dll->get_state()==status::busy || m_platform->get_state()==status::busy) return status::busy;
			if(m_dll->get_state()==status::ready&&m_platform->get_state()==status::ready) return status::ready;
			return status::null;
		}
		void set_time_out(timer_t tl) noexcept
		{
			m_time_out=tl;
		}
		platform_adapter* get_adapter() noexcept
		{
			return m_platform;
		}
	};
}
bool darwin::darwin::wait_for_dll()
{
	if(m_dll==nullptr) return false;
	for(timer_t nt=timer::time(); m_dll->get_state()==status::busy&&timer::time()-nt<=m_time_out;);
	if(m_dll->get_state()==status::busy) return false;
	else return true;
}
bool darwin::darwin::wait_for_platform()
{
	if(m_platform==nullptr) return false;
	for(timer_t nt=timer::time(); m_platform->get_state()==status::busy&&timer::time()-nt<=m_time_out;);
	if(m_platform->get_state()==status::busy) return false;
	else return true;
}
darwin::darwin::~darwin()
{
	if(m_platform!=nullptr)
		if(wait_for_platform()&&m_platform->get_state()==status::ready)
			m_platform->stop();
	if(m_dll!=nullptr)
		if(wait_for_dll()&&m_dll->get_state()==status::ready)
			m_dll->free_dll();
}
void darwin::darwin::load(const std::string& file)
{
	if(get_state()!=status::leisure) throw std::logic_error(__func__);
	if(wait_for_dll()&&m_dll->get_state()==status::leisure)
		m_dll->load_dll(file);
	m_platform=m_dll->get_platform_adapter();
	m_platform->init();
}
void darwin::darwin::exit(int code=0)
{
	if(m_platform!=nullptr)
		if(wait_for_platform()&&m_platform->get_state()==status::ready)
			m_platform->stop();
	if(m_dll!=nullptr)
		if(wait_for_dll()&&m_dll->get_state()==status::ready)
			m_dll->free_dll();
	std::exit(code);
}