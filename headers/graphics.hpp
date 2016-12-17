#pragma once
#include <cstddef>
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
		virtual const pixel& get_pixel(const std::array<std::size_t,2>& posit) const override
		{
			if(this->mImage==nullptr)
				throw std::logic_error(__func__);
			if(posit[0]>this->mWidth-1||posit[1]>this->mHeight-1)
				throw std::out_of_range(__func__);
			return this->mImage[posit[1]*this->mWidth+posit[0]];
		}
		// 单点绘制函数
		virtual void draw_pixel(const std::array<std::size_t,2>& posit,const pixel& pix) override
		{
			if(this->mImage==nullptr)
				throw std::logic_error(__func__);
			if(posit[0]>this->mWidth-1||posit[1]>this->mHeight-1)
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
	};
}