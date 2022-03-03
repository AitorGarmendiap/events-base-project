/*! 
 * Test driver entry point for the event-driven programming CS225 assignment. 
 * 
 * @author  Iker Silvano
 *
 */
 
#include "testing.hh" // testing framework (ASSERT_THAT, etc.)
using namespace testing;

// suppress unused parameter/variable warnings to avoid annoying diagnostics
// ** don't do this unless you have a good reason to (which I do) **
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wunused-local-typedefs"

/**********************************
 *  Driver code inclussion point  *
 **********************************/

// Combined driver tests
#include "test_suite.hh"

// restore the original diagnostics settings
#pragma GCC diagnostic pop

#include <iostream>     // std::cout, std::endl
#include <string>       // std::string
#include <vector>       // std::vector
#include <exception>    // std::exception
#include <stdexcept>    // std::out_of_range, std::invalid_argument


/*********************************************************************
 *                           Main function                           *
 *********************************************************************/

void print_instructions()
{
    static const std::string instructions_message
    (
       "Usage instructions: <program-executable> [-h|--help|1-16]\n"
       "  - calling the program with no parameters will run all the registered tests\n"
       "  - passing a number as a parameter will run the specified test (0-indexed).\n"
       "  - The -h and --help flags display this message.\n\n"
       #ifdef VERBOSE
           "Program compiled with the verbosity flag. A message will be printed to stdout describing the test execution status.\n"
       #else
           "Program compiled without verbosity. The program will silently complete with no feedback, as long as there are no errors.\n"
       #endif
    );
    std::cout << instructions_message << std::endl;
}

int main( int argc, const char** argv )
{
    try
    {
        // if no parameter supplied -> run all tests
        if( argc == 1 )
        {
            TestSuite::run_all();

            #ifdef VERBOSE
                print("\n--- All tests completed successfully! ---\n", colors::green);
            #endif
        }

        else if( argc == 2 ) // one parameter
        {
            std::string param = argv[1];

            // asking for help
            if( param == "-h" || param == "--help" )
                print_instructions();

            else // a specific test is requested
            {
                // parse the command line parameter as a number
                std::size_t test_index = std::atoi( param.c_str() ) - 1u;

                // run the requested test (will throw if out of range)
                TestSuite::run_test(test_index);
            }
        }
        else // driver program only allows 0 or 1 argument
        {
            print("Wrong usage: too many parameters provided\n", colors::red);
            print_instructions();
        }

    } catch( const TestFailed& fail ) { // if a running test fails, the program interrupts its execution
                                        // and a detailed message about the failure conditions is printed
        std::ostringstream oss;
        oss << "Test "    << fail.which()  << " failed";
        print( oss.str(), colors::red );

        std::cout << "\nReason: ";
        print( fail.why(), colors::yellow );
        
        std::cout << "\nCheck the test description: ";
        print( fail.tip(), colors::yellow );

    } catch( const std::exception& ex ) {
    	std::ostringstream oss;
        oss << "Runtime error exception caught: \n";
        oss << ex.what() << std::endl << std::endl;
        print( oss.str(), colors::red );
    } catch(...) {
    	print( "An unknown exception was thrown. This should never happen\n", colors::red );
    }

}

