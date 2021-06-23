/**
 * @file VDReceiver.hpp Receive VD packets
 *
 * This is part of the DUNE DAQ , copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#ifndef ETHREADOUT_PLUGINS_VDRECEIVER_HPP_
#define ETHREADOUT_PLUGINS_VDRECEIVER_HPP_

#include "appfwk/DAQModule.hpp"
#include "appfwk/DAQSource.hpp"
#include "UDPReceiver.hpp"
#include "readout/utils/ReusableThread.hpp"
#include "readout/ReadoutTypes.hpp"

namespace dunedaq {
namespace ethreadout {

class VDReceiver : public dunedaq::appfwk::DAQModule
{
public:
  explicit VDReceiver(const std::string& name);

  VDReceiver(const VDReceiver&) = delete;
  VDReceiver& operator=(const VDReceiver&) = delete;
  VDReceiver(VDReceiver&&) = delete;
  VDReceiver& operator=(VDReceiver&&) = delete;

  void init(const nlohmann::json& obj) override;
  void get_info(opmonlib::InfoCollector& ci, int level) override;

private:
  // Commands
  void do_conf(const nlohmann::json& obj);
  void do_start(const nlohmann::json& obj);
  void do_stop(const nlohmann::json& obj);
  void do_work();

  std::unique_ptr<UDPReceiver<dunedaq::readout::types::WIB_SUPERCHUNK_STRUCT>> m_receiver;

  dunedaq::readout::ReusableThread m_work_thread;
  std::atomic<bool> m_run_marker;

  using raw_queue_qt = appfwk::DAQSink<dunedaq::readout::types::WIB_SUPERCHUNK_STRUCT>;
  std::unique_ptr<raw_queue_qt> m_raw_data_sink;

};
} // namespace ethreadout
} // namespace dunedaq

#endif // ETHREADOUT_PLUGINS_VDRECEIVER_HPP_