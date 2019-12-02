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

#include <boost/asio.hpp>
using namespace boost;

#include <tins/ip.h>
#include <tins/udp.h>
#include <tins/rawpdu.h>

#include <array>
#include <fstream>
#include <iostream>

#include <dab/msc_data_group/msc_data_group_generator.h>
#include <dab/packet/packet_generator.h>

#include "INIReader.h"

/**
 * @author Felix Morgner
 * @since 1.0
 *
 * A simple structure to hold our configuration
 */
struct configuration_t
  {
  /**
   * The DAB packet address for our packets
   */
  std::uint16_t packet_address{1000};

  /**
   * The IP destination address of the packed data
   */
  std::string destination_address{"10.0.0.1"};

  /**
   * The UDP destination port of the packed data
   */
  std::uint16_t destination_port{4242};

  /**
   * The IP source address of the packed data
   */
  std::string source_address{"10.0.0.2"};

  /**
   * The UDP source port of the packed data
   */
  std::uint16_t source_port{1337};
  };

/**
 * @author Felix Morgner
 * @since 1.0
 *
 * Receive data from the remote endpoint via UDP
 *
 * @param runLoop The ASIO io_service to run on
 * @param port The port to listen on for data
 */
std::string receive(asio::io_service & runLoop, std::uint16_t port)
  {
  // Set up ASIO to receive data via UDP
  asio::ip::udp::socket socket{runLoop, asio::ip::udp::endpoint{asio::ip::udp::v4(), port}};
  asio::ip::udp::endpoint remote{};

  // The buffer for the incoming data
  std::array<char, 1024> buffer{};

  // Receive data from a remote endpoint
  auto length = socket.receive_from(asio::buffer(buffer), remote);
  return {buffer.data(), length};
  }

/**
 * @author Felix Morgner
 * @since 1.0
 *
 * Wrap and split the received data into DAB packet mode packets
 *
 * @param data The data to wrap and split
 * @param config The configuration to use
 */
std::string wrap_data(std::string const & data, configuration_t const & config)
  {
  // Prepare our DAB packaging objects
  static auto grouper = dab::msc_data_group_generator{};
  static auto packer = dab::packet_generator{config.packet_address};

  // Repackage the received data into a new IP datagram
  auto datagram = (
    Tins::IP{config.destination_address, config.source_address} /
    Tins::UDP{config.destination_port, config.source_port} /
    Tins::RawPDU{data}
    ).serialize();

  // Wrap the newly created datagram into MSC data groups and split it into packets
  auto group = grouper.build(datagram);
  auto split = packer.build(group);
  return {reinterpret_cast<char const *>(split.data()), split.size()};
  }

int main() try
  {
  // The UDP port our daemon is listening at
  std::uint16_t constexpr UDP_PORT{4321};


  const char *configuration_file = "injector.ini";
  INIReader ini(configuration_file);
  int line_err = ini.ParseError();

  if (line_err) {
      std::cerr << "Error, cannot read configuration file '" << configuration_file << "'" << std::endl;
      std::cerr << "At line:       " << line_err << std::endl;
      return 1;
  }
  configuration_t conf;
  conf.source_address      = ini.Get("source.address", conf.source_address);
  conf.source_port         = ini.GetInteger("source.port", conf.source_port);
  conf.destination_address = ini.Get("destination.address", conf.destination_address);
  conf.destination_port    = ini.GetInteger("destination.port", conf.destination_port);
  conf.packet_address      = ini.GetInteger("packet.address", conf.packet_address);

  std::clog << "Loaded configuration: " <<
      conf.source_address << ":" << conf.source_port << " -> " <<
      conf.destination_address << ":" << conf.destination_port <<
      " packet addr " << conf.packet_address << std::endl;

  // The ASIO io_service we want to run network I/O operations on
  asio::io_service runLoop{};

  // The FIFO to write the data to
  std::ofstream fifo{"/tmp/dabdata", std::ios::binary};

  // Our main run loop
  while(true)
    {
    fifo << wrap_data(receive(runLoop, UDP_PORT), conf) << std::flush;
    }
  }
catch(std::exception const & error)
  {
  std::cerr << "Error: " << error.what() << '\n';
  }
catch(...)
  {
  std::cerr << "Unknown error\n";
  }
