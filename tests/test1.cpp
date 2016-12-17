#include "../headers/darwin.hpp"
#include <string>
int main()
{
    darwin::runtime.load("./test.dll");
    while(darwin::runtime.get_state()==darwin::status::busy);
    auto adapter=darwin::runtime.get_adapter();
    auto pic=adapter->get_drawable();
    std::string greeting="Hello,World!";
    darwin::pixel p(' ',{true,false},{darwin::colors::red,darwin::colors::white});
    while(true)
    {
        adapter->fit_drawable();
        pic->fill(p);
        for(std::size_t x=0;x<greeting.size()&&x<pic->get_width();++x)
        {
            p.set_char(greeting[x]);
            pic->draw_pixel({x,0},p);
        }
        adapter->update_drawable();
    }
    return 0;
}