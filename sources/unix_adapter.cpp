#include "../headers/module.hpp"
#include "../headers/graphics.hpp"
#include <iostream>
#include <string>
#include <unistd.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <sys/select.h>
namespace darwin {
	class unix_adapter:public platform_adapter {
		bool mReady=false;
		picture mDrawable;
	public:
		static std::size_t get_terminal_width() {
			struct winsize size;
			ioctl(STDIN_FILENO, TIOCGWINSZ, &size);
			return size.ws_col;
		}
		static std::size_t get_terminal_height() {
			struct winsize size;
			ioctl(STDIN_FILENO, TIOCGWINSZ, &size);
			return size.ws_row-1;
		}
		static void echo(bool in) {
			std::cout << std::flush;
			struct termios oldt, newt;
			tcgetattr(0, &oldt);
			newt = oldt;
			if (in) {
				std::cout << "\33[?25h" << std::flush;
				newt.c_lflag |= ECHO;
				newt.c_lflag |= ICANON;
			} else {
				std::cout << "\33[?25l" << std::flush;
				newt.c_lflag &= ~ECHO;
				newt.c_lflag &= ~ICANON;
			}
			tcsetattr(0, TCSANOW, &newt);
		}
		static void clrscr() {
			std::cout<<"\e[2J\e[0;0f"<<std::flush;
		}
		static void rst_attri() {
			std::cout<<"\e[37;40;0m"<<std::flush;
		}
		static void rst_cursor() {
			std::cout<<"\e[0;0f"<<std::flush;
		}
		unix_adapter()=default;
		virtual ~unix_adapter()=default;
		virtual status get_state() const noexcept override {
			if(mReady)
				return status::ready;
			else
				return status::leisure;
		}
		virtual results init() noexcept override {
			rst_attri();
			clrscr();
			mReady = true;
			std::ios::sync_with_stdio(false);
			echo(false);
			return results::success;
		}
		virtual results stop() noexcept override {
			rst_attri();
			mReady = false;
			std::ios::sync_with_stdio(true);
			echo(true);
			return results::success;
		}
		virtual results exec_commands(commands c) noexcept override {
			switch(c) {
			case commands::echo_on:
				echo(true);
				break;
			case commands::echo_off:
				echo(false);
				break;
			case commands::reset_cursor:
				rst_cursor();
				break;
			case commands::reset_attri:
				rst_attri();
				break;
			case commands::clrscr:
				clrscr();
				break;
			}
			return results::success;
		}
		virtual results fit_drawable() noexcept override {
			mDrawable.resize(get_terminal_width(),get_terminal_height());
			return results::success;
		}
		virtual drawable* get_drawable() noexcept override {
			return &mDrawable;
		}
		virtual results update_drawable() noexcept override {
			rst_cursor();
			const std::size_t sw(get_terminal_width()),sh(get_terminal_height());
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
					std::cout<<cmd<<pix.get_char();
				}
				rst_attri();
				std::cout<<std::endl;
			}
			return results::success;
		}
	} dunixadapter;
	platform_adapter* module_resource()
	{
		return &dunixadapter;
	}
}