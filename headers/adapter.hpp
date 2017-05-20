#pragma once
#include "./core.hpp"
namespace darwin {
	class platform_adapter {
	public:
		platform_adapter()=default;
		platform_adapter(const platform_adapter&)=delete;
		virtual ~platform_adapter()=default;
		virtual void init()=0;
		virtual void stop()=0;
        virtual bool is_kb_hit() const=0;
        virtual int get_kb_hit() const=0;
        virtual void update(const canvas&)=0;
		virtual std::size_t get_width() const=0;
        virtual std::size_t get_height() const=0;
	};
	typedef platform_adapter*(*module_enterance)();
	const char* module_enterance_name="COV_DARWIN_MODULE_MAIN";
}
