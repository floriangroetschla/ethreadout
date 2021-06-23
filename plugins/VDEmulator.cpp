/**
 * @file VDEmulator.cpp VDEmulator implementation
 *
 * This is part of the DUNE DAQ , copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */
#include "VDEmulator.hpp"
#include "ethreadout/vdemulator/Nljs.hpp"
#include "ethreadout/vdemulator/Structs.hpp"
#include "appfwk/DAQModuleHelper.hpp"

namespace dunedaq {
namespace ethreadout {

VDEmulator::VDEmulator(const std::string& name)
  : DAQModule(name)
  , m_work_thread(0)
{
  register_command("conf", &VDEmulator::do_conf);
  register_command("start", &VDEmulator::do_start);
  register_command("stop", &VDEmulator::do_stop);
}

void
VDEmulator::init(const data_t& args)
{
  try {
    auto qi = appfwk::queue_index(args, { "input_queue" });
    m_raw_data_source.reset(new raw_queue_qt(qi["input_queue"].inst));
  } catch (const ers::Issue& excpt) {
    TLOG() << "Could not create queue";
  }
}

void
VDEmulator::get_info(opmonlib::InfoCollector& ci, int level)
{
}

void
VDEmulator::do_conf(const data_t& args)
{
  vdemulator::Conf config = args.get<vdemulator::Conf>();
  m_sender = std::make_unique<UDPSender>(config.source_ip, config.source_port, config.destination_ip, config.destination_port);
  m_work_thread.set_name("vdemu", config.source_port);
}

void
VDEmulator::do_start(const data_t& args)
{
  m_run_marker = true;
  m_work_thread.set_work(&VDEmulator::do_work, this);
}

void
VDEmulator::do_stop(const data_t& args)
{
  m_run_marker = false;
  while (!m_work_thread.get_readiness()) {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }
}

void VDEmulator::do_work()
{
  dunedaq::readout::types::WIB_SUPERCHUNK_STRUCT element;
  while (m_run_marker) {
    try {
      m_raw_data_source->pop(element, std::chrono::milliseconds(100));
      m_sender->send(reinterpret_cast<char*>(&element), sizeof(element));
    } catch (const dunedaq::appfwk::QueueTimeoutExpired& excpt) {
      continue;
    }
  }
}

} // namespace ethreadout
} // namespace dunedaq

DEFINE_DUNE_DAQ_MODULE(dunedaq::ethreadout::VDEmulator)