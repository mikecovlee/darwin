#pragma once
#include "../headers/darwin.hpp"
#include "./config.hpp"
#include <unordered_map>
#include <functional>
#include <deque>
namespace darwin{
    void main();
    void on_load();
    void on_draw(drawable*);
    using keyboard_event_listener=std::function<bool()>;
    class keyboard_event final{
        friend void darwin::main();
        std::unordered_map<char,std::deque<keyboard_event_listener>> mMap;
        void active(char key)
        {
            if(mMap.find(key)!=mMap.end())
            {
                for(auto& func:mMap.at(key))
                {
                    if(func())
                        break;
                }
            }
        }
    public:
        keyboard_event()=default;
        keyboard_event(const keyboard_event&)=delete;
        ~keyboard_event()=default;
        void add_listener(char key,const keyboard_event_listener& listener)
        {
            mMap[key].push_front(listener);
        }
    } keyboard;
}