#include <cassert>

namespace fingerprints
{

template<typename FING_T>
void Fingerprints<FING_T>::preprocess(const vector<string> &words)
{

}

template<typename FING_T>
void Fingerprints<FING_T>::test(const vector<string> &patterns)
{
    clock_t start, end;

    start = std::clock();

    for (const string &pattern : patterns) 
    {
        FING_T curFingerprint = calcFingerprint(pattern.c_str(), pattern.size());

        // We iterate over all words and calculate the Hamming distance only
        // when the fingerprint comparison is not successful.
    }

    end = std::clock();

    double elapsedSec = (end - start) / static_cast<double>(CLOCKS_PER_SEC);
    elapsedUs = elapsedSec / 1'000'000;
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

} // namespace fingerprints
