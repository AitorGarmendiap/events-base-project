#pragma once

namespace cs225
{
    class Event
    {
    public:
        // this makes the class polymorphic
        virtual ~Event() {}
    };

    class HandlerFunction
    {
    public:
        void handle(const Event& event)
        {
            call(event);
        }
    private:
        virtual void call(const Event&) = 0;
    };

    template <typename T, typename E>
    class MemberFunctionHandler
    {
        
    }
}