/*! 
 * Test suite for the CS225 event-driven programming assignment.
 *
 * Contains the combined collection of tests for the assignment. 
 * Use this if you find it convenient to work with a single file that contains all the driver code.
 *
 * Modifications to this file are allowed only for development convenience (e.g. commenting out code
 * that fails to compile). This file will be used as provided during the grading process, so make sure
 * that your solution does not rely on any modification done on this file.
 *
 * Students are responsible for all code elements that live in the `cs225` namespace. These should 
 * be declared in the header files as hinted by the inclussion directives.
 *
 * If you want to disable a test, just comment it out. Bear in mind that the `TEST` macro makes it 
 * necessary to compile and run the test code, so both compilation and run time errors should
 * be taken into account.
 *
 * @author  Iker Silvano
 *
 */

#pragma once

// common headers
#include "testing.hh" // testing framework (ASSERT_THAT, etc.)
using namespace testing;

// ===========================================================================
// ===========================================================================
// ===========================================================================

#include "type_info.hh"  // cs225::TypeInfo, cs225::type_of

#include <map>          // std::map

/*********************************************************************
 *                          Type info tests                          *
 *********************************************************************/

namespace Tests { namespace TypeInfo
{

/*******************************
 *  dummy classes for testing  *
 *******************************/

struct Widget {};
struct Gadget {};


/***********
 *  tests  *
 ***********/

// [ Test #1 ] -------------------------------------------------------
TEST( "TypeInfo instantiation",
      "Testing TypeInfo class instantiability" )
{
    Widget widget;
    cs225::TypeInfo instance_info( widget );
    cs225::TypeInfo class_info( typeid(Widget) );

    ASSERT_THAT( instance_info.get_name() == typeid(widget).name() );
    ASSERT_THAT( class_info.get_name() == typeid(Widget).name() );
}

// [ Test #2 ] -------------------------------------------------------
TEST( "TypeInfo comparison",
      "TypeInfo should provide comparison operations to test for equality" )
{
    Widget widget;
    Gadget gadget;
    cs225::TypeInfo widget_info( widget );
    cs225::TypeInfo gadget_info( gadget );

    ASSERT_THAT( widget_info == cs225::TypeInfo( typeid(Widget) ) );
    ASSERT_THAT( widget_info != gadget_info );
}

// [ Test #3 ] -------------------------------------------------------
TEST( "TypeInfo ordered container compatibility",
      "TypeInfo instances should be compatible with ordered containers in the STL (you may have to check the requirements for this)" )
{
    // ordered container that tracks the number of instantiations per type
    std::map<cs225::TypeInfo, int> instance_count;

    // every time that an instance is created the counter for its type is increased
    Widget w1;
    instance_count[ cs225::type_of(w1) ]++;
    Gadget g1;
    instance_count[ cs225::type_of(g1) ]++;
    Widget w2;
    instance_count[ cs225::type_of(w2) ]++;
    int i;
    (void)i; // to supress the warning
    instance_count[ cs225::type_of<int>() ]++;

    ASSERT_THAT( instance_count[ cs225::type_of<Widget>() ] == 2 );
    ASSERT_THAT( instance_count[ cs225::type_of<Gadget>() ] == 1 );
    ASSERT_THAT( instance_count[ cs225::type_of<int>() ]    == 1 );
}

} // namespace TypeInfo
} // namespace Tests




// ===========================================================================
// ===========================================================================
// ===========================================================================




#include "event.hh" // cs225::Event, cs225::HandlerFunction, cs225::MemberFunctionHandler, cs225::EventHandler

/*********************************************************************
 *                            Event tests                            *
 *********************************************************************/

namespace Tests { namespace Events
{

struct DummyEvent : public cs225::Event {};

// [ Test #4 ] -------------------------------------------------------
TEST( "Polymorphism in event classes",
      "Classes that belong in the event class hierarchy should be polymorphic" )
{
    cs225::Event * polymorphic_event = new DummyEvent;

    ASSERT_THAT( cs225::type_of(*polymorphic_event) == cs225::type_of<DummyEvent>() );

    delete polymorphic_event;
}


// this is a dummy variable to simulate the side effects of a function being called
bool dummy_handler_code_executed = false;

struct DummyHandler : public cs225::HandlerFunction
{
private:
    void call( const cs225::Event & )
    {
        dummy_handler_code_executed = true;
    }
};

// [ Test #5 ] -------------------------------------------------------
TEST( "Override event handling in concrete handler function classes",
      "Concrete handler functions should provide a custom logic to deal with events. Use the NVI idiom." )
{
    DummyHandler concrete_handler;
    DummyEvent concrete_event;

    // passing an event to the handler function should trigger the logic defined by DummyHandler::call
    concrete_handler.handle( concrete_event );

    ASSERT_THAT( dummy_handler_code_executed );
}

// this is a dummy variable to simulate the side effects of a function being called
// the index of the array is associated to the state of a Dude instance
bool dude_member_handler_code_executed[8] = {false};

// Dummy class with state (has member data) for testing purposes
struct Dude
{
    Dude( int s = 0 )
        : state(s) {}

    void handle_it( const DummyEvent & )
    {   // trigger some side effect that depends on the state of the instance
        dude_member_handler_code_executed[state] = true;
    }

private:
    int state; // dummy variable that simulates some object's state
};


// [ Test #6 ] -------------------------------------------------------
TEST( "Member function handler initialization",
      "Concrete member function handlers require both parts involved in a call through a pointer to member function: the pointer to the member function and the instance to call it through." )
{
    Dude dude;
    cs225::MemberFunctionHandler<Dude, DummyEvent> dude_dummy_event_handler( &dude, &Dude::handle_it );

    // nothing to check, just make sure that the class supports this semantics and the code reaches this point
    SUCCEED();
}


// [ Test #7 ] -------------------------------------------------------
TEST( "Member function handler manual event dispatching",
      "Concrete member function handler should delegate to the registered member function call through the specific class instance that was provided on instantiation." )
{
    Dude dude_1( 1 );
    Dude dude_3( 3 );
    Dude dude_6( 6 );
    // the member function handler is associated with dude_3 instance specifically
    cs225::MemberFunctionHandler<Dude, DummyEvent> dude_dummy_event_handler( &dude_3, &Dude::handle_it );

    // spawning a DummyEvent should only affect the associated object (dude_3)
    dude_dummy_event_handler.handle( DummyEvent() );

    // make sure that it was dude_3 who invoked the handler function and none of the other instances
    ASSERT_THAT( dude_member_handler_code_executed[3] );
    ASSERT_THAT( !dude_member_handler_code_executed[1] && !dude_member_handler_code_executed[6]  );
}


// class that represents on-screen 2D coordinate information
struct Coordinates
{
    Coordinates( int xx, int yy )
        : x(xx), y(yy) {}

    bool operator == ( const Coordinates & other ) const
    {
        return (x == other.x) && (y == other.y);
    }

    int x, y;
};

// a dummy concrete event with associated state
// represents a mouse clicking event, with the associated screen coordinates where the clicking happened
struct MouseClickedEvent : public cs225::Event
{
    MouseClickedEvent( int xx, int yy )
        : position(xx, yy) {}

    Coordinates position; // mouse coordinates
};

// a dummy class with internal state (position, selected flag) that will handle a mouse clicked event
// represents a ground unit in some RTS game (its logic is self-explanatory)
struct InfantryUnit
{
    InfantryUnit()
        : is_selected(false)
        , position(0, 0)
    {}

    void select() { is_selected = true; }

    // member function to be used as the MouseClickedEvent handling logic
    void move_to( const MouseClickedEvent & mouse )
    {
        if( is_selected )
            position = mouse.position;
    }

    const Coordinates & get_position() const { return position; }

private:
    bool is_selected; // flag to determine if the unit is currently selected
    Coordinates position;
};

// [ Test #8 ] -------------------------------------------------------
TEST( "Member function handlers interacting with stateful objects and events",
      "Objects and events that are associated to member function handlers may have internal state. Make sure that handlers behave as expected considering these variables." )
{
    InfantryUnit soldier;
    // unit_mover will take care of handling soldier-related events
    cs225::MemberFunctionHandler<InfantryUnit, MouseClickedEvent> unit_mover( &soldier, &InfantryUnit::move_to );

    soldier.select(); // if there are other InfantryUnits interested in the same events they need to be selected to respond
    // spawn a MouseClickedEvent that should move the soldier to the specific coordinates
    unit_mover.handle( MouseClickedEvent(20, 83) );

    ASSERT_THAT( soldier.get_position() == Coordinates(20, 83) );
}
/*
// Event that simulates having a GUI button clicked
struct ButtonClickedEvent : cs225::Event
{
    enum ButtonId { play, stop };

    ButtonClickedEvent( ButtonId id )
        : button_id(id) {}

    ButtonId button_id;
};

// Dummy class that simulates an application with a GUI interface
// The application provides some dummy logic to handle events that are triggeres through (fake) mouse interaction
struct App
{
    App()
        : is_playing(false)
        , is_focused(false)
    {}

    void on_mouse_clicked( const MouseClickedEvent & )
    {
        is_focused = true;
    }

    void on_button_clicked( const ButtonClickedEvent & button )
    {
        if( !is_focused ) return; // do nothing if the application is not in focus

        switch( button.button_id )
        {
            case ButtonClickedEvent::play : is_playing = true;
                break;
            case ButtonClickedEvent::stop : is_playing = false;
                break;

            // maybe handle other button types
        }
    }

    bool is_playing;
    bool is_focused;
};

// [ Test #9 ] -------------------------------------------------------
TEST( "Event handler multiple handler function registration",
      "An event handler registers a collection of event handlers. Upon event dispatching, the event handler should invoke the associated handler function (the one matching the instance-event pair)." )
{
    // the application event handler is an object whose sole 
    // responsibility is to capture and respond to app-related events
    cs225::EventHandler app_event_handler;
    // the event handler requires an object to relate to, to deliver notifications
    App my_app;

    // through this, the event handler will subscribe to the event type that the handler expects
    app_event_handler.register_handler( my_app, &App::on_button_clicked );
    app_event_handler.register_handler( my_app, &App::on_mouse_clicked );
    // registering duplicate event handlers has no effect
    app_event_handler.register_handler( my_app, &App::on_mouse_clicked );

    // when triggered, the event handler will have its object invoke the event handler with the event
    app_event_handler.handle( ButtonClickedEvent(ButtonClickedEvent::play) );
    // handle the event but do nothing since the app is not yet focused
    ASSERT_THAT( my_app.is_playing == false );

    // spawning a MouseClickedEvent puts the app in focus
    app_event_handler.handle( MouseClickedEvent(0, 0) ); // mouse coordinates are irrelevant
    ASSERT_THAT( my_app.is_focused );
    ASSERT_THAT( my_app.is_playing == false );

    // spawning a ButtonClickedEvent with play id puts the app in play mode (since it's already focused)
    app_event_handler.handle( ButtonClickedEvent(ButtonClickedEvent::play) );
    ASSERT_THAT( my_app.is_playing );

    // spawning a ButtonClickedEvent with stop id disables the play mode
    app_event_handler.handle( ButtonClickedEvent(ButtonClickedEvent::stop) );
    ASSERT_THAT( my_app.is_playing == false );
}

} // namespace Events
} // namespace Tests
*/

// ===========================================================================
// ===========================================================================
// ===========================================================================


#include "event_dispatcher.hh" // cs225::Listener, cs225::EventDispatcher, cs225::trigger_event

/*********************************************************************
 *                      Event dispatcher tests                       *
 *********************************************************************/
/*
namespace Tests { namespace EventDispatcher
{

bool dummy_listener_handle_event_called = false;

// dummy classes for testing purposes
struct DummyEvent : public cs225::Event {};
struct DummyListener : public cs225::Listener
{
    virtual void handle_event( const cs225::Event & )
    {
        dummy_listener_handle_event_called = true;
    }
};

// [ Test #10 ] -------------------------------------------------------
// testing the inheritance requirements for event listeners
TEST( "Event listener classes implement the Listener interface",
      "All classes that are intended to act as event listeners (respond to events asynchronously) should implement a common interface." )
{
    cs225::Listener * dummy_listener = new DummyListener;

    dummy_listener->handle_event( DummyEvent() );
    ASSERT_THAT( dummy_listener_handle_event_called );

    delete dummy_listener;
}

// [ Test #11 ] -------------------------------------------------------
// testing the singleton semantics for the event dispatcher
TEST( "Event dispatcher implements the singleton pattern",
      "Event dispatcher is not directly instantiable, a static instance is accessed through the appropriate public function." )
{
    // this syntax should be illegal
    // cs225::EventDispatcher event_dispatcher;

    // this is the proper way to access the event dispatcher
    cs225::EventDispatcher & event_dispatcher = cs225::EventDispatcher::get_instance();

    SUCCEED();
}

// more dummy classes for testing purposes
struct EverythingIsOnFireEvent : public cs225::Event {};
struct MyPhoneIsVibratingEvent : public cs225::Event {};
struct LammyListener : public cs225::Listener
{   // to make it non-abstract
    virtual void handle_event( const cs225::Event & ) {}
};

// [ Test #12 ] -------------------------------------------------------
// testing the subscription and the debug-printing mechanisms in the event dispatcher
TEST( "Objects can subscribe to different event types through the event dispatcher",
      "On subscription, the event dispatcher should add an entry to the corresponding collection of event subscribers. A printing operation might come in handy for debugging purposes." )
{
    cs225::EventDispatcher & event_dispatcher = cs225::EventDispatcher::get_instance();

    DummyListener dl_1, dl_2;
    LammyListener ll_1, ll_2;
    DummyEvent event;

    // subscribe a bunch of dummy objects to specific dummy events
    event_dispatcher.subscribe( dl_1, cs225::type_of(event) );
    event_dispatcher.subscribe( dl_1, cs225::type_of<EverythingIsOnFireEvent>() );
    event_dispatcher.subscribe( dl_2, cs225::type_of<DummyEvent>() );
    event_dispatcher.subscribe( ll_1, cs225::type_of<EverythingIsOnFireEvent>() );
    event_dispatcher.subscribe( ll_1, cs225::type_of<MyPhoneIsVibratingEvent>() );
    event_dispatcher.subscribe( ll_2, cs225::type_of( MyPhoneIsVibratingEvent() ) );

    // We'll print the event dispatcher description into a stringstream to be able to compare its output with the expectation
    std::stringstream ss;
    ss << event_dispatcher;
    const std::string my_output = ss.str();

    // this is what the output operation above should produce when using GCC
    // the strings for the type names should be generated by the compiler
    // (a custom string approach is possible, but we don't really mind here, as long as strings are unique)
    // this test will only pass if your event dispatcher printing operation output matches this exactly (again, only applicable when using GCC)
    const std::string expected_event_dispatcher_inspection_output =
        "The event type N5Tests15EventDispatcher10DummyEventE has the following subscribers:\n"
        "\tAn instance of type N5Tests15EventDispatcher13DummyListenerE\n"
        "\tAn instance of type N5Tests15EventDispatcher13DummyListenerE\n"
        "The event type N5Tests15EventDispatcher23EverythingIsOnFireEventE has the following subscribers:\n"
        "\tAn instance of type N5Tests15EventDispatcher13DummyListenerE\n"
        "\tAn instance of type N5Tests15EventDispatcher13LammyListenerE\n"
        "The event type N5Tests15EventDispatcher23MyPhoneIsVibratingEventE has the following subscribers:\n"
        "\tAn instance of type N5Tests15EventDispatcher13LammyListenerE\n"
        "\tAn instance of type N5Tests15EventDispatcher13LammyListenerE\n";

    ASSERT_THAT( my_output == expected_event_dispatcher_inspection_output );
}

// [ Test #13 ] -------------------------------------------------------
// testing the manual cleanup mechanism for the event dispatcher
TEST( "Event dispatcher cleanup",
      "Because of the singleton nature of the event dispatcher, a way of manually cleaning up its contents is required. The destructor will only be called at the end of the program." )
{
    cs225::EventDispatcher & event_dispatcher = cs225::EventDispatcher::get_instance();
    // the event dispatcher has to be cleaned up manually, since the singleton pattern makes its state global and persistent
    // we'll have to call this at the end of every test, or otherwise the subscription list will be carried on to the next test
    event_dispatcher.clear();

    // debug-print the contents to inspect the current state of the event dispatcher
    std::stringstream ss;
    ss << event_dispatcher;
    const std::string my_output = ss.str();

    // after cleaning up the debug-print function should print an empty message
    ASSERT_THAT( my_output == "" );
}

// Dummy listener that counts the number of event notifications
struct EventListenerWithCounter : public cs225::Listener
{
    EventListenerWithCounter()
        : times_called(0u) {}

    virtual void handle_event( const cs225::Event & )
    {
        times_called++;
    }
    unsigned int times_called;
};

// [ Test #14 ] -------------------------------------------------------
// testing the event triggering mechanism in the event dispatcher
TEST( "Event dispatcher triggers events and notifies subscribers",
      "Event dispatcher should notify all the entries in the corresponding subscriber collection for the event that was triggered." )
{
    cs225::EventDispatcher & event_dispatcher = cs225::EventDispatcher::get_instance();
    EventListenerWithCounter counted_listener_1, counted_listener_2;

    event_dispatcher.subscribe( counted_listener_1, cs225::type_of<DummyEvent>() );
    event_dispatcher.subscribe( counted_listener_2, cs225::type_of<DummyEvent>() );

    // the trigger method acts as the entry point for event dispatching
    // upon triggering, the event dispatcher notifies all the subscribers in the corresponding listener collection
    event_dispatcher.trigger_event( DummyEvent() );

    ASSERT_THAT( counted_listener_1.times_called == 1u );
    ASSERT_THAT( counted_listener_2.times_called == 1u );

    // trigger a second event
    event_dispatcher.trigger_event( DummyEvent() );

    ASSERT_THAT( counted_listener_1.times_called == 2u );
    ASSERT_THAT( counted_listener_2.times_called == 2u );

    event_dispatcher.clear();
}

// [ Test #15 ] -------------------------------------------------------
// testing a proxy function to trigger events in a more convenient way
TEST( "Events can be triggered though a proxy function",
      "Using a proxy function hides away the internal details of the event dispatcher and results in a cleaner syntax." )
{
    EventListenerWithCounter counted_listener_1, counted_listener_2;

    cs225::EventDispatcher::get_instance().subscribe( counted_listener_1, cs225::type_of<DummyEvent>() );
    cs225::EventDispatcher::get_instance().subscribe( counted_listener_2, cs225::type_of<DummyEvent>() );

    // this kind of syntax, while valid, is too verbose and inconvenient
    cs225::EventDispatcher::get_instance().trigger_event( DummyEvent() );

    ASSERT_THAT( counted_listener_1.times_called == 1u );
    ASSERT_THAT( counted_listener_2.times_called == 1u );

    // use this proxy function to the same effect
    cs225::trigger_event( DummyEvent() );

    ASSERT_THAT( counted_listener_1.times_called == 2u );
    ASSERT_THAT( counted_listener_2.times_called == 2u );

    cs225::EventDispatcher::get_instance().clear();
}


// [ Test #16 ] -------------------------------------------------------
TEST( "Objects can unsubscribe from events they were subscribed to",
      "On unsubscription the event dispatcher removes the entry from the collection of subscribers for the specified event type. Beyond that point, the object should stop being notified about the event type it unsubscribed from." )
{
    EventListenerWithCounter counted_listener;
    cs225::EventDispatcher & event_dispatcher = cs225::EventDispatcher::get_instance();
    event_dispatcher.subscribe( counted_listener, cs225::type_of<DummyEvent>() );

    cs225::trigger_event( DummyEvent() );
    cs225::trigger_event( DummyEvent() );

    ASSERT_THAT( counted_listener.times_called == 2u );

    // the listener is unsubscribed from the event
    event_dispatcher.unsubscribe( counted_listener, cs225::type_of<DummyEvent>() );
    // the listener is no longer notified about said event type
    cs225::trigger_event( DummyEvent() );

    ASSERT_THAT( counted_listener.times_called == 2u );

    event_dispatcher.clear();
}


} // namespace EventDispatcher
} // namespace Tests
*/
