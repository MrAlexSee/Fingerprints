/*
 *** A proof-of-concept program for lightweight fingerprints for fast approximate keyword matching using bitwise operations.
 *** Authors: Aleksander Cisłak, Szymon Grabowski. License: GNU LGPL v3.
 *** Set BOOST_DIR in makefile and type "make" for optimized compile.
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
using FING_T = uint16_t;

/** Indicates that program execution should continue after checking parameters. */
constexpr int paramsResContinue = -1;

}

/** Handles cmd-line parameters, returns paramsResContinue if program execution should continue. */
int handleParams(int argc, const char **argv);
/** Returns true if input files are readable, false otherwise. */
bool checkInputFiles(const char *execName);
/** Runs the main program and returns the program exit code. */
int run();
/** Filters input data (dictionary and patterns) based on cmd-line parameters. */
void filterInput(vector<string> &dict, vector<string> &patterns);

void runFingerprints(const vector<string> &words, const vector<string> &patterns);
void initFingerprintParams(Fingerprints<FING_T>::DistanceType &distanceType,
    Fingerprints<FING_T>::FingerprintType &fingerprintType,
    Fingerprints<FING_T>::LettersType &lettersType);

void dumpParamInfoToStdout(int fingSizeB);
void dumpRunInfo(float elapsedUs, const vector<string> &words, size_t processedWordsCount);

}

int main(int argc, const char **argv)
{
    int paramsRes = handleParams(argc, argv);
    if (paramsRes != paramsResContinue)
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
       ("calc-rejection", "calculate percentages of rejected words instead of measuring time")
       ("dump,d", "dump input files and params info with elapsed time and throughput to output file (useful for testing)")
       ("dump-construction", "dump fingerprint construction time")
       ("distance,D", po::value<string>(&params.distanceType)->default_value("ham"), "distance metric: ham (Hamming), lev (Levenshtein)")
       ("fingerprint-type,f", po::value<string>(&params.fingerprintType)->default_value("occ"), "fingerprint type: none, occ (occurrence), occhalved (occurrence halved), count, pos (position)")
       ("help,h", "display help message")
       ("in-dict-file,i", po::value<string>(&params.inDictFile)->required(), "input dictionary file path (positional arg 1)")
       ("in-pattern-file,I", po::value<string>(&params.inPatternFile)->required(), "input pattern file path (positional arg 2)")
       ("iter", po::value<int>(&params.nIter)->default_value(1), "number of iterations per pattern lookup")
       ("approx,k", po::value<int>(&params.kApprox)->required(), "perform approximate search (Hamming or Levenshtein) for k errors")
       ("letters-type,l", po::value<string>(&params.lettersType)->default_value("common"), "letters type: common, mixed, rare")
       ("out-file,o", po::value<string>(&params.outFile)->default_value("res.txt"), "output file path")
       ("pattern-count,p", po::value<int>(&params.nPatterns), "maximum number of patterns read from top of the pattern file (non-positive values are ignored)")
       ("pattern-size", po::value<int>(&params.patternSize), "if set, only patterns of this size (letter count) will be read from the pattern file (non-positive values are ignored)")
       // Not using a default value from Boost for separator because it literally prints a newline.
       ("separator,s", po::value<string>(&params.separator), "input data (dictionary and patterns) separator (default = newline)")
       ("version,v", "display version info")
       ("word-count,w", po::value<int>(&params.nWords), "maximum number of words read from top of the dictionary file (non-positive values are ignored)");

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
    catch (const po::error &e)
    {
        cerr << "Usage: " << argv[0] << " " << params.usageInfoString << endl << endl;
        cerr << options << endl;

        cerr << "Error: " << e.what() << endl;
        return params.errorExitCode;
    }

    if (vm.count("calc-rejection"))
    {
        params.calcRejection = true;
    }
    if (vm.count("dump-construction"))
    {
        params.dumpConstruction = true;
    }
    if (vm.count("dump"))
    {
        params.dumpToFile = true;
    }

    return paramsResContinue;
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
       
        filterInput(dict, patterns);

        cout << "=====" << endl;
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

void filterInput(vector<string> &dict, vector<string> &patterns)
{
    if (params.nWords > 0 and static_cast<size_t>(params.nWords) < dict.size())
    {
        dict.resize(params.nWords);
    }

    if (params.nPatterns > 0 and static_cast<size_t>(params.nPatterns) < patterns.size())
    {
        patterns.resize(params.nPatterns);
    }

    if (params.patternSize > 0)
    {
        Helpers::filterWordsBySize(patterns, params.patternSize);
    }
}

void runFingerprints(const vector<string> &words, const vector<string> &patterns)
{
    dumpParamInfoToStdout(sizeof(FING_T));

    Fingerprints<FING_T>::DistanceType distanceType;
    Fingerprints<FING_T>::FingerprintType fingerprintType;
    Fingerprints<FING_T>::LettersType lettersType;

    initFingerprintParams(distanceType, fingerprintType, lettersType);

    Fingerprints<FING_T> fingerprints(distanceType, fingerprintType, lettersType);
    fingerprints.preprocess(words);
    
    cout << "Preprocessed #words = " << words.size() << endl;

    if (params.dumpConstruction)
    {
        float elapsedTotalUs = fingerprints.getElapsedUs();

        size_t dictSizeB = Helpers::getTotalSize(words);
        float dictSizeMB = static_cast<float>(dictSizeB) / 1'000'000.0f;

        float throughputMBs = dictSizeMB / (elapsedTotalUs / 1'000'000.0f);

         cout << boost::format("Construction: thru = %1% MB/s, elapsed = %2% us") 
            % throughputMBs % elapsedTotalUs << endl;
    }

    cout << "Testing #queries = " << patterns.size() << endl;
   
    if (params.calcRejection)
    {
        float rejectedFrac = fingerprints.testRejection(patterns, params.kApprox);
        cout << boost::format("Rejected ratio = %1%%%") % (100.0f * rejectedFrac) << endl;
    }
    else
    {
        int nMatches = fingerprints.test(patterns, params.kApprox, params.nIter, false);
        cout << "Got #matches = " << nMatches << endl;

        float elapsedTotalUs = fingerprints.getElapsedUs();
        float elapsedPerIterUs = elapsedTotalUs / static_cast<float>(params.nIter);

        size_t processedWordsCount = fingerprints.getProcessedWordsCount();
        cout << "Total (all patterns) processed #words = " << processedWordsCount << endl;

        dumpRunInfo(elapsedPerIterUs, words, processedWordsCount);
    }
}

void initFingerprintParams(Fingerprints<FING_T>::DistanceType &distanceType,
    Fingerprints<FING_T>::FingerprintType &fingerprintType,
    Fingerprints<FING_T>::LettersType &lettersType)
{
    if (params.distanceType == "ham")
    {
        distanceType = Fingerprints<FING_T>::DistanceType::Ham;
    }
    else if (params.distanceType == "lev")
    {
        distanceType = Fingerprints<FING_T>::DistanceType::Lev;
    }
    else
    {
        throw invalid_argument("bad distance type: " + params.distanceType);
    }

    if (params.fingerprintType == "none")
    {
        fingerprintType = Fingerprints<FING_T>::FingerprintType::None;
    }
    else if (params.fingerprintType == "occ")
    {
        fingerprintType = Fingerprints<FING_T>::FingerprintType::Occ;
    }
    else if (params.fingerprintType == "occhalved")
    {
        fingerprintType = Fingerprints<FING_T>::FingerprintType::OccHalved;
    }
    else if (params.fingerprintType == "count")
    {
        fingerprintType = Fingerprints<FING_T>::FingerprintType::Count;
    }
    else if (params.fingerprintType == "pos")
    {
        fingerprintType = Fingerprints<FING_T>::FingerprintType::Pos;
    }
    else
    {
        throw invalid_argument("bad fingerprint type: " + params.fingerprintType);
    }

    if (params.lettersType == "common")
    {
        lettersType = Fingerprints<FING_T>::LettersType::Common;
    }
    else if (params.lettersType == "mixed")
    {
        lettersType = Fingerprints<FING_T>::LettersType::Mixed;
    }
    else if (params.lettersType == "rare")
    {
        lettersType = Fingerprints<FING_T>::LettersType::Rare;
    }
    else
    {
        throw invalid_argument("bad letters type: " + params.lettersType);
    }
}

void dumpParamInfoToStdout(int fingSizeB)
{
    cout << "Using distance: " << params.distanceType << endl;
    cout << boost::format("Using fingerprint type: %1%, size = %2% bytes") % params.fingerprintType % fingSizeB << endl; 

    if (params.patternSize != Params::noValue)
    {
        cout << "Using pattern size = " << params.patternSize << endl;
    }

    cout << "Using letters type: " << params.lettersType << endl;
    cout << "Using k = " << params.kApprox << endl;
    cout << "#iterations = " << params.nIter << endl << endl;
}

void dumpRunInfo(float elapsedUs, const vector<string> &words, size_t processedWordsCount)
{
    size_t dictSizeB = Helpers::getTotalSize(words);
    float dictSizeMB = static_cast<float>(dictSizeB) / 1'000'000.0f;

    float elapsedPerWordNs = (1'000.0f * elapsedUs) / static_cast<float>(processedWordsCount);
    cout << boost::format("Elapsed = %1% us, per word = %2% ns") % elapsedUs % elapsedPerWordNs << endl;

    if (params.dumpToFile)
    {
        string outStr = (boost::format("%1% %2% %3% %4% %5% %6% %7%") % params.inDictFile % params.inPatternFile % dictSizeMB
            % params.distanceType % params.fingerprintType % params.lettersType % elapsedPerWordNs).str();

        Helpers::dumpToFile(outStr, params.outFile, true);
        cout << "Dumped info to: " << params.outFile << endl << endl;
    }
}

} // namespace fingerprints
