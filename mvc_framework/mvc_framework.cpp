#include "./mvc_framework.hpp"
void darwin::main()
{
    using namespace darwin;
    std::string module_path="./darwin.module";
    std::size_t frame_limit=30;
    bool clear_drawable=true;
    bool always_fit_drawable=true;
    bool auto_update_drawable=true;
    config c("./darwin.config");
    if(c.usable())
    {
        c.read();
        module_path=c.get_value("module_path");
        frame_limit=std::stol(c.get_value("frame_limit"));
        if(c.get_value("clear_drawable")=="true")
            clear_drawable=true;
        if(c.get_value("clear_drawable")=="false")
            clear_drawable=false;
        if(c.get_value("always_fit_drawable")=="true")
            always_fit_drawable=true;
        if(c.get_value("always_fit_drawable")=="false")
            always_fit_drawable=false;
        if(c.get_value("auto_update_drawable")=="true")
            auto_update_drawable=true;
        if(c.get_value("auto_update_drawable")=="false")
            auto_update_drawable=false;
    }
    runtime.load(module_path);
    runtime.fit_drawable();
    darwin::on_load();
    sync_clock clock(frame_limit);
    while(true)
    {
        clock.reset();
        if(runtime.is_kb_hit())
            keyboard.active(runtime.get_kb_hit());
        if(always_fit_drawable)
            runtime.fit_drawable();
        if(clear_drawable)
            runtime.get_drawable()->clear();
        darwin::on_draw(runtime.get_drawable());
        if(auto_update_drawable)
            runtime.update_drawable();
        clock.sync();
    }
}
int main()
{
    darwin::main();
    return 0;
}