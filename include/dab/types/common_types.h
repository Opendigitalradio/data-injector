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

#ifndef DABCOMMON_TYPES_COMMON_TYPES
#define DABCOMMON_TYPES_COMMON_TYPES

#include "dab/types/parse_status.h"
#include "dab/types/queue.h"

#include <complex>
#include <cstdint>
#include <utility>
#include <vector>

namespace dab
  {

  /**
   * @brief A convenience alias that represents a complex sample.
   *
   * It might be worth noting that std::complex was designed to be bit compatible
   * with the C99 complex type and can thus be used in conjunction with fftw.
   *
   * @author Felix Morgner
   * @since  1.0.1
   */
  using sample_t = std::complex<float>;

  /**
   * @brief A convenience alias that represents a vector of bytes
   *
   * @author Tobias Stauber
   * @since  1.0.0
   **/
  using byte_vector_t = std::vector<std::uint8_t>;

  /**
   * @brief A type used as return value by parsers.
   *
   * The first argument contains the return status of the parser.
   * The second argument contains the returned byte_vector_t.
   *
   * @author Tobias Stauber
   * @since  1.0.0
   **/
  using pair_status_vector_t = std::pair<dab::parse_status, byte_vector_t>;

  /**
   * @brief The type of a queue for transporting samples
   *
   * @author Felix Morgner
   * @since  1.0.0
   */
  using sample_queue_t = internal::queue<sample_t>;

  /**
   * @brief The type of a queue for transporting symbols
   *
   * @author Felix Morgner
   * @since  1.0.0
   */
  using symbol_queue_t = internal::queue<std::vector<float>>;

  namespace internal
    {

    /**
     * @brief A convenience alias that represents a complex sample
     *
     * @author Felix Morgner
     * @since  1.0.0
     *
     * @deprecated Deprecated since 1.0.1, see dab::sample_t
     */
    using sample_t = dab::sample_t;

    }

  }

#endif
