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
       ("dump,d", "dump input files and params info with elapsed and throughput to output file (useful for testing)")
       ("help,h", "display help message")
       ("fingerprint-type,f", po::value<int>(&params.fingerprintType), "fingerprint type: 0 -> occurrence, 1 -> count (default = 0)")
       ("in-dict-file,i", po::value<string>(&params.inDictFile)->required(), "input dictionary file path (positional arg 1)")
       ("in-pattern-file,I", po::value<string>(&params.inPatternFile)->required(), "input pattern file path (positional arg 2)")
       ("approx,k", po::value<int>(&params.kApprox)->required(), "perform approximate search (Hamming distance) for k errors")
       ("letters-type,l", po::value<int>(&params.lettersType), "letters type: 0 -> common, 1 -> mixed, 2 -> rare (default = 0)")
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

        if (params.nPatterns > 0)
        {
            patterns.resize(params.nPatterns);
        }

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
    using FING_T = uint16_t;
    Fingerprints<FING_T> fingerprints(params.fingerprintType, params.lettersType);

    cout << boost::format("Using fingerprint size: %1% bytes") % sizeof(FING_T) << endl; 
    cout << "Using fingerprint type: " << params.fingerprintType << endl; 
    cout << "Using letters type: " << params.lettersType << endl << endl;

    fingerprints.preprocess(words);
    cout << "Preprocessed #words = " << words.size() << endl;

    cout << "Testing #queries = " << patterns.size() << endl;
    int nMatches = fingerprints.test(patterns, params.kApprox);

    cout << "Got #matches = " << nMatches << endl;

    float elapsedUs = fingerprints.getElapsedUs();
    float elapsedS = elapsedUs / 1'000'000;

    size_t dictSizeB = Helpers::getTotalSize(words);
    float dictSizeMB = static_cast<float>(dictSizeB) / 1'000'000.0;

    float throughputMBs = dictSizeMB / (elapsedS / patterns.size());
    float elapsedPerWordNs = 1'000.0 * elapsedUs / (words.size() * patterns.size());

    cout << boost::format("Elapsed = %1% us, per word = %2% ns, throughput = %3% MB/s")
        % elapsedUs % elapsedPerWordNs % throughputMBs << endl;

    if (params.dumpToFile)
    {
        string outStr = (boost::format("%1% %2% %3% %4% %5%") % params.inDictFile % params.inPatternFile
            % dictSizeMB % elapsedPerWordNs % throughputMBs).str();

        Helpers::dumpToFile(outStr, params.outFile, true);
        cout << "Dumped info to: " << params.outFile << endl;
    }
}

} // namespace fingerprints
