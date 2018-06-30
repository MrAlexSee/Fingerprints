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
    inline static void calcTotalSize(Fingerprints<FING_T> &fingerprints, 
        const vector<string> &words, size_t *wordCountsBySize)
    {
        return fingerprints.calcTotalSize(words, wordCountsBySize);
    }
};

} // namespace fingerprints

#endif // FINGERPRINTS_WHITEBOX_HPP