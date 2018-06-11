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
#include <functional>
#include <thread>
#include <chrono>

namespace darwin {
	typedef unsigned long timer_t;

	class timer;
}
class darwin::timer final {
	static std::chrono::time_point <std::chrono::high_resolution_clock> m_timer;
public:
	enum class time_unit {
		nano_sec, micro_sec, milli_sec, second, minute
	};

	static void reset()
	{
		m_timer = std::chrono::high_resolution_clock::now();
	}

	static timer_t time(time_unit unit = time_unit::milli_sec)
	{
		switch (unit) {
		case time_unit::nano_sec:
			return std::chrono::duration_cast<std::chrono::nanoseconds>(
			           std::chrono::high_resolution_clock::now() - m_timer).count();
		case time_unit::micro_sec:
			return std::chrono::duration_cast<std::chrono::microseconds>(
			           std::chrono::high_resolution_clock::now() - m_timer).count();
		case time_unit::milli_sec:
			return std::chrono::duration_cast<std::chrono::milliseconds>(
			           std::chrono::high_resolution_clock::now() - m_timer).count();
		case time_unit::second:
			return std::chrono::duration_cast<std::chrono::seconds>(
			           std::chrono::high_resolution_clock::now() - m_timer).count();
		case time_unit::minute:
			return std::chrono::duration_cast<std::chrono::minutes>(
			           std::chrono::high_resolution_clock::now() - m_timer).count();
		}
		return 0;
	}

	static void delay(timer_t time, time_unit unit = time_unit::milli_sec)
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

	static timer_t measure(const std::function<void()> &func, time_unit unit = time_unit::milli_sec)
	{
		timer_t begin(0), end(0);
		begin = time(unit);
		func();
		end = time(unit);
		return end - begin;
	}
};

std::chrono::time_point <std::chrono::high_resolution_clock>
darwin::timer::m_timer(std::chrono::high_resolution_clock::now());