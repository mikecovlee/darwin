#pragma once
#include <random>
#include <ctime>
namespace darwin {
	static std::default_random_engine random_engine(time(nullptr));
	template<typename T>
	T rand(T begin,T end)
	{
		return std::uniform_int_distribution<T>(begin,end)(random_engine);
	}
}
