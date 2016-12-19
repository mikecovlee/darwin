#pragma once
namespace darwin {
	class win32_module_adapter:public module_adapter {
	public:
		win32_module_adapter();
		virtual ~win32_module_adapter();
		virtual status get_state() const noexcept override;
		virtual results load_module(const std::string&) noexcept override;
		virtual results free_module() noexcept override;
		virtual platform_adapter* get_platform_adapter() noexcept override;
	} dwin32module;
	static darwin runtime(&dwin32module);
}