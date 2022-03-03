
/*! 
 *  Collection of testing mechanisms to support CS225 assignments.
 *
 *  Students need not understand the contents of this file.
 *  These are the relevant parts that students should be aware of:
 *
 *    - The macro `TEST( "description", "hint" )` automatically creates a unit test
 *      and registers it for execution. The first string contains a brief description
 *      of the behavior the test is meant to drive. The second string is a detailed 
 *      hint to guide the implementation.
 *
 *    - The macro `ASSERT_THAT(expr)` evaluates the expression `expr`. If it evaluates
 *      as true it silently falls through; if it evaluates as false the test execution 
 *      will throw an exception that will interrupt the test suite execution and print
 *      a detailed failure message. The program will stop if a failing assertion is 
 *      encountered.
 *      
 *    - The macro `SUCCEED()` is equivalent to `ASSERT_THAT(true)`. It serves as an 
 *      indication that the unit test is successful once it's reached. This is used 
 *      when a regular assertion is not appropriate.
 *
 *    - The macro `FAIL()` is the counterpart to `SUCCEED()`. It is used to mark a 
 *      path of execution or point in the test as incorrect (i.e. if a test reaches 
 *      a line with the `FAIL()` statement it means that it will fail for reaching a 
 *      point that it's not supposed to).
 *
 *  The rest of the testing framework implementation consists on either implementation 
 *  details or facilities to run the tests that will be used from the main function
 *  (which is provided as well and students need not touch).
 *
 *  The `VERBOSE` macro, if defined, will make the test running routines to print verbose 
 *  information about the running state of each test (mainly, it will display a message
 *  indicating test execution success, for verification purposes). 
 *  This is not active by default (so test execution will have no effect unless it fails).
 *  It can be activated either by defining the a macro named `VERBOSE` or, preferably, by 
 *  specifying the compiler flag `-DVERBOSE` on the command line or the makefile.
 *
 *  @author  Iker Silvano
 */
 

#pragma once


// print colored output messages
// this should be set from the command line with the -DUSE_COLORED_OUTPUT flag
//#define USE_COLORED_OUTPUT
#include "output_coloring.hh"

#include <algorithm>    // std::for_each
#include <iostream>     // std::cout, std::cerr, std::endl
#include <sstream>      // std::ostringstream
#include <string>       // std::string
#include <vector>       // std::vector

// print verbose messages about test execution
// this should be set from the command line with the -DVERBOSE flag
//#define VERBOSE

namespace testing
{

class Test
{
public:
    typedef void (*function_type)();

    Test( function_type fn, const std::string& nm, const std::string& desc = "" )
        : func( fn ) 
        , test_name( nm )
        , test_description( desc )
    {}

    void run() const { func(); }
    const std::string& description() const { return test_description; }
    const std::string& name() const        { return test_name; }

private:
    function_type func;
    std::string test_name;
    std::string test_description;

}; // Test


struct AssertionFailed
{
    explicit AssertionFailed( const std::string& msg )
        : reason( msg ) {}

    std::string reason;

}; // AssertionFailed

class TestFailed 
{
public:
    TestFailed( const Test& test, const std::string& msg )
        : failed_test( test ) 
        , message( msg )
    {}

    const std::string& which() const { return failed_test.name(); }
    const std::string& why() const   { return message; }
    const std::string& tip() const   { return failed_test.description(); }

private:
    const Test& failed_test;
    std::string message;

}; // TestFailed

void run( const Test& test )
{
    try
    {
        #ifdef VERBOSE
            std::cout << "Running: '" << test.name() << "' test\n";
        #endif

        test.run();

    } catch( const AssertionFailed& af ) {
        throw TestFailed( test, af.reason );
    }

    #ifdef VERBOSE
        std::ostringstream oss;
        oss << "Test '" << test.name() << "' completed successfully\n";
        print(oss.str(), colors::green);
    #endif
}

class TestSuite 
{
public:
    static bool register_test( Test::function_type fn, const char* name, const char* description = "" )
    {
        registered_tests.push_back( Test(fn, name, description) );
        return true;
    }

    static void run_all()
    {
        std::for_each( registered_tests.begin(), registered_tests.end(), ::testing::run );
    }

    static void run_test( std::size_t index )
    {
        ::testing::run( registered_tests.at(index) );
    }

private:
    static std::vector<Test> registered_tests;

}; // TestSuite

// static initialization
// caveat: this being instantiated here implies that there shouldn't
// be more than one translation unit including this header
std::vector<Test> TestSuite::registered_tests;

} // namespace testing


#define ASSERT_THAT(expression)                                         \
    if( !(expression) )                                                 \
    {                                                                   \
        std::ostringstream ss;                                          \
        ss << "The following assertion failed at function "             \
           << __PRETTY_FUNCTION__                                       \
           << ", line " << __LINE__ << std::endl                        \
           << "Expression: " << #expression;                            \
        throw AssertionFailed( ss.str() );                              \
    }

#define SUCCEED()                                                       \
    return;

#define FAIL()                                                          \
    std::ostringstream ss;                                              \
    ss << "This point should not be reached: "                          \
       << __PRETTY_FUNCTION__                                           \
       << ", line " << __LINE__;                                        \
    throw AssertionFailed( ss.str() );

#define CONCAT_IMPL(x,y) x ## y
#define CONCAT(x,y) CONCAT_IMPL(x,y)

#define REGISTER_TEST_IMPL( fn_name, var_name, name, description )      \
    static void fn_name();                                              \
    static volatile bool var_name = testing::TestSuite::register_test(fn_name, name, description); \
    void fn_name()

#define REGISTER_TEST( name, description, line )                        \
    REGISTER_TEST_IMPL( CONCAT(test_fn_, line), CONCAT(sink_, line), name, description )

#define TEST( name, description )                                       \
    REGISTER_TEST( name, description, __LINE__ )

