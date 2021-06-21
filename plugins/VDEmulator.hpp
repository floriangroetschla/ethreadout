/**
 * @file VDEmulator.hpp Emulate the VD detector
 *
 * This is part of the DUNE DAQ , copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#ifndef ETHREADOUT_PLUGINS_VDEMULATOR_HPP_
#define ETHREADOUT_PLUGINS_VDEMULATOR_HPP_

#include "appfwk/DAQModule.hpp"
#include "appfwk/DAQSource.hpp"
#include "UDPSender.hpp"

namespace dunedaq {
namespace ethreadout {

class VDEmulator : public dunedaq::appfwk::DAQModule
{
public:
  explicit VDEmulator(const std::string& name);

  VDEmulator(const VDEmulator&) = delete;
  VDEmulator& operator=(const VDEmulator&) = delete;
  VDEmulator(VDEmulator&&) = delete;
  VDEmulator& operator=(VDEmulator&&) = delete;

  void init(const nlohmann::json& obj) override;
  void get_info(opmonlib::InfoCollector& ci, int level) override;

private:
  // Commands
  void do_conf(const nlohmann::json& obj);
  void do_start(const nlohmann::json& obj);
  void do_stop(const nlohmann::json& obj);

};
} // namespace ethreadout
} // namespace dunedaq

#endif // ETHREADOUT_PLUGINS_ETHREADOUT_HPP_