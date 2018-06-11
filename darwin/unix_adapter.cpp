/*
* Covariant Darwin Universal Character Graphics Library
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*
* Copyright (C) 2018 Michael Lee(李登淳)
* Email: mikecovlee@163.com
* Github: https://github.com/mikecovlee
*/
#include "./module.hpp"
#include "./graphics.hpp"
#include "./unix_conio.hpp"
#include <cstdio>
#include <string>

namespace darwin {
	class unix_adapter : public platform_adapter {
		bool mReady = false;
		picture mDrawable;
	public:
		unix_adapter() = default;

		virtual ~unix_adapter() = default;

		virtual status get_state() const override
		{
			if (mReady)
				return status::ready;
			else
				return status::leisure;
		}

		virtual results init() override
		{
			conio::reset();
			//conio::clrscr();
			conio::echo(false);
			mReady = true;
			return results::success;
		}

		virtual results stop() override
		{
			conio::reset();
			//conio::clrscr();
			conio::echo(true);
			mReady = false;
			return results::success;
		}

		virtual results exec_commands(commands c) override
		{
			switch (c) {
			case commands::echo_on:
				conio::echo(true);
				break;
			case commands::echo_off:
				conio::echo(false);
				break;
			case commands::reset_cursor:
				conio::gotoxy(0, 0);
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

		virtual bool is_kb_hit() override
		{
			return conio::kbhit();
		}

		virtual int get_kb_hit() override
		{
			return conio::getch();
		}

		virtual results fit_drawable() override
		{
			mDrawable.resize(conio::terminal_width(), conio::terminal_height());
			return results::success;
		}

		virtual drawable *get_drawable() override
		{
			return &mDrawable;
		}

		virtual results update_drawable() override
		{
			conio::gotoxy(0, 0);
			std::size_t sw(std::min<std::size_t>(mDrawable.get_width(), conio::terminal_width())), sh(
			    std::min<std::size_t>(mDrawable.get_height(), conio::terminal_height()));
			for (std::size_t y = 0; y < sh; ++y) {
				for (std::size_t x = 0; x < sw; ++x) {
					const pixel &pix = mDrawable.get_pixel(x, y);
					std::string cmd = "\e[0m\e[";
					switch (pix.get_front_color()) {
					case colors::white:
						cmd += "37;";
						break;
					case colors::black:
						cmd += "30;";
						break;
					case colors::red:
						cmd += "31;";
						break;
					case colors::green:
						cmd += "32;";
						break;
					case colors::blue:
						cmd += "34;";
						break;
					case colors::pink:
						cmd += "35;";
						break;
					case colors::yellow:
						cmd += "33;";
						break;
					case colors::cyan:
						cmd += "36;";
						break;
					default:
						return results::failure;
						break;
					}
					switch (pix.get_back_color()) {
					case colors::white:
						cmd += "47";
						break;
					case colors::black:
						cmd += "40";
						break;
					case colors::red:
						cmd += "41";
						break;
					case colors::green:
						cmd += "42";
						break;
					case colors::blue:
						cmd += "44";
						break;
					case colors::pink:
						cmd += "45";
						break;
					case colors::yellow:
						cmd += "43";
						break;
					case colors::cyan:
						cmd += "46";
						break;
					default:
						return results::failure;
						break;
					}
					if (pix.is_bright())
						cmd += ";1";
					if (pix.is_underline())
						cmd += ";4";
					cmd += "m";
					printf("%s%c", cmd.c_str(), pix.get_char());
				}
				conio::reset();
				printf("\n");
			}
			fflush(stdout);
			return results::success;
		}
	} dunixadapter;

	platform_adapter *module_resource()
	{
		return &dunixadapter;
	}
}
