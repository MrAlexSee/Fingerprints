/*
 ***
 A proof-of-concept program for lightweight fingerprints for fast approximate keyword matching using bitwise operations.
 ***
 Authors: Aleksander Cisłak, Szymon Grabowski. License: GNU LGPL v3.
 ***
 Set BOOST_DIR in makefile and type "make" for optimized compile.
 ***
 */

#include <boost/format.hpp>
#include <boost/program_options.hpp>
#include <chrono>
#include <cstdint>
#include <iostream>
#include <string>
#include <vector>

#include "fingerprints.hpp"
#include "fingerprints.cpp"

#include "helpers.hpp"
#include "params.hpp"

using namespace fingerprints;
using namespace std;

namespace po = boost::program_options;

namespace fingerprints
{

namespace 
{

Params params;

}

int handleParams(int argc, const char **argv);
bool checkInputFiles(const char *execName);
int run();

void runFingerprints(const vector<string> &words, const vector<string> &patterns);

}

int main(int argc, const char **argv)
{
    int paramsRes = handleParams(argc, argv);
    if (paramsRes != -1)
    {
        return paramsRes;
    }

    if (checkInputFiles(argv[0]) == false)
    {
        return params.errorExitCode;
    }

    return run();
}

namespace fingerprints
{

int handleParams(int argc, const char **argv)
{
    po::options_description options("Parameters");
    options.add_options()
       ("dump,d", "dump input file info and throughput to output file (useful for throughput testing)")
       ("help,h", "display help message")
       ("in-dict-file,i", po::value<string>(&params.inDictFile)->required(), "input dictionary file path (positional arg 1)")
       ("in-pattern-file,I", po::value<string>(&params.inPatternFile)->required(), "input pattern file path (positional arg 2)")
       ("approx,k", po::value<int>(&params.kApprox)->required(), "perform approximate search (Hamming distance) for k errors")
       ("out-file,o", po::value<string>(&params.outFile), "output file path")
       ("pattern-count,p", po::value<int>(&params.nPatterns), "maximum number of patterns read from top of the patterns (non-positive values are ignored)")
       ("separator,s", po::value<string>(&params.separator), "input data separator")
       ("version,v", "display version info");

    po::positional_options_description positionalOptions;

    positionalOptions.add("in-dict-file", 1);
    positionalOptions.add("in-pattern-file", 1);

    po::variables_map vm;

    try
    {
        po::store(po::command_line_parser(argc, argv).
                  options(options).
                  positional(positionalOptions).run(), vm);

        if (vm.count("help"))
        {
            cout << "Usage: " << argv[0] << " " << params.usageInfoString << endl << endl;
            cout << options << endl;

            return 0;
        }
        if (vm.count("version"))
        {
            cout << params.versionInfo << endl;
            return 0;
        }

        po::notify(vm);
    }
    catch (const po::error& e)
    {
        cerr << "Usage: " << argv[0] << " " << params.usageInfoString << endl << endl;
        cerr << options << endl;

        cerr << "Error: " << e.what() << endl;
        return params.errorExitCode;
    }

    if (vm.count("dump"))
    {
        params.dumpToFile = true;
    }

    return -1;
}

bool checkInputFiles(const char *execName)
{
    if (Helpers::isFileReadable(params.inDictFile) == false)
    {
        cerr << "Cannot access input dictionary file (doesn't exist or insufficient permissions): " << params.inDictFile << endl;
        cerr << "Run " << execName << " -h for more information" << endl << endl;

        return false;
    }

    if (Helpers::isFileReadable(params.inPatternFile) == false)
    {
        cerr << "Cannot access input patterns file (doesn't exist or insufficient permissions): " << params.inPatternFile << endl;
        cerr << "Run " << execName << " -h for more information" << endl << endl;

        return false;
    }

    return true;
}

int run()
{
    try
    {
        vector<string> dict = Helpers::readWords(params.inDictFile, params.separator);
        vector<string> patterns = Helpers::readWords(params.inPatternFile, params.separator);

        cout << boost::format("Read #words = %1%, #queries = %2%") % dict.size() % patterns.size() << endl;
        runFingerprints(dict, patterns);
    }
    catch (const exception &e)
    {
        cerr << endl << "Fatal error occurred: " << e.what() << endl;
        return params.errorExitCode;
    }

    return 0;
}

void runFingerprints(const vector<string> &words, const vector<string> &patterns)
{
    Fingerprints<uint16_t> fingerprints;

    fingerprints.preprocess(words);
    cout << boost::format("Preprocessed #words = %1%") % words.size() << endl;

    int nMatches = fingerprints.test(patterns, params.kApprox);

    cout << boost::format("#queries = %1%, #matches = %2%") % patterns.size() % nMatches << endl;

    float elapsedUs = fingerprints.getElapsedUs();
    float elapsedS = elapsedUs / 1'000'000;

    size_t dictSizeB = Helpers::getTotalSize(words);
    float dictSizeMB = static_cast<float>(dictSizeB) / 1'000'000.0;

    float throughputMBs = dictSizeMB / elapsedS;

    cout << boost::format("Elapsed = %1% us, throughput = %2% MB/s")
        % elapsedUs % throughputMBs  << endl;

    if (params.dumpToFile)
    {
        string outStr = params.inDictFile + " " + to_string(dictSizeMB) + " " + to_string(throughputMBs);
        Helpers::dumpToFile(outStr, params.outFile);
    }
}

} // namespace fingerprints
