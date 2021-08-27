#ifndef R_EXASOL_REPEATING_BUFFER_FILL_H
#define R_EXASOL_REPEATING_BUFFER_FILL_H

#include <functional>

namespace exa {
    namespace algo {
        /**
         * Calls @param bufferFiller until the given input buffer is completely filled.
         */
        template <typename InputIt>
        inline bool repeatingBufferFill(InputIt first, InputIt last, std::function<int (InputIt, int)> bufferFiller) {
            int retVal = 0;
            do {
                const int nextLength = std::distance(first, last);
                retVal = bufferFiller(first, nextLength);
                if (retVal <= 0) {
                    break;
                }
                std::advance(first, retVal);
            } while(first != last);
            return (retVal > 0);
        }
    }
}

#endif //R_EXASOL_REPEATING_BUFFER_FILL_H
