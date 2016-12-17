#pragma once
#include "./adapter.hpp"
namespace darwin
{
    platform_adapter* dll_resource();
}
extern "C"
{
    darwin::platform_adapter* COV_DARWIN_DLL_MAIN()
    {
        return darwin::dll_resource();
    }
}