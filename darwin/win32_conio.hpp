#pragma once
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
#include <windows.h>
#include <cstring>
#include <conio.h>
#include <cstdlib>

namespace conio {
	static HANDLE StdHandle = GetStdHandle(STD_OUTPUT_HANDLE);

	static int terminal_width()
	{
		static CONSOLE_SCREEN_BUFFER_INFO csbi;
		GetConsoleScreenBufferInfo(StdHandle, &csbi);
		return csbi.srWindow.Right - csbi.srWindow.Left;
	}

	static int terminal_height()
	{
		static CONSOLE_SCREEN_BUFFER_INFO csbi;
		GetConsoleScreenBufferInfo(StdHandle, &csbi);
		return csbi.srWindow.Bottom - csbi.srWindow.Top;
	}

	static void gotoxy(SHORT x, SHORT y)
	{
		SetConsoleCursorPosition(StdHandle, {x, y});
	}

	static void echo(bool mode)
	{
		static CONSOLE_CURSOR_INFO cci;
		GetConsoleCursorInfo(StdHandle, &cci);
		cci.bVisible = mode;
		SetConsoleCursorInfo(StdHandle, &cci);
	}

	class console final {
		SHORT w, h;
		std::size_t offset = 0;
		CHAR_INFO *buffer = nullptr;
		WORD attri = 0;
	public:
		console() = delete;

		console(std::size_t width, std::size_t height) : w(width), h(height), buffer(new CHAR_INFO[w * h]) {}

		~console()
		{
			delete[] buffer;
		}

		void set_color(int textcolor, int bgcolor)
		{
			attri = textcolor + (bgcolor - 8) * 16;
		}

		void reset()
		{
			set_color(15, 8);
		}

		void put_char(char ch)
		{
			buffer[offset].Attributes = attri;
			buffer[offset].Char.AsciiChar = ch;
			++offset;
		}

		void flush()
		{
			static CONSOLE_SCREEN_BUFFER_INFO csbi;
			GetConsoleScreenBufferInfo(StdHandle, &csbi);
			WriteConsoleOutput(StdHandle, buffer, {w, h}, {0, 0}, &csbi.srWindow);
		}
	};

	static void set_title(const char *title)
	{
		SetConsoleTitle(title);
	}

	static void set_color(int textcolor, int bgcolor)
	{
		SetConsoleTextAttribute(StdHandle, textcolor + (bgcolor - 8) * 16);
	}

	static void reset()
	{
		set_color(15, 8);
	}

	static void clrscr()
	{
		system("cls");
	}

	static int getch()
	{
		return ::getch();
	}

	static int kbhit()
	{
		return ::kbhit();
	}
}