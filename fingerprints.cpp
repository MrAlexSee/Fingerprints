#include <bitset>
#include <cassert>
#include <unordered_map>

namespace fingerprints
{

template<typename FING_T>
Fingerprints<FING_T>::Fingerprints(int fingerprintType, int lettersType)
{
    if (fingerprintType < 0 or fingerprintType > 1)
    {
        throw runtime_error("bad fingerprint type: " + to_string(fingerprintType));
    }
    if (lettersType < 0 or lettersType > 2)
    {
        throw runtime_error("bad letters type: " + to_string(lettersType));
    }

    initNErrorsLUT();

    switch (fingerprintType)
    {
        case 0:
            initCharsMap(0, lettersType);
            calcOccSetBitsLUT();

            calcFingerprintFun = bind(&Fingerprints<FING_T>::calcFingerprintOcc, this, 
                placeholders::_1, placeholders::_2);
            break;
        case 1:
            initCharsMap(1, lettersType);
            calcCountSetBitsLUT();

            calcFingerprintFun = bind(&Fingerprints<FING_T>::calcFingerprintCount, this, 
                placeholders::_1, placeholders::_2);
            break;
        default:
            assert(false);
    }
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

            *(reinterpret_cast<FING_T *>(curEntry)) = calcFingerprintFun(wordPtr, wordSize);
            curEntry += sizeof(FING_T);

            strncpy(curEntry, wordPtr, wordSize);
        
            curEntry += wordSize;
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
        FING_T patFingerprint = calcFingerprintFun(pattern.c_str(), curSize);

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
    elapsedUs = elapsedS * 1'000'000;

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
void Fingerprints<FING_T>::initCharsMap(int fingerprintType, int lettersType)
{
    charsMap = new unsigned char[charsMapSize];

    for (size_t i = 0; i < charsMapSize; ++i)
    {
        charsMap[i] = noCharIndex;
    }

    size_t nChars;
    switch (fingerprintType)
    {
        case 0:
            nChars = sizeof(FING_T) * 8;
            break;
        case 1:
            nChars = sizeof(FING_T) * 4;
            break;
        default:
            assert(false);
    }

    string charList = getCharList(nChars, lettersType);

    assert(nChars == charList.size());
    for (size_t i = 0; i < nChars; ++i)
    {
        const char c = charList[i];
        charsMap[static_cast<size_t>(c)] = i;
    }
}

template<typename FING_T>
string Fingerprints<FING_T>::getCharList(size_t nChars, int lettersType) const
{
    switch (lettersType)
    {
        case 0: // common
            switch (nChars)
            {
                case 8:
                    return commonChars8;
                case 16:
                    return commonChars16;
            }
        case 1: // mixed
            switch (nChars)
            {
                case 8:
                    return mixedChars8;
                case 16:
                    return mixedChars16;
            }
        case 2: // rare
            switch (nChars)
            {
                case 8:
                    return rareChars8;
                case 16:
                    return rareChars16;
            }
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
void Fingerprints<FING_T>::calcCountSetBitsLUT()
{
    assert(sizeof(FING_T) == 2);

    FING_T maxVal = std::numeric_limits<FING_T>::max();
    setBitsLUT = new unsigned char[maxVal + 1];

    FING_T mask = 0b011;
    FING_T n = 0;

    while (true)
    {
        unsigned char nErrors = 0;

        for (int iM = 0; iM < 8; ++iM)
        {
            FING_T curDiff = n & (mask << (2 * iM));

            // Unfortunately each difference in a count can lead only up to max 1 error.
            // Example: 01 xor 10 -> 11 (1 error).
            if (curDiff)
            {
                nErrors += 1;
            }

            assert(nErrors >= 0 and nErrors <= 8);
            setBitsLUT[n] = nErrors;
        }

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
FING_T Fingerprints<FING_T>::calcFingerprintCount(const char *str, size_t size) const
{
    assert(sizeof(FING_T) == 2);
    FING_T s = 0x0;

    for (size_t i = 0; i < size; ++i)
    {
        FING_T mask = 0x1;
        unsigned char index = charsMap[static_cast<size_t>(str[i])];

        // Index < size of sketch in bits / 2 (count sketch = 2 bits per character).
        assert(index < sizeof(FING_T) * 4 or index == noCharIndex);

        if (index != noCharIndex)
        {
            // In a count sketch we use two bits per index, hence we shift the mask twice in order
            // to set it to the relevant position.
            mask <<= index;
            mask <<= index;

            // If right bit is 0, we set it to 1 (0->1 or 2->3).
            if (not (s & mask))
            {
                s |= mask;
            }
            else // Otherwise either 1->2 or 2->3.
            {
                mask <<= 1;

                if (s & mask) // 2 -> 3
                {
                    s |= (mask >> 1);
                }
                else // 1 -> 2
                {
                    s |= mask;
                    s &= ~(mask >> 1);
                }
            }
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
