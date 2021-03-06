/**
 * @file udp_sender.cxx Test application for
 * UDP implementation
 *
 * This is part of the DUNE DAQ Application Framework, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "UDPReceiver.hpp"
#include "UDPSender.hpp"
#include "readout/ReadoutTypes.hpp"

using namespace dunedaq::ethreadout;

int
main(int argc, char* argv[])
{
  dunedaq::readout::types::WIB_SUPERCHUNK_STRUCT superchunk;
  char* payload = reinterpret_cast<char*>(&superchunk);

  std::vector<std::thread> threads;
  for (uint i = 0; i < 4; ++i) {
    threads.emplace_back([&]() {
      UDPSender sender("127.0.0.1", 30000 + i, "127.0.0.1", 30000);
      while (true) {
        sender.send(payload, sizeof(superchunk));
      }
    });
  }

  while (true) {
    // Wait
  }
}