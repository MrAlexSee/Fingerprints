#include <algorithm>
#include <bitset>
#include <cassert>
#include <climits>
#include <cstring>
#include <chrono>
#include <iostream>
#include <string>
#include <unordered_map>
#include <unordered_set>

namespace fingerprints
{

template<typename FING_T>
Fingerprints<FING_T>::Fingerprints(int distanceType, int fingerprintType, int lettersType)
{
    if (distanceType < 0 or distanceType > 1)
    {
        throw runtime_error("bad distance type: " + to_string(distanceType));
    }
    if (fingerprintType < -1 or fingerprintType > 3)
    {
        throw runtime_error("bad fingerprint type: " + to_string(fingerprintType));
    }
    if (lettersType < 0 or lettersType > 2)
    {
        throw runtime_error("bad letters type: " + to_string(lettersType));
    }

    initNErrorsLUT();
    
    if (static_cast<DistanceType>(distanceType) != DistanceType::Hamming)
    {
        useHamming = false;
        
        levV0 = new int[maxWordSize];
        levV1 = new int[maxWordSize];
    }

    switch (static_cast<FingerprintType>(fingerprintType))
    {
        case FingerprintType::NoFing:
            useFingerprints = false;
            break;
        case FingerprintType::Occ:
            initCharsMap(fingerprintType, lettersType);
            calcOccNMismatchesLUT();

            calcFingerprintFun = bind(&Fingerprints<FING_T>::calcFingerprintOcc, this, 
                placeholders::_1, placeholders::_2);
            break;
        case FingerprintType::Count:
            initCharsMap(fingerprintType, lettersType);
            calcCountNMismatchesLUT();

            calcFingerprintFun = bind(&Fingerprints<FING_T>::calcFingerprintCount, this, 
                placeholders::_1, placeholders::_2);
            break;
        case FingerprintType::Pos:
            initCharList(lettersType);
            calcPosNMismatchesLUT();

            calcFingerprintFun = bind(&Fingerprints<FING_T>::calcFingerprintPos, this, 
                placeholders::_1, placeholders::_2);
            break;
        case FingerprintType::OccHalved:
            initCharsMap(fingerprintType, lettersType);
            calcOccNMismatchesLUT();

            calcFingerprintFun = bind(&Fingerprints<FING_T>::calcFingerprintOccHalved, this, 
                placeholders::_1, placeholders::_2);
            break;
        default:
            assert(false);
    }
}

template<typename FING_T>
Fingerprints<FING_T>::~Fingerprints()
{
    delete[] fingArray;

    delete[] charsMap;
    delete[] charList;

    delete[] nErrorsLUT;
    delete[] nMismatchesLUT;

    delete[] levV0;
    delete[] levV1;
}

template<typename FING_T>
void Fingerprints<FING_T>::preprocess(const vector<string> &words)
{
    // We remove the duplicates.
    unordered_set<string> wordSet(words.begin(), words.end());

    // We convert the set back to a vector for further sorting.
    vector<string> wordsUnique(wordSet.begin(), wordSet.end());
    assert(wordsUnique.size() <= words.size());

    if (useFingerprints)
    {
        preprocessFingerprints(move(wordsUnique));
    }
    else
    {
        preprocessWords(move(wordsUnique));
    }
}

template<typename FING_T>
int Fingerprints<FING_T>::test(const vector<string> &patterns, int k, int nIter, bool setProcessedWordsCollection)
{
    if (k < 0)
    {
        throw runtime_error("bad k: " + to_string(k));
    }

    int nMatches = 0;
    clock_t start, end;

    if (useFingerprints)
    {
        if (useHamming)
        {
            start = std::clock();

            for (int i = 0; i < nIter; ++i)
            {
                nMatches = testFingerprintsHamming(patterns, k);
            }

            end = std::clock();
        }
        else
        {
            start = std::clock();
            
            for (int i = 0; i < nIter; ++i)
            {
                nMatches = testFingerprintsLeven(patterns, k);
            }

            end = std::clock();        
        }
    }
    else
    {
        if (useHamming)
        {
            start = std::clock();
            
            for (int i = 0; i < nIter; ++i)
            {
                nMatches = testWordsHamming(patterns, k);
            }

            end = std::clock();
        }
        else
        {
            start = std::clock();
            
            for (int i = 0; i < nIter; ++i)
            {
                nMatches = testWordsLeven(patterns, k);
            }

            end = std::clock();
        }
    }

    processedWords.clear();
    processedWordsCount = 0;

    // An additional run which does not affect time measurement is performed only to set the processed words count.
    // It mirrors the search which is performed above.
    if (setProcessedWordsCollection)
    {
        setProcessedWords(patterns, k);
    }
    else
    {
        setProcessedWordsCount(patterns, k);
    }

    float elapsedS = (end - start) / static_cast<float>(CLOCKS_PER_SEC);
    elapsedUs = elapsedS * 1'000'000.0f;

    return nMatches;
}

template<typename FING_T>
float Fingerprints<FING_T>::testRejection(const vector<string> &patterns, int k)
{
    if (k < 0)
    {
        throw runtime_error("bad k: " + to_string(k));
    }

    float res;

    if (useHamming)
    {
        res = testRejectionHamming(patterns, k);
    }
    else
    {
        res = testRejectionLeven(patterns, k);
    }

    return res;
}

template<typename FING_T>
void Fingerprints<FING_T>::preprocessFingerprints(vector<string> words)
{
    size_t wordCountsBySize[maxWordSize + 1];
    size_t totalSize = calcTotalSize(words, wordCountsBySize);

    fingArray = new char[totalSize];
    
    sort(words.begin(), words.end(), [](const string &str1, const string &str2) {
        return str1.size() < str2.size();
    });

    char *curEntry = fingArray;
    size_t iWord = 0;

    for (size_t wordSize = 1; wordSize <= maxWordSize; ++wordSize)
    {
        fingArrayEntries[wordSize] = curEntry;

        for (size_t iCurWord = 0; iCurWord < wordCountsBySize[wordSize]; ++iCurWord)
        {
            assert(words[iWord].size() == wordSize);
            const char *wordPtr = words[iWord].c_str();

            *(reinterpret_cast<FING_T *>(curEntry)) = calcFingerprintFun(wordPtr, wordSize);
            curEntry += sizeof(FING_T);

            strncpy(curEntry, wordPtr, wordSize);
        
            curEntry += wordSize;
            iWord += 1;
        }
    }

    fingArrayEntries[maxWordSize + 1] = curEntry;
    assert(iWord == words.size()); // Making sure that all words have been processed.
}

template<typename FING_T>
void Fingerprints<FING_T>::preprocessWords(vector<string> words)
{
    size_t wordCountsBySize[maxWordSize + 1];
    size_t totalSize = calcTotalSize(words, wordCountsBySize);

    // No fingerprints in this version.
    totalSize -= words.size() * sizeof(FING_T);

    fingArray = new char[totalSize];
    
    sort(words.begin(), words.end(), [](const string &str1, const string &str2) {
        return str1.size() < str2.size();
    });

    char *curEntry = fingArray;
    size_t iWord = 0;

    for (size_t wordSize = 1; wordSize <= maxWordSize; ++wordSize)
    {
        fingArrayEntries[wordSize] = curEntry;

        for (size_t iCurWord = 0; iCurWord < wordCountsBySize[wordSize]; ++iCurWord)
        {
            assert(words[iWord].size() == wordSize);
            strncpy(curEntry, words[iWord].c_str(), wordSize);
        
            curEntry += wordSize;
            iWord += 1;
        }
    }

    fingArrayEntries[maxWordSize + 1] = curEntry;
    assert(iWord == words.size()); // Making sure that all words have been processed.
}

template<typename FING_T>
void Fingerprints<FING_T>::initNErrorsLUT()
{
    size_t fingSizeBits = sizeof(FING_T) * 8;
    nErrorsLUT = new unsigned char[fingSizeBits + 1];

    for (size_t i = 0; i <= fingSizeBits; ++i)
    {
        nErrorsLUT[i] = ceil(i / 2.0f);
    }
}

template<typename FING_T>
void Fingerprints<FING_T>::initCharsMap(int fingerprintType, int lettersType)
{
    charsMap = new unsigned char[charsMapSize];

    for (size_t i = 0; i < charsMapSize; ++i)
    {
        charsMap[i] = noCharIndex;
    }

    size_t nChars;

    switch (static_cast<FingerprintType>(fingerprintType))
    {
        case FingerprintType::Occ:
            nChars = sizeof(FING_T) * 8;
            break;
        case FingerprintType::Count:
            nChars = sizeof(FING_T) * 4;
            break;
        case FingerprintType::OccHalved:
            nChars = sizeof(FING_T) * 4;
            break;
        default:
            assert(false);
            return;
    }

    string charList = getCharList(nChars, lettersType);
    assert(charList.size() == nChars);

    for (size_t i = 0; i < nChars; ++i)
    {
        const char c = charList[i];

        if (static_cast<FingerprintType>(fingerprintType) == FingerprintType::OccHalved)
        {
            charsMap[static_cast<size_t>(c)] = 2 * i;
        }
        else
        {
            charsMap[static_cast<size_t>(c)] = i;
        }
    }
}

template<typename FING_T>
void Fingerprints<FING_T>::initCharList(int lettersType)
{
    size_t nChars = sizeof(FING_T) * 8 / nBitsPerPos;
    string curList = getCharList(nChars, lettersType);

    charList = new unsigned char[curList.size() + 1];

    for (size_t i = 0; i < curList.size(); ++i)
    {
        charList[i] = curList[i];
    }

    charList[curList.size()] = '\0';
}

template<typename FING_T>
string Fingerprints<FING_T>::getCharList(size_t nChars, int lettersType) const
{
    switch (static_cast<LettersType>(lettersType))
    {
        case LettersType::Common:
            switch (nChars)
            {
                case 5:
                    return engCommonLettersPos5;
                case 8:
                    return engCommonLetters8;
                case 16:
                    return engCommonLetters16;
            }
        case LettersType::Mixed:
            switch (nChars)
            {
                case 5:
                    return engMixedLettersPos5;
                case 8:
                    return engMixedLetters8;
                case 16:
                    return engMixedLetters16;
            }
        case LettersType::Rare:
            switch (nChars)
            {
                case 5:
                    return engRareLettersPos5;
                case 8:
                    return engRareLetters8;
                case 16:
                    return engRareLetters16;
            }
    }

    assert(false);
    return "";
}

template<typename FING_T>
void Fingerprints<FING_T>::calcOccNMismatchesLUT()
{
    FING_T maxVal = std::numeric_limits<FING_T>::max();
    nMismatchesLUT = new unsigned char[maxVal + 1];

    FING_T n = 0x0U;

    while (true)
    {
        nMismatchesLUT[n] = calcHammingWeight(n);
        assert(nMismatchesLUT[n] >= 0 and nMismatchesLUT[n] <= sizeof(FING_T) * 8);

        // Beware of overflows here.
        if (n == maxVal)
        {
            break;
        }

        n += 1;
    }
}

template<typename FING_T>
void Fingerprints<FING_T>::calcCountNMismatchesLUT()
{
    assert(sizeof(FING_T) == 2);

    FING_T maxVal = std::numeric_limits<FING_T>::max();
    nMismatchesLUT = new unsigned char[maxVal + 1];

    FING_T mask = 0b011U;
    FING_T n = 0x0U;

    while (true)
    {
        unsigned char nErrors = 0;

        for (int iM = 0; iM < 8; ++iM)
        {
            // Unfortunately each difference in a count can lead only up to max 1 error.
            // Example: fingerprint 01 (letter count = 1) xored with fingerprint 10 (letter count = 2) 
            // -> results in 11.
            // Two bits are set in the result but they translate to only 1 error.
            if (n & (mask << (2 * iM)))
            {
                nErrors += 1;
            }
        }

        assert(nErrors >= 0 and nErrors <= 8);
        nMismatchesLUT[n] = nErrors;

        // Beware of overflows here.
        if (n == maxVal)
        {
            break;
        }

        n += 1;
    }
}

template<typename FING_T>
void Fingerprints<FING_T>::calcPosNMismatchesLUT()
{
    assert(sizeof(FING_T) == 2);

    FING_T maxVal = std::numeric_limits<FING_T>::max();
    nMismatchesLUT = new unsigned char[maxVal + 1];

    FING_T mask = 0b0111U;
    FING_T n = 0x0U;

    while (true)
    {
        unsigned char nErrors = 0;

        // If there is any difference between the corresponding positions,
        // then at least one of the bits in the triplet will be set.
        for (int iM = 0; iM < 5; ++iM)
        {
            if (n & (mask << (nBitsPerPos * iM)))
            {
                nErrors += 1;
            }
        }

        assert(nErrors >= 0 and nErrors <= 5);

        // The last bit is used for a single occurrence.
        if (n & (0x1U << 15))
        {
            nErrors += 1;
        }

        assert(nErrors >= 0 and nErrors <= 6);
        nMismatchesLUT[n] = nErrors;

        // Beware of overflows here.
        if (n == maxVal)
        {
            break;
        }

        n += 1;
    }
}

template<typename FING_T>
size_t Fingerprints<FING_T>::calcTotalSize(const vector<string> &words, size_t *wordCountsBySize)
{
    size_t totalSize = 0;

    for (size_t i = 0; i <= maxWordSize; ++i)
    {
        wordCountsBySize[i] = 0;
    }

    for (const string &word : words)
    {
        wordCountsBySize[word.size()] += 1;
        totalSize += word.size() + sizeof(FING_T);
    }

    return totalSize;
}

template<typename FING_T>
int Fingerprints<FING_T>::testFingerprintsHamming(const vector<string> &patterns, int k)
{
    int nMatches = 0;

    for (const string &pattern : patterns) 
    {
        const size_t curSize = pattern.size();
        const FING_T patFingerprint = calcFingerprintFun(pattern.c_str(), curSize);

        char *curEntry = fingArrayEntries[curSize];
        char *nextEntry = fingArrayEntries[curSize + 1];

        while (curEntry != nextEntry)
        {
            // We iterate over all words and calculate the Hamming distance only
            // when the fingerprint comparison is not successful.
            if (calcNErrors(patFingerprint, *(reinterpret_cast<FING_T *>(curEntry))) <= k)
            {
                curEntry += sizeof(FING_T);

                if (isHamAMK(pattern.c_str(), curEntry, curSize, k))
                {
                    // Make sure that the number of results is returned in order to
                    // prevent the compiler from overoptimizing unused results.
                    nMatches += 1;
                }

                curEntry += curSize;
            }
            else
            {
                curEntry += sizeof(FING_T);
                curEntry += curSize;
            }
        }
    }

    return nMatches;
}

template<typename FING_T>
int Fingerprints<FING_T>::testFingerprintsLeven(const vector<string> &patterns, int k)
{
    int nMatches = 0;

    for (const string &pattern : patterns) 
    {
        const size_t patSize = pattern.size();
        const FING_T patFingerprint = calcFingerprintFun(pattern.c_str(), patSize);

        // We omit sizes which differ by more than k.
        int left = static_cast<int>(patSize) - k;
        size_t right = patSize + k;

        const size_t start = (left < 1) ? 1u : left;
        const size_t stop = (right > maxWordSize) ? maxWordSize : right;

        for (size_t curSize = start; curSize <= stop; ++curSize)
        {
            char *curEntry = fingArrayEntries[curSize];
            char *nextEntry = fingArrayEntries[curSize + 1];

            while (curEntry != nextEntry)
            {
                // We iterate over all words and calculate the Hamming distance only
                // when the fingerprint comparison is not successful.
                if (calcNErrors(patFingerprint, *(reinterpret_cast<FING_T *>(curEntry))) <= k)
                {
                    curEntry += sizeof(FING_T);

                    if (isLevAMK(pattern.c_str(), patSize, curEntry, curSize, k))
                    {
                        // Make sure that the number of results is returned in order to
                        // prevent the compiler from overoptimizing unused results.
                        nMatches += 1;
                    }

                    curEntry += curSize;
                }
                else
                {
                    curEntry += sizeof(FING_T);
                    curEntry += curSize;
                }
            }
        }
    }

    return nMatches;
}

template<typename FING_T>
int Fingerprints<FING_T>::testWordsHamming(const vector<string> &patterns, int k)
{
    int nMatches = 0;
   
    for (const string &pattern : patterns) 
    {
        const size_t curSize = pattern.size();

        char *curEntry = fingArrayEntries[curSize];
        char *nextEntry = fingArrayEntries[curSize + 1];

        while (curEntry != nextEntry)
        {
            if (isHamAMK(pattern.c_str(), curEntry, curSize, k))
            {
                // Make sure that the number of results is returned in order to
                // prevent the compiler from overoptimizing unused results.
                nMatches += 1;
            }

            curEntry += curSize;
        }
    }

    return nMatches;
}

template<typename FING_T>
int Fingerprints<FING_T>::testWordsLeven(const vector<string> &patterns, int k)
{
    int nMatches = 0;

    for (const string &pattern : patterns) 
    {
        const size_t patSize = pattern.size();

        // We omit sizes which differ by more than k.
        int left = static_cast<int>(patSize) - k;
        size_t right = patSize + k;

        const size_t start = (left < 1) ? 1u : left;
        const size_t stop = (right > maxWordSize) ? maxWordSize : right;

        for (size_t curSize = start; curSize <= stop; ++curSize)
        {
            char *curEntry = fingArrayEntries[curSize];
            char *nextEntry = fingArrayEntries[curSize + 1];

            while (curEntry != nextEntry)
            {
                if (isLevAMK(pattern.c_str(), patSize, curEntry, curSize, k))
                {
                    // Make sure that the number of results is returned in order to
                    // prevent the compiler from overoptimizing unused results.
                    nMatches += 1;
                }

                curEntry += curSize;
            }
        }
    }

    return nMatches;
}

template<typename FING_T>
float Fingerprints<FING_T>::testRejectionHamming(const vector<string> &patterns, int k)
{
    int nRejected = 0, nTested = 0;

    for (const string &pattern : patterns) 
    {
        const size_t curSize = pattern.size();
        const FING_T patFingerprint = calcFingerprintFun(pattern.c_str(), curSize);

        char *curEntry = fingArrayEntries[curSize];
        char *nextEntry = fingArrayEntries[curSize + 1];

        while (curEntry != nextEntry)
        {
            if (calcNErrors(patFingerprint, *(reinterpret_cast<FING_T *>(curEntry))) > k)
            {
                nRejected += 1;
            }

            curEntry += sizeof(FING_T);
            curEntry += curSize;
        
            nTested += 1;
        }
    }

    if (nTested == 0)
    {
        return 0.0f;
    }

    return static_cast<float>(nRejected) / nTested;
}

template<typename FING_T>
float Fingerprints<FING_T>::testRejectionLeven(const vector<string> &patterns, int k)
{
    int nRejected = 0, nTested = 0;

    for (const string &pattern : patterns) 
    {
        const size_t patSize = pattern.size();
        const FING_T patFingerprint = calcFingerprintFun(pattern.c_str(), patSize);

        int left = static_cast<int>(patSize) - k;
        size_t right = patSize + k;

        const size_t start = (left < 1) ? 1u : left;
        const size_t stop = (right > maxWordSize) ? maxWordSize : right;

        for (size_t curSize = start; curSize <= stop; ++curSize)
        {
            char *curEntry = fingArrayEntries[curSize];
            char *nextEntry = fingArrayEntries[curSize + 1];

            while (curEntry != nextEntry)
            {
                if (calcNErrors(patFingerprint, *(reinterpret_cast<FING_T *>(curEntry))) > k)
                {
                    nRejected += 1;
                }

                curEntry += sizeof(FING_T);
                curEntry += curSize;
            
                nTested += 1;
            }
        }
    }

    if (nTested == 0)
    {
        return 0.0f;
    }

    return static_cast<float>(nRejected) / nTested;
}

template<typename FING_T>
void Fingerprints<FING_T>::setProcessedWords(const vector<string> &patterns, int k)
{
    if (useHamming)
    {
        for (const string &pattern : patterns) 
        {
            const size_t curSize = pattern.size();

            char *curEntry = fingArrayEntries[curSize];
            char *nextEntry = fingArrayEntries[curSize + 1];

            while (curEntry != nextEntry)
            {
                if (useFingerprints)
                {
                    curEntry += sizeof(FING_T);
                }

                processedWords.push_back(string(curEntry, curSize));
                curEntry += curSize;
            }
        }
    }
    else
    {
        // For each pattern: all words in a collection.
        for (size_t i = 0; i < patterns.size(); ++i)
        {
            for (size_t curSize = 1; curSize <= maxWordSize; ++curSize)
            {
                char *curEntry = fingArrayEntries[curSize];
                char *nextEntry = fingArrayEntries[curSize + 1];

                while (curEntry != nextEntry)
                {
                    if (useFingerprints)
                    {
                        curEntry += sizeof(FING_T);
                    }

                    processedWords.push_back(string(curEntry, curSize));
                    curEntry += curSize;
                }
            }   
        }
    }
}

template<typename FING_T>
void Fingerprints<FING_T>::setProcessedWordsCount(const vector<string> &patterns, int k)
{
    if (useHamming)
    {
        for (const string &pattern : patterns) 
        {
            const size_t curSize = pattern.size();

            char *curEntry = fingArrayEntries[curSize];
            char *nextEntry = fingArrayEntries[curSize + 1];

            while (curEntry != nextEntry)
            {
                if (useFingerprints)
                {
                    curEntry += sizeof(FING_T);
                }

                processedWordsCount += 1;
                curEntry += curSize;
            }
        }
    }
    else
    {
        size_t singleCollectionCount = 0;

        for (size_t curSize = 1; curSize <= maxWordSize; ++curSize)
        {
            char *curEntry = fingArrayEntries[curSize];
            char *nextEntry = fingArrayEntries[curSize + 1];

            while (curEntry != nextEntry)
            {
                if (useFingerprints)
                {
                    curEntry += sizeof(FING_T);
                }

                singleCollectionCount += 1;
                curEntry += curSize;
            }
        }   

        // For each pattern: all words in a collection.
        processedWordsCount = patterns.size() * singleCollectionCount;
    }
}

template<typename FING_T>
FING_T Fingerprints<FING_T>::calcFingerprintOcc(const char *str, size_t size) const
{
    assert(charsMap != nullptr);

    FING_T fing = 0x0U;
    FING_T mask = 0x1U;

    for (size_t i = 0; i < size; ++i)
    {
        unsigned char index = charsMap[static_cast<size_t>(str[i])];
        assert(index < sizeof(FING_T) * 8 or index == noCharIndex);

        if (index != noCharIndex)
        {
            fing |= (mask << index);
        }
    }

    return fing;
}

template<typename FING_T>
FING_T Fingerprints<FING_T>::calcFingerprintCount(const char *str, size_t size) const
{
    assert(charsMap != nullptr);
    
    // 2 byte == 16 bit fingerprints, 2 bits per count -> counts for 8 letters.
    assert(sizeof(FING_T) == 2);

    FING_T fing = 0x0U;

    for (size_t i = 0; i < size; ++i)
    {
        FING_T mask = 0x1U;
        unsigned char index = charsMap[static_cast<size_t>(str[i])];

        // Index < size of fingerprint in bits / 2 (count fingerprint -> 2 bits per character).
        assert(index < sizeof(FING_T) * 4 or index == noCharIndex);

        if (index != noCharIndex)
        {
            // In a count fingerprint we use two bits per index, hence we shift the mask twice in order
            // to set it to the relevant position.
            mask <<= index;
            mask <<= index;

            // If right bit is 0, we set it to 1 (0->1 or 2->3).
            if (not (fing & mask))
            {
                fing |= mask;
            }
            else // Otherwise either 1->2 or 2->3.
            {
                mask <<= 1;

                if (fing & mask) // 2 -> 3 or 3 -> 3 (no change)
                {
                    fing |= (mask >> 1);
                }
                else // 1 -> 2
                {
                    fing |= mask;
                    fing &= ~(mask >> 1);
                }
            }
        }
    }

    return fing;
}

template<typename FING_T>
FING_T Fingerprints<FING_T>::calcFingerprintPos(const char *str, size_t size) const
{
    assert(charList != nullptr);

    // 2 byte == 16 bit fingerprints, 3 bits per position -> positions for 5 letters.
    // 1 bit (the last one) is unused for positions and it is used for storing the occurrence of one additional letter.
    assert(sizeof(FING_T) == 2);

    FING_T fing = 0x0U;

    // We iterate excluding the last occurrence letter.
    for (size_t i = 0; i < 5; ++i)
    {
        // 0-based index == position of the first (leftmost) occurrence, 
        // 0b111 indicates position >= 6 or no match.
        size_t index = 0b111U;
        size_t stop = min(size, index);

        // We look for the first position at the defined index.
        for (size_t iC = 0; iC < stop; ++iC)
        {
            if (str[iC] == charList[i])
            {
                index = iC;
                break;
            }
        }
        
        // We encode this position in the fingerprint.
        fing |= (static_cast<FING_T>(index) << (i * nBitsPerPos));
    }

    // Finally we encode the last occurrence bit.
    const char occChar = charList[5];

    for (size_t iC = 0; iC < size; ++iC)
    {
        if (str[iC] == occChar)
        {
            fing |= (0x1U << 15);
            return fing;
        }
    }

    return fing;
}

template<typename FING_T>
FING_T Fingerprints<FING_T>::calcFingerprintOccHalved(const char *str, size_t size) const
{
    assert(charsMap != nullptr);

    FING_T fing = 0x0U;
    FING_T mask = 0x1U;

    const size_t mid = size / 2;

    for (size_t i = 0; i < mid; ++i)
    {
        unsigned char index = charsMap[static_cast<size_t>(str[i])];
        assert(index == noCharIndex or index < sizeof(FING_T) * 8);

        if (index != noCharIndex)
        {
            fing |= (mask << index);
        }
    }

    for (size_t i = mid; i < size; ++i)
    {
        unsigned char index = charsMap[static_cast<size_t>(str[i])];
        assert(index == noCharIndex or (index + 1U) < sizeof(FING_T) * 8);

        if (index != noCharIndex)
        {
            index += 1; // Second half
            fing |= (mask << index);
        }
    }

    return fing;
}

template<typename FING_T>
unsigned int Fingerprints<FING_T>::calcHammingWeight(unsigned int n)
{
    return bitset<8 * sizeof(FING_T)>(n).count();
}

template<typename FING_T>
unsigned char Fingerprints<FING_T>::calcNErrors(FING_T f1, FING_T f2) const
{
    unsigned char setBits = nMismatchesLUT[f1 ^ f2];

    assert(setBits >= 0 and setBits <= sizeof(FING_T) * 8);
    return nErrorsLUT[setBits];
}

template<typename FING_T>
bool Fingerprints<FING_T>::isHamAMK(const char *str1, const char *str2, const size_t size, const int k)
{
    // With compiler optimizations, this version is faster than any bitwise/avx/sse magic (tested).
    int nErrors = 0;

    for (size_t i = 0; i < size; ++i)
    {
        assert(str1[i] != '\0' and str2[i] != '\0');

        if (str1[i] != str2[i])
        {
            if (++nErrors > k)
            {
                return false;
            }
        }
    }

    return true;
}

// Calculates only the 2k + 1 strip since we are only interested in distance <= k.
// Attribution: based on: https://commons.apache.org/sandbox/commons-text/jacoco/org.apache.commons.text.similarity/LevenshteinDistance.java.html
template<typename FING_T>
bool Fingerprints<FING_T>::isLevAMK(const char *str1, const size_t size1, const char *str2, const size_t size2, const int k)
{
    const int size1s = size1;
    const int size2s = size2;

    if (size1s == 0)
    {
        return size2s <= k;
    }
    else if (size2s == 0)
    {
        return size1s <= k;
    }

    const int boundary = min(size1s, k) + 1;
    
    for (int i1 = 0; i1 < boundary; ++i1)
    {
        levV0[i1] = i1;
    }
    for (int i1 = boundary; i1 <= size1s; ++i1)
    {
        levV0[i1] = INT_MAX;
    }

    for (int i2 = 0; i2 <= size1s; ++i2)
    {
        levV1[i2] = INT_MAX;
    }

    for (int i = 1; i <= size2s; ++i)
    {
        levV1[0] = i;

        int left = max(1, i - k);
        int right = min(size1s, i + k);

        if (left > right)
        {
            return false;
        }
        if (left > 1)
        {
            levV1[left - 1] = INT_MAX;
        }

        const char c = str2[i - 1];
        assert(c != '\0');

        for (int iCur = left; iCur <= right; ++iCur)
        {
            const char curC = str1[iCur - 1];
            assert(curC != '\0');

            if (curC == c)
            {
                levV1[iCur] = levV0[iCur - 1];               
            }
            else
            {
                levV1[iCur] = 1 + min(min(levV1[iCur - 1], levV0[iCur]), levV0[iCur - 1]);
            }
        }

        int *temp = levV0;
        levV0 = levV1;
        levV1 = temp;
    }

    return levV0[size1] <= k;
}

} // namespace fingerprints
