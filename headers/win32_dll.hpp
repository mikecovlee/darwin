#pragma once
namespace darwin
{
    class win32_dll_adapter:public dll_adapter{
        public:
        win32_dll_adapter();
        virtual ~win32_dll_adapter();
        virtual status get_state() const noexcept override;
		virtual results load_dll(const std::string&) noexcept override;
		virtual results free_dll() noexcept override;
		virtual platform_adapter* get_platform_adapter() noexcept override;
    } dwin32dll;
    static darwin runtime(&dwin32dll);
}