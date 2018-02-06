#ifndef CS5237_DEBUG_TRACINGH
#define CS5237_DEBUG_TRACINGH

#include <iostream>

/*
 * Quick and dirty implementation of a 'profiling' / trace tool.
 *
 * Use TRACE("message") at the beginning of a function.
 *
 * Messages are output to std::cout, 'intended' by one space for their
 * depth in the call-stack.
 * This is helpful for 'visualising' the slow parts of very slow algorithms.
 */

#ifndef NDEBUG
#define TRACE(X) TracingIndent tmp; TracingIndent::trace() << X << std::endl;
#else
#define TRACE(X)
#endif



class TracingIndent {
public:
    TracingIndent() { TracingIndent::level(+1); }
    ~TracingIndent() { TracingIndent::level(-1); }
    static int level(int dx = 0) {
        static int x(0);
        x += dx;
        return x;
    }

    static std::ostream& trace() {
        for (int i = 0; i < TracingIndent::level(); i++) {
            std::cout << ' ';
        }
        return std::cout;
    }
};


// static int TracingIndent::level(int dx = 0)

#endif