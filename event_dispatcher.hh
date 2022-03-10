#pragma once

namespace cs225
{
    class Listener
    {
    public:
        virtual ~Listener() {}
        virtual void handle_event(const Event&) = 0;
    };

    class EventDispatcher
    {
    public:
        static EventDispatcher& get_instance()
        {
            return instance;
        }
    private:
        static EventDispatcher instance;
    };

    EventDispatcher EventDispatcher::instance;
}