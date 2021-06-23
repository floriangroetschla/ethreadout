/**
 * @file udp_test.cxx Test application for
 * UDP implementation
 *
 * This is part of the DUNE DAQ Application Framework, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "UDPReceiver.hpp"
#include "UDPSender.hpp"

using namespace dunedaq::ethreadout;

int
main(int argc, char* argv[])
{
  UDPReceiver receiver("127.0.0.1", 30000);
  UDPSender sender("127.0.0.1",30001, "127.0.0.1", 30000);
  char* payload = "test";
  sender.send(payload, 5);
  receiver.receive();
}