/*
 * Copyright (C) 2017 Opendigitalradio (http://www.opendigitalradio.org/)
 * Copyright (C) 2017 Felix Morgner <felix.morgner@hsr.ch>
 * Copyright (C) 2017 Tobias Stauber <tobias.stauber@hsr.ch>
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors
 *    may be used to endorse or promote products derived from this software without
 *    specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef DABCOMMON_LITERALS_BINARY_LITERAL
#define DABCOMMON_LITERALS_BINARY_LITERAL

#include <utility>

namespace dab
  {

  namespace literals
    {

    namespace impl
      {

      constexpr unsigned long long two_to(std::size_t power) {
        return power ? 2ull * two_to(power - 1) : 1ull;
      }

      template<char ...>
        struct parse_binary;

      template<char ...Digits> //Case for ‘0’
        struct parse_binary<'0', Digits...> {
          static unsigned long long const value { parse_binary<Digits...>::value };
        };

      template<char ...Digits> //Case for ‘1’
        struct parse_binary<'1', Digits...> {
          static unsigned long long const value {
            two_to(sizeof ...(Digits)) + parse_binary<Digits...>::value };
        };

      template<> //Base case
        struct parse_binary<> {
          static unsigned long long const value { 0 };
        };
      }

    /**
     * @brief An user defined literal for binary numbers
     *
     * @author Tobias Stauber
     * @since  1.0.0
     **/
    template<char ...Digits>
      constexpr unsigned long long operator"" _b() {
        return impl::parse_binary<Digits...>::value;
      }

    }

  }

#endif
