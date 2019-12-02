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

#include "dab/msc_data_group/msc_data_group_generator.h"
#include "dab/util/crc16.h"
#include "dab/util/vector_helpers.h"
#include "dab/constants/msc_data_group_constants.h"

#include <dab/types/common_types.h>
#include <dab/literals/binary_literal.h>

#include <bitset>

namespace dab
  {

  using namespace internal;

  byte_vector_t msc_data_group_generator::build_header()
    {
    auto header = byte_vector_t(2);
    header[0]  = 0 << 7;  //Extension flag
    header[0] |= 1 << 6;  //CRC flag
    header[0] |= 0 << 5;  //Segmentation flag
    header[0] |= 0 << 4;  //User access flag
    header[1]  = constants::kDataGroupTypes[0];    //Data group type
    header[1] |= m_continuity_index << 4; //Continuity index
    header[1] |= m_repetition_index;      //Repetition index
    return header;
    }

  byte_vector_t msc_data_group_generator::build(byte_vector_t & ip_datagram)
    {
    if(ip_datagram != m_last_ip_datagram)
      {
      m_continuity_index = (m_continuity_index + 1) % 16;
      m_repetition_index = 0;
      }
    else
      {
      m_repetition_index > 0 ? m_repetition_index-- : m_repetition_index = 0;
      }
    m_last_ip_datagram = ip_datagram;
    auto header = build_header();
    concat_vectors_inplace(header, ip_datagram);
    auto crc = genCRC16(header);
    concat_vectors_inplace(header, crc);
    return header;
    }
  }
