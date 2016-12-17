#pragma once
#include "../sources/unix_adapter.cpp"
namespace darwin
{
    class unix_dll_adapter:public dll_adapter{
        platform_adapter* m_adapter;
        public:
        unix_dll_adapter()=delete;
        unix_dll_adapter(platform_adapter* adapter):m_adapter(adapter){}
        virtual ~unix_dll_adapter()=default;
        virtual status get_state() const noexcept override{
            return m_adapter->get_state();
        }
		virtual results load_dll(const std::string&) noexcept override{
            return results::success;
        }
		virtual results free_dll() noexcept override{
            return results::success;
        }
		virtual platform_adapter* get_platform_adapter() noexcept override{
            return this->m_adapter;
        }
    } dunixdll(&dunixadapter);
    static darwin runtime(&dunixdll);
}