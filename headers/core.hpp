#pragma once
#include <array>
#include <string>
#include <iosfwd>
#include <memory>
#include <typeinfo>
#include <typeindex>
namespace darwin {
	enum class status {
		null,ready,leisure,error
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
		std::array<bool,2> mAttris= {false,false};
		std::array<colors,2> mColors= {colors::white,colors::black};
	public:
		pixel()=default;
		pixel(const pixel&)=default;
		pixel(char ch,const std::array<bool,2>& a,const std::array<colors,2>& c):mChar(ch),mAttris(a),mColors(c) {}
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
		virtual const pixel& get_pixel(const std::array<std::size_t,2>&) const=0;
		virtual void draw_pixel(const std::array<std::size_t,2>&,const pixel&)=0;
	};
}