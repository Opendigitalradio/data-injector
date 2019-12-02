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

#ifndef DABCOMMON_CONSTANTS_TRANSMISSION_MODES
#define DABCOMMON_CONSTANTS_TRANSMISSION_MODES

#include "dab/types/transmission_mode.h"

namespace dab
  {

  /**
   * @brief DAB transport mode 1 descriptor
   *
   * @author Felix Morgner
   * @since  1.0.0
   */
  internal::types::transmission_mode constexpr kTransmissionMode1{1, 1536,  76, 3, 12, 4, 196608, 504, 2048, 2656};

  /**
   * @brief DAB transport mode 2 descriptor
   *
   * @author Felix Morgner
   * @since  1.0.0
   */
  internal::types::transmission_mode constexpr kTransmissionMode2{2, 384,  76, 3,  3, 1, 49152,  126,  512, 664};

  /**
   * @brief DAB transport mode 3 descriptor
   *
   * @author Felix Morgner
   * @since  1.0.0
   */
  internal::types::transmission_mode constexpr kTransmissionMode3{3, 192, 153, 8,  4, 1, 49152, 63,  256, 345};

  /**
   * @brief DAB transport mode 4 descriptor
   *
   * @author Felix Morgner
   * @since  1.0.0
   */
  internal::types::transmission_mode constexpr kTransmissionMode4{4, 768,  76, 3,  6, 2, 98304, 252, 1024, 1328};

  }

#endif
