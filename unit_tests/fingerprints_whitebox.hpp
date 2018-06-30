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
};

} // namespace fingerprints

#endif // FINGERPRINTS_WHITEBOX_HPP