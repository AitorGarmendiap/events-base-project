/*! 
 *  Command line output coloring utility.
 * 
 *  This file contains a minimal routine to colorize the command line output text.
 */
 
#pragma once

#include <iostream>

// Windows implementation
#if (  ( defined(__WIN32) || defined(_WIN32) || defined(_WIN64) ) \
    && ( defined(USE_COLORED_OUTPUT) ) )

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

namespace colors
{
    const int white  = 7;
    const int green  = 2;
    const int red    = 4;
    const int yellow = 6;
    const int blue   = 1;
}

template <typename T>
void print( const T& msg, int color = colors::white )
{
    HANDLE console = ::GetStdHandle(STD_OUTPUT_HANDLE);
    ::SetConsoleTextAttribute(console, color);
    std::cout << msg;
    ::SetConsoleTextAttribute(console, colors::white);
}

// Linux implementation
#elif (  (defined (__linux__) \
      && ( defined(USE_COLORED_OUTPUT) ) ) )
     
namespace colors
{
    const int white  = 39;
    const int green  = 32;
    const int red    = 31;
    const int yellow = 33;
    const int blue   = 34;
}


template <typename T>
void print( const T& msg, int color = colors::white )
{
    std::cout << "\e[" << color << 'm' << msg << "\e[0m";
}

// default implementation: coloring is deactivated (or platform unkown)
#else 

// unused color codes to avoid compilation issues
namespace colors
{
    const int white = 39;
    const int green = 32;
    const int red = 31;
    const int yellow = 33;
    const int blue = 34;
}

template <typename T>
void print( const T& msg, int )
{
    std::cout << msg;
}

#endif
