/**
 * @file EthReadoutIssues.hpp Ethernet readout system related ERS issues
 *
 * This is part of the DUNE DAQ , copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */
#ifndef ETHREADOUT_SRC_ETHREADOUTISSUES_HPP_
#define ETHREADOUT_SRC_ETHREADOUTISSUES_HPP_

#include <ers/Issue.hpp>

#include <string>

namespace dunedaq {

ERS_DECLARE_ISSUE(ethreadout, ConfigurationError, " Readout Configuration Error: " << conferror, ((std::string)conferror))

ERS_DECLARE_ISSUE(ethreadout, CannotCreateSocket, " Could not create socket: " << socketname, ((std::string)socketname))

ERS_DECLARE_ISSUE(ethreadout, CannotBindSocket, " Could not bind socket: " << socketname, ((std::string)socketname))


} // namespace dunedaq

#endif // ETHREADOUT_SRC_ETHREADOUTISSUES_HPP_