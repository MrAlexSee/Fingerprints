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
    inline static bool getUseFingerprints(const Fingerprints<FING_T> &fingerprints)
    {
        return fingerprints.useFingerprints;
    }

    template<typename FING_T>
    inline static function<FING_T(const char *, size_t)> getCalcFingerprintFun(Fingerprints<FING_T> &fingerprints)
    {
        return fingerprints.calcFingerprintFun;
    }

    template<typename FING_T>
    inline static unsigned char *getNErrorsLUT(const Fingerprints<FING_T> &fingerprints)
    {
        return fingerprints.nErrorsLUT;
    }

    template<typename FING_T>
    inline static const unsigned char *getCharsMap(const Fingerprints<FING_T> &fingerprints)
    {
        return fingerprints.charsMap;
    }

    template<typename FING_T>
    inline static const unsigned char *getCharList(const Fingerprints<FING_T> &fingerprints)
    {
        return fingerprints.charList;
    }

    template<typename FING_T>
    inline static const unsigned char *getNMismatchesLUT(const Fingerprints<FING_T> &fingerprints)
    {
        return fingerprints.nMismatchesLUT;
    }

    template<typename FING_T>
    inline static size_t calcTotalSize(const vector<string> &words, size_t *wordCountsBySize)
    {
        return Fingerprints<FING_T>::calcTotalSize(words, wordCountsBySize);
    }

    template<typename FING_T>
    inline static FING_T calcFingerprintOcc(const Fingerprints<FING_T> &fingerprints, const char *str, size_t size)
    {
        return fingerprints.calcFingerprintOcc(str, size);
    }

    template<typename FING_T>
    inline static FING_T calcFingerprintCount(const Fingerprints<FING_T> &fingerprints, const char *str, size_t size)
    {
        return fingerprints.calcFingerprintCount(str, size);
    }

    template<typename FING_T>
    inline static unsigned int calcHammingWeight(unsigned int n)
    {
        return Fingerprints<FING_T>::calcHammingWeight(n);
    }

    template<typename FING_T>
    inline static unsigned char calcNErrors(const Fingerprints<FING_T> &fingerprints, FING_T f1, FING_T f2)
    {
        return fingerprints.calcNErrors(f1, f2);
    }

    template<typename FING_T>
    inline static bool isHamAMK(const char *str1, const char *str2, size_t size, int k)
    {
        return Fingerprints<FING_T>::isHamAMK(str1, size, str2, size, k);
    }
};

} // namespace fingerprints

#endif // FINGERPRINTS_WHITEBOX_HPP