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
    public:
        using MemberFunction = void (T::*)(const E&);
        MemberFunctionHandler(T* obj, MemberFunction mf) : instance{obj}, handler_fn{mf} {}
        void handle(const E& event)
        {
            (instance->*handler_fn)(event);
        }
    private:
        T* instance;
        MemberFunction handler_fn;
    }
}