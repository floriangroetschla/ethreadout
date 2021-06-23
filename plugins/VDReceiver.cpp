/**
 * @file VDReceiver.cpp VDReceiver implementation
 *
 * This is part of the DUNE DAQ , copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */
#include "VDReceiver.hpp"
#include "ethreadout/vdreceiver/Nljs.hpp"
#include "ethreadout/vdreceiver/Structs.hpp"
#include "appfwk/DAQModuleHelper.hpp"

namespace dunedaq {
namespace ethreadout {

VDReceiver::VDReceiver(const std::string& name)
  : DAQModule(name)
  , m_work_thread(0)
{
  register_command("conf", &VDReceiver::do_conf);
  register_command("start", &VDReceiver::do_start);
  register_command("stop", &VDReceiver::do_stop);
}

void
VDReceiver::init(const data_t& args)
{
  try {
    auto qi = appfwk::queue_index(args, { "output_queue" });
    m_raw_data_sink.reset(new raw_queue_qt(qi["output_queue"].inst));
  } catch (const ers::Issue& excpt) {
    TLOG() << "Could not create queue";
  }
}

void
VDReceiver::get_info(opmonlib::InfoCollector& ci, int level)
{
}

void
VDReceiver::do_conf(const data_t& args)
{
  vdreceiver::Conf config = args.get<vdreceiver::Conf>();
  m_receiver = std::make_unique<UDPReceiver<dunedaq::readout::types::WIB_SUPERCHUNK_STRUCT>>(config.ip, config.port);
  m_work_thread.set_name("vdreceiver", config.port);
}

void
VDReceiver::do_start(const data_t& args)
{
  m_run_marker = true;
  m_work_thread.set_work(&VDReceiver::do_work, this);
}

void
VDReceiver::do_stop(const data_t& args)
{
  m_run_marker = false;
  while (!m_work_thread.get_readiness()) {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }
}

void VDReceiver::do_work()
{
  dunedaq::readout::types::WIB_SUPERCHUNK_STRUCT element;
  while (m_run_marker) {
    bool success = m_receiver->receive(element);
    if (!success) {
      TLOG() << "Could not receive element";
    } else {
      m_raw_data_sink->push(element);
    }
  }
}

} // namespace ethreadout
} // namespace dunedaq

DEFINE_DUNE_DAQ_MODULE(dunedaq::ethreadout::VDReceiver)