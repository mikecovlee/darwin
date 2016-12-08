#pragma once
#include "./core.hpp"
namespace darwin
{
    class platform_adapter
    {
        public:
        platform_adapter()=default;
        platform_adapter(const platform_adapter&)=delete;
        virtual ~platform_adapter()=default;
        virtual status get_state() const noexcept=0;
        virtual results init() noexcept=0;
        virtual results stop() noexcept=0;
        virtual results exec_commands(commands) noexcept=0;
        virtual drawable* get_drawable() noexcept=0;
        virtual results update_drawable() noexcept=0;
    };
    class dll_adapter
    {
        public:
        dll_adapter()=default;
        dll_adapter(const dll_adapter&)=delete;
        virtual ~dll_adapter()=default;
        virtual status get_state() const noexcept=0;
        virtual results load_dll(const std::string&) noexcept=0;
        virtual results free_dll() noexcept=0;
        virtual platform_adapter* get_platform_adapter() noexcept=0;
    };
}