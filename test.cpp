////////////////////////////////////////////////////////////////////////
//
// test.cpp: Test harness for all the unit tests.
//
// Copyright (c) Simon Frankaus 2018
//

#include <cmath>
#include <memory>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/TestCase.h>
#include <cppunit/extensions/HelperMacros.h>

CppUnit::Test *suite()
{
    CppUnit::TestFactoryRegistry &registry =
        CppUnit::TestFactoryRegistry::getRegistry();

    registry.registerFactory(
        &CppUnit::TestFactoryRegistry::getRegistry("WeightingTestCase"));

    return registry.makeTest();
}

int main(int argc, char* argv[])
{
    // if command line contains "-selftest" then this is the post build check
    // => the output must be in the compiler error format.
    bool selfTest = (argc > 1) && (std::string("-selftest") == argv[1]);

    CppUnit::TextUi::TestRunner runner;
    runner.addTest(suite());

    if (selfTest) {
        // Change the default outputter to a compiler error format
        // outputter The test runner owns the new outputter.
        runner.setOutputter(CppUnit::CompilerOutputter::defaultOutputter(
                                &runner.result(),
                                std::cerr));
    }

    bool wasSucessful = runner.run("");
    return wasSucessful ? 0 : 1;
}
