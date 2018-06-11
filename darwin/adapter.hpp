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
#include "./core.hpp"
#include <functional>

namespace darwin {
	class platform_adapter {
	public:
		platform_adapter() = default;

		platform_adapter(const platform_adapter &) = delete;

		virtual ~platform_adapter() = default;

		virtual status get_state() const =0;

		virtual results init()=0;

		virtual results stop()=0;

		virtual results exec_commands(commands)=0;

		virtual bool is_kb_hit()=0;

		virtual int get_kb_hit()=0;

		virtual results fit_drawable()=0;

		virtual drawable *get_drawable()=0;

		virtual results update_drawable()=0;
	};

	class module_adapter {
	public:
		module_adapter() = default;

		module_adapter(const module_adapter &) = delete;

		virtual ~module_adapter() = default;

		virtual status get_state() const =0;

		virtual results load_module(const std::string &)=0;

		virtual results free_module()=0;

		virtual platform_adapter *get_platform_adapter()=0;
	};

	typedef platform_adapter *(*module_enterance)();

	const char *module_enterance_name = "COV_DARWIN_MODULE_MAIN";
}