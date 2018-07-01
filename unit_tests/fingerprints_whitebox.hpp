#ifndef FINGERPRINTS_WHITEBOX_HPP
#define FINGERPRINTS_WHITEBOX_HPP

#ifndef FINGERPRINTS_WHITEBOX
#define FINGERPRINTS_WHITEBOX \
    friend class FingerprintsWhitebox;
#endif

#include "../fingerprints.hpp"

namespace fingerprints
{

class FingerprintsWhitebox
{
public:
    FingerprintsWhitebox() = delete;

    template<typename FING_T>
    inline static size_t calcTotalSize(const vector<string> &words, size_t *wordCountsBySize)
    {
        return Fingerprints<FING_T>::calcTotalSize(words, wordCountsBySize);
    }

    template<typename FING_T>
    inline static unsigned int calcHammingWeight(unsigned int n)
    {
        return Fingerprints<FING_T>::calcHammingWeight(n);
    }

    template<typename FING_T>
    inline static bool isHamAMK(const char *str1, const char *str2, size_t size, int k)
    {
        return Fingerprints<FING_T>::isHamAMK(str1, str2, size, k);
    }
};

} // namespace fingerprints

#endif // FINGERPRINTS_WHITEBOX_HPP