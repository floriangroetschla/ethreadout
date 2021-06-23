/**
 * @file UDPSender.hpp Specific udp sender.
 *
 * This is part of the DUNE DAQ , copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */
#ifndef ETHREADOUT_SRC_UDPSENDER_HPP_
#define ETHREADOUT_SRC_UDPSENDER_HPP_

#include "appfwk/app/Nljs.hpp"
#include "appfwk/cmd/Nljs.hpp"
#include "appfwk/cmd/Structs.hpp"
#include "EthReadoutIssues.hpp"

#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <boost/bind.hpp>
#include <iostream>

namespace dunedaq {
namespace ethreadout {

class UDPSender {
public:
  UDPSender(std::string source_ip, int source_port, std::string destination_ip, int destination_port)
  : m_io_service()
  , m_socket(m_io_service, boost::asio::ip::udp::endpoint(boost::asio::ip::address::from_string(source_ip), source_port))
  {
    m_endpoint = boost::asio::ip::udp::endpoint(boost::asio::ip::address::from_string(destination_ip), destination_port);
    //m_socket.open(boost::asio::ip::udp::v4());
  }

  bool send(const char* datagram, int len) {
    auto sent = m_socket.send_to(boost::asio::buffer(datagram, len), m_endpoint, 0, m_err);
    return true;
  }

  private:
  boost::asio::io_service m_io_service;
  boost::asio::ip::udp::socket m_socket;
  boost::asio::ip::udp::endpoint m_endpoint;
  boost::system::error_code m_err;

};

} // namespace ethreadout
} // namespace dunedaq

#endif // ETHREADOUT_SRC_UDPSENDER_HPP_