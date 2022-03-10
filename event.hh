#include <map>

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
    class MemberFunctionHandler : public HandlerFunction
    {
    public:
        using MemberFunction = void (T::*)(const E&);
        MemberFunctionHandler(T* obj, MemberFunction mf) : instance{obj}, handler_fn{mf} {}
    private:
        void call(const Event& event) override
        {
            (instance->*handler_fn)(static_cast<const E&>(event);
        }

        T* instance;
        MemberFunction handler_fn;
    };

    class EventHandler
    {
    public:
        template <typename T, typename E>
        void register_handler(T& instance, void (T::*handler_method)(const E&))
        {
            // insert the type-of-event, handler pair into the map
            TypeInfo type = type_of<E>();
            // avoid this if the entry exists
            auto found_it = handler_map.find(type_of(event));
            if (found_it != handle_map.end())
                return;
                
            HandlerFunction *handler = new MemberFunctionHandler<T, E> {&instance, handler_method};
            handler_map.insert(std::make_pair(type, handler));
        }
        ~EventHandler()
        {
            for (auto& entry : handler_map)
                delete entry.second;
        }

        // dispatch a specific event to the appropiate handler
        void handle(const Event& event)
        {
            // find the handler in the map
            auto found_it = handler_map.find(type_of(event));
            // invoke the handler passing the event parameter
            if (found_it != handle_map.end())
                found_it->second->handle(event);
        }
    private:
        std::map<TypeInfo, HandlerFunction*> handle_map;
    };
}