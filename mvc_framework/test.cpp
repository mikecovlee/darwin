#define DARWIN_FORCE_BUILTIN
#include "./mvc_framework.cpp"
#include <array>
std::array<int,2> ball{0,0};
void darwin::on_load()
{
    keyboard.add_listener('w',[&]()->bool{
        --ball[1];
        return true;
    });
    keyboard.add_listener('s',[&]()->bool{
        ++ball[1];
        return true;
    });
    keyboard.add_listener('a',[&]()->bool{
        --ball[0];
        return true;
    });
    keyboard.add_listener('d',[&]()->bool{
        ++ball[0];
        return true;
    });
}
void darwin::on_draw(darwin::drawable* paint)
{
    paint->draw_pixel(ball[0],ball[1],pixel('@',true,false,colors::white,colors::black));
}