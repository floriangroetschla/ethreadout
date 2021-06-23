/**
 * @file udp_receiver.cxx Test application for
 * UDP implementation
 *
 * This is part of the DUNE DAQ Application Framework, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "UDPReceiver.hpp"
#include "UDPSender.hpp"
#include "logging/Logging.hpp"
#include "readout/ReadoutTypes.hpp"

using namespace dunedaq::ethreadout;

int
main(int argc, char* argv[])
{
  std::atomic<int64_t> bytes_received_total = 0;
  std::atomic<int64_t> bytes_received_since_last_statistics = 0;
  std::chrono::steady_clock::time_point time_point_last_statistics = std::chrono::steady_clock::now();

  auto statistics_thread = std::thread([&]() {
    while (true) {
      std::this_thread::sleep_for(std::chrono::milliseconds(1000));
      double time_diff = std::chrono::duration_cast<std::chrono::duration<double>>(std::chrono::steady_clock::now() -
                                                                                   time_point_last_statistics)
        .count();
      std::cout << "Bytes received: " << bytes_received_total << ", Throughput: "
             << static_cast<double>(bytes_received_since_last_statistics) / ((int64_t)1 << 17) / time_diff << " Mb/s"
             << std::endl;
      time_point_last_statistics = std::chrono::steady_clock::now();
      bytes_received_since_last_statistics = 0;
    }
  });

  std::vector<std::thread> threads;
  for (uint i = 0; i < 4; ++i) {
    threads.emplace_back([&]() {
      UDPReceiver<dunedaq::readout::types::WIB_SUPERCHUNK_STRUCT> receiver("127.0.0.1", 30000);
      dunedaq::readout::types::WIB_SUPERCHUNK_STRUCT element;
      while (true) {
        bool success = receiver.receive(element);
        if (!success) {
          std::cout << "Could not receive data" << std::endl;
        } else {
          bytes_received_total += sizeof(element);
          bytes_received_since_last_statistics += sizeof(element);
        }
      }
    });
  }
}