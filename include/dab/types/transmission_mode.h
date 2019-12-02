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

#ifndef DAB_TYPES_TRANSMISSION_MODE
#define DAB_TYPES_TRANSMISSION_MODE

#include <cstdint>
#include <type_traits>

/**
 * @internal
 * @file
 *
 * @brief This file contains the definition of the template base for DAB transmission modes
 *
 * @author Felix Morgner
 * @since  1.0.0
 */

namespace dab
  {

  /**
   * @internal
   *
   * @brief This namespace contains implementation details and should not be used by end users.
   *
   * @author Felix Morgner
   * @since  1.0.0
   */
  namespace internal
    {

    /**
     * @internal
     */
    namespace types
      {

      namespace impl
        {
        constexpr std::size_t next_power_of_two(std::size_t const value, std::size_t const start = 1)
          {
          return start > value ? start : next_power_of_two(value, start * 2);
          }
        }

      /**
       * @internal
       * @brief This class is the a template base for the different DAB modes
       *
       * @tparam Carriers The number of OFDM subcarriers for the transfer mode
       * @tparam FrameSymbols The number of OFDM symbols, including the PR, for each frame
       * @tparam FicSymbols The number of OFDM symbols that make up the FIC
       * @tparam FrameFibs The number of FIBs per frame for the transfer mode
       * @tparam FrameCifs The number of CIFs that make up the MSC of the frame
       *
       * This class provides a number of type members that contain compile-time constant
       * information related to the different modes supported by DAB. There exist some
       * specializations in the file modes.h that represent the 4 currently standardized
       * DAB transfer modes.
       *
       * @note This class is not thought to be used for instanciating object. Even though it
       * would be possible, functions would require to be templated. We therefore encourage the
       * use of values of type dab::transmission_mode_t to specify modes. There exists a
       * couple of * free functions to retrieve mode related information via
       * dab::transmission_mode_t values. We chose this design as it allows clean
       * compiletime computation without a large amount of cryptic nested if/elses.
       *
       * @author Felix Morgner
       * @since  1.0.0
       */
      struct transmission_mode
        {
        constexpr transmission_mode(std::uint8_t const id,
                                    std::uint16_t const carriers,
                                    std::uint8_t const frame_symbols,
                                    std::uint8_t const fic_symbols,
                                    std::uint8_t const frame_fibs,
                                    std::uint8_t const frame_cifs,
                                    std::uint32_t const frame_duration,
                                    std::uint16_t const guard_duration,
                                    std::uint16_t const useful_duration,
                                    std::uint16_t const null_duration)
          : id{id},
            carriers{carriers},
            frame_symbols(frame_symbols - 1),
            fic_symbols{fic_symbols},
            frame_fibs{frame_fibs},
            frame_cifs{frame_cifs},
            frame_duration{frame_duration},
            guard_duration{guard_duration},
            useful_duration{useful_duration},
            null_duration{null_duration}
          {

          }

        std::uint8_t const id;

        /**
         * @internal
         *
         * @brief The number of carriers for the selected transport mode.
         *
         * Each transport mode is associated with a different number of OFDM subcarriers. The
         * @p value member of this type provides access to the number of OFDM subcarriers.
         *
         * @author Felix Morgner
         * @since  1.0.0
         */
        std::uint16_t const carriers;

        /**
         * @internal
         *
         * @brief The number of symbols that make up a single frame.
         *
         * Each DAB transport frame is made up of a certain number of OFDM symbols. This type alias
         * provides a member called @p value that allows access to the number of symbols per frame.
         *
         * @note This typedef reduces the actual supplied number by 1, since we strip away the
         * phase-reference symbol.
         *
         * @author Felix Morgner
         * @since  1.0.0
         */
        std::uint8_t const frame_symbols;

        /**
         * @internal
         *
         * @brief The number of symbols that make up the Fast Information Channel (FIC).
         *
         * The Fast Information Channel (FIC) is made up of a certain number of symbols, depending
         * on the transfer mode. The FIC carries information describing the DAB ensemble that is
         * being broadcast on a specific frequency.
         *
         * @author Felix Morgner
         * @since  1.0.0
         */
        std::uint8_t const fic_symbols;

        /**
         * @internal
         *
         * @brief The number of Fast Information Blocks (FIBs) in a single frame.
         *
         * Each DAB transfer mode use a different number of Fast Information Blocks. These blocks
         * carry the so called Fast Information Groups, which in turn carry the actual information
         * describing the structure of the DAB ensemble.
         *
         * @author Felix Morgner
         * @since  1.0.0
         */
        std::uint8_t const frame_fibs;

        /**
         * @internal
         *
         * @brief The number of Common Interleaved Frames (CIFs) contained in a single frame.
         *
         * A DAB frame, or rather the MSC of a DAB frame, is made up of a mode dependent number of
         * Common Interleaved Frames (CIFs). These CIFs contain the so-called Capacity Units (CUs)
         * which in turn carry the services of an ensemble.
         *
         * @author Felix Morgner
         * @since  1.0.0
         */
        std::uint8_t const frame_cifs;

        /**
         * @internal
         *
         * @brief The duration of a frame in samples @ 2.048 MSps
         *
         * @author Felix Morgner
         * @since  1.0.0
         */
        std::uint32_t const frame_duration;

        /**
         * @internal
         *
         * @brief The duration of the guard in samples @ 2.048 MSps
         *
         * @author Felix Morgner
         * @since  1.0.0
         */
        std::uint16_t const guard_duration;

        /**
         * @internal
         *
         * @brief The duration of a symbol without guard in samples @ 2.048 MSps
         *
         * @author Felix Morgner
         * @since  1.0.0
         */
        std::uint16_t const useful_duration;

        /**
         * @internal
         *
         * @brief The duration of the null symbol in samples @ 2.048 MSps
         *
         * @author Felix Morgner
         * @since  1.0.0
         */
        std::uint16_t const null_duration;


        /**
         * @internal
         *
         * @brief The duration of a symbol with the guard in samples @ 2.048 MSps
         *
         * @author Felix Morgner
         * @since  1.0.0
         */
        std::uint16_t const symbol_duration = useful_duration + guard_duration;

        /**
         * @internal
         *
         * @brief The number of symbols that make up the Main Service Channel (MSC).
         *
         * The number of symbols making up the Main Service Channel (MSC) is dependent upon the
         * transfer mode. The number of MSC symbols is calculated directly from the value carried in
         * frame_symbols and fic_symbols.
         *
         * @author Felix Morgner
         * @since  1.0.0
         */
        std::uint8_t const msc_symbols = frame_symbols - fic_symbols;

        /**
         * @internal
         *
         * @brief The number of bits that make up a FIB codeword.
         *
         * The FIBs are made up of convolutionally coded codewords. Each group of codewords makes
         * up a FIB and describes a single CIF. The number of FIB bits is equal to the number of
         * FIBs times 256 bit per byte devided by the number of cifs.
         *
         * @author Felix Morgner
         * @since  1.0.0
         */
        std::uint16_t const fib_codeword_bits = frame_fibs * 256 / frame_cifs;

        std::uint16_t const punctured_codeword_size = fib_codeword_bits * 3;

        /**
         * @internal
         *
         * @brief The number of bits in a symbol.
         *
         * Each OFDM symbol in DAB transports 2 bits of information due to the QPSK used in DAB.
         *
         * @author Felix Morgner
         * @since  1.0.0
         */
        std::uint16_t const symbol_bits = carriers * 2;

        /**
         * @internal
         *
         * @brief The length of the FFT used to demodulate the signal
         *
         * @author Felix Morgner
         * @since  1.0.0
         */
        std::uint16_t const fft_length = impl::next_power_of_two(carriers);
        };

      }

    }

  }

#endif
