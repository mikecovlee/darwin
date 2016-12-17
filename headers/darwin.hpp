#pragma once
#include "./timer.hpp"
#include "./core.hpp"
#include "./adapter.hpp"
#include "./graphics.hpp"
#include <string>
#include <cstdlib>
namespace darwin {
	class sync_clock final {
	private:
		timer_t mBegin;
		std::size_t mFreq;
	public:
		sync_clock():mBegin(timer::time()),mFreq(60) {}
		sync_clock(std::size_t freq):mBegin(timer::time()),mFreq(freq) {}
		~sync_clock()=default;
		std::size_t get_freq() const
		{
			return mFreq;
		}
		void set_freq(std::size_t freq)
		{
			mFreq=freq;
		}
		void reset()
		{
			mBegin=timer::time();
		}
		void sync()
		{
			timer_t spend=timer::time()-mBegin;
			timer_t period=1000/mFreq;
			if(period>spend)
				timer::delay(period-spend);
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
#if defined(__DFUNIX__)
#include "./unix_dll.hpp"
#else
#if defined(__WIN32__) || defined(WIN32)
#include "./win32_dll.hpp"
#else
#include "./unix_dll.hpp"
#endif
#endif
