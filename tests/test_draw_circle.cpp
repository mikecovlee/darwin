#include "../headers/darwin.hpp"
void draw_circle(darwin::drawable*,const std::array<std::size_t,3>&,const darwin::pixel&);
int main()
{
	darwin::runtime.load("./test.dll");
	auto adapter=darwin::runtime.get_adapter();
	auto pic=adapter->get_drawable();
	darwin::pixel p(' ', {true,false}, {darwin::colors::black,darwin::colors::white});
	darwin::sync_clock clock(30);
	while(true) {
		clock.reset();
		adapter->fit_drawable();
		p.set_char(' ');
		pic->fill(p);
		draw_circle(pic,{
			static_cast<std::size_t>(0.5*pic->get_width()),
			static_cast<std::size_t>(0.5*pic->get_height()),
			pic->get_width()>=pic->get_height()?
			static_cast<std::size_t>(0.5*pic->get_height()):static_cast<std::size_t>(0.5*pic->get_width())},
			p);
		adapter->update_drawable();
		clock.sync();
	}
	return 0;
}