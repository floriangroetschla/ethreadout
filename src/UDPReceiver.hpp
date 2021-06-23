/**
 * @file UDPReceiver.hpp Specific udp receiver.
 *
 * This is part of the DUNE DAQ , copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */
#ifndef ETHREADOUT_SRC_UDPRECEIVER_HPP_
#define ETHREADOUT_SRC_UDPRECEIVER_HPP_

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

class UDPReceiver {
public:
  UDPReceiver(std::string ip, int port)
    : m_io_service()
    , m_socket(m_io_service)
  {
    m_socket.open(boost::asio::ip::udp::v4());

    int one = 1;
    setsockopt(m_socket.native_handle(), SOL_SOCKET, SO_REUSEPORT, &one, sizeof(one));
    m_socket.bind(boost::asio::ip::udp::endpoint(boost::asio::ip::address::from_string(ip), port));
  }


  size_t receive() {
    boost::asio::ip::udp::endpoint sender_endpoint;
    size_t len = m_socket.receive_from(
    boost::asio::buffer(m_buffer), sender_endpoint);
    return len;
  }

private:
  boost::asio::io_service m_io_service;
  boost::asio::ip::udp::socket m_socket;
  boost::asio::ip::udp::endpoint m_endpoint;
  boost::array<char, 5568> m_buffer;
};

} // namespace ethreadout
} // namespace dunedaq

#endif // ETHREADOUT_SRC_UDPRECEIVER_HPP_