#pragma once
#include "../sources/unix_adapter.cpp"
#include <csignal>
namespace darwin {
	class unix_dll_adapter:public dll_adapter {
		platform_adapter* m_adapter;
		static void force_exit(int);
		static void handle_segfault(int);
	public:
		unix_dll_adapter()=delete;
		unix_dll_adapter(platform_adapter* adapter):m_adapter(adapter) {}
		virtual ~unix_dll_adapter()=default;
		virtual status get_state() const noexcept override
		{
			return m_adapter->get_state();
		}
		virtual results load_dll(const std::string&) noexcept override
		{
			signal(SIGSEGV,handle_segfault);
			signal(SIGINT,force_exit);
			signal(SIGABRT,force_exit);
			return results::success;
		}
		virtual results free_dll() noexcept override
		{
			signal(SIGSEGV,nullptr);
			signal(SIGINT,nullptr);
			signal(SIGABRT,nullptr);
			return results::success;
		}
		virtual platform_adapter* get_platform_adapter() noexcept override
		{
			return this->m_adapter;
		}
	} dunixdll(&dunixadapter);
	static darwin runtime(&dunixdll);
}
void darwin::unix_dll_adapter::force_exit(int flag)
{
	printf("Darwin have been exited safety.\n");
	runtime.exit(0);
}
void darwin::unix_dll_adapter::handle_segfault(int flag)
{
	printf("Your program have some problem about the Segmentation Fault.Please check your program after we terminate this program.\n");
	printf("Darwin have been exited safety.\n");
	runtime.exit(1);
}
