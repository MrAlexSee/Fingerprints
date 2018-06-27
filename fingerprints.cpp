#include <bitset>
#include <cassert>
#include <unordered_map>

namespace fingerprints
{

template<typename FING_T>
Fingerprints<FING_T>::Fingerprints()
{
    initNErrorsLUT();

    #if FING_TYPE == 0
        initCharsMap();
        calcOccSetBitsLUT();
    #else
        #error Bad FING_TYPE
    #endif
}

template<typename FING_T>
Fingerprints<FING_T>::~Fingerprints()
{
    delete[] nErrorsLUT;
    delete[] charsMap;
    delete[] setBitsLUT;

    delete[] fingArray;
}

template<typename FING_T>
void Fingerprints<FING_T>::preprocess(vector<string> words)
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
            strncpy(curEntry, wordPtr, wordSize);
        
            curEntry += wordSize;
            *(reinterpret_cast<FING_T *>(curEntry)) = calcFingerprint(wordPtr, wordSize);

            curEntry += sizeof(FING_T);
            iWord += 1;
        }
    }

    assert(iWord == words.size()); // Making sure that all words have been processed.
}

template<typename FING_T>
int Fingerprints<FING_T>::test(const vector<string> &patterns, int k)
{
    int nMatches = 0;
    clock_t start, end;

    start = std::clock();

    for (const string &pattern : patterns) 
    {
        const size_t curSize = pattern.size();
        FING_T patFingerprint = calcFingerprint(pattern.c_str(), curSize);

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

    end = std::clock();

    double elapsedS = (end - start) / static_cast<double>(CLOCKS_PER_SEC);
    elapsedUs = elapsedS / 1'000'000;

    return nMatches;
}

template<typename FING_T>
void Fingerprints<FING_T>::initNErrorsLUT()
{
    size_t fingSizeBits = sizeof(FING_T) * 8;
    nErrorsLUT = new unsigned char[fingSizeBits + 1];

    for (size_t i = 0; i <= fingSizeBits; ++i)
    {
        nErrorsLUT[i] = ceil(i / 2.0);
    }
}

template<typename FING_T>
void Fingerprints<FING_T>::initCharsMap()
{
    charsMap = new unsigned char[charsMapSize];

    for (size_t i = 0; i < charsMapSize; ++i)
    {
        charsMap[i] = noCharIndex;
    }

#if FING_TYPE == 0
    size_t nChars = sizeof(FING_T) * 8;
    string charList = getCharList(nChars);
#else
    #error Bad FING_TYPE
#endif

    assert(nChars == charList.size());
    for (size_t i = 0; i < nChars; ++i)
    {
        const char c = charList[i];
        charsMap[static_cast<size_t>(c)] = i;
    }
}

template<typename FING_T>
string Fingerprints<FING_T>::getCharList(size_t nChars) const
{
    switch (nChars)
    {
        case 8:
            return commonChars8;
        case 16:
            return commonChars16;
        default:
            assert(false);
    }

    assert(false);
    return "";
}

template<typename FING_T>
void Fingerprints<FING_T>::calcOccSetBitsLUT()
{
    FING_T maxVal = std::numeric_limits<FING_T>::max();
    setBitsLUT = new unsigned char[maxVal + 1];

    FING_T n = 0;

    while (true)
    {
        setBitsLUT[n] = calcHammingWeight(n);
        assert(setBitsLUT[n] >= 0 and setBitsLUT[n] <= sizeof(FING_T) * 8);

        // Beware of overflows here.
        if (n == maxVal)
            break;

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
FING_T Fingerprints<FING_T>::calcFingerprint(const char *str, size_t size) const
{
    #if FING_TYPE == 0
        return calcFingerprintOcc(str, size);
    #else
        #error Bad FING_TYPE
    #endif
}

template<typename FING_T>
FING_T Fingerprints<FING_T>::calcFingerprintOcc(const char *str, size_t size) const
{
    assert(charsMap != nullptr);

    FING_T s = 0x0;
    FING_T mask = 0x1;

    for (size_t i = 0; i < size; ++i)
    {
        unsigned char index = charsMap[static_cast<size_t>(str[i])];
        assert(index < sizeof(FING_T) * 8 or index == noCharIndex);

        if (index != noCharIndex)
        {
            s |= (mask << index);
        }
    }

    return s;
}

template<typename FING_T>
unsigned int Fingerprints<FING_T>::calcHammingWeight(unsigned int n)
{
    return bitset<sizeof(FING_T)>(n).count();
}

template<typename FING_T>
unsigned char Fingerprints<FING_T>::calcNErrors(FING_T f1, FING_T f2) const
{
    unsigned char setBits = setBitsLUT[f1 ^ f2];

    assert(setBits >= 0 and setBits <= sizeof(FING_T) * 8);
    return nErrorsLUT[setBits];
}

template<typename FING_T>
bool Fingerprints<FING_T>::isHamAMK(const char *str1, const char *str2, size_t size, int k)
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

} // namespace fingerprints
