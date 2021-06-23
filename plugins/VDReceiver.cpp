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
{
  register_command("conf", &VDReceiver::do_conf);
  register_command("start", &VDReceiver::do_start);
  register_command("stop", &VDReceiver::do_stop);
}

void
VDReceiver::init(const data_t& args)
{
  auto ini = args.get<appfwk::app::ModInit>();
  for (const auto& qi : ini.qinfos) {
    if (qi.dir != "input") {
      continue;
    }

    if (m_queues.find(qi.name) != m_queues.end()) {
      TLOG() << "Queue " << qi.name << " used twice";
    } else {
      m_queues[qi.name] = qi.inst;
    }
  }
}

void
VDReceiver::get_info(opmonlib::InfoCollector& ci, int level)
{
}

void
VDReceiver::do_conf(const data_t& args)
{
  auto conf = args.get<vdreceiver::Conf>();
  for (const auto& link_conf : conf.link_confs) {
    if (m_port_map.find(link_conf.source_port) != m_port_map.end()) {
      TLOG() << "Same port used twice: " << link_conf.source_port;
    } else if (m_queues.find(link_conf.queue_name) == m_queues.end()) {
      TLOG() << "Could not find queue " << link_conf.queue_name;
    } else {
      m_port_map[link_conf.source_port] = std::make_unique<raw_queue_qt>((m_queues[link_conf.queue_name]));
    }
  }

  for (uint i = 0; i < 4; ++i) {
    m_receivers.emplace_back(std::make_unique<UDPReceiver<dunedaq::readout::types::WIB_SUPERCHUNK_STRUCT>>(conf.ip, conf.port));
    m_worker_threads.emplace_back(std::make_unique<dunedaq::readout::ReusableThread>(0));
    m_worker_threads.back()->set_name("vdreceiver", i);
  }


  //m_receiver = std::make_unique<UDPReceiver<dunedaq::readout::types::WIB_SUPERCHUNK_STRUCT>>(config.ip, config.port);
}

void
VDReceiver::do_start(const data_t& args)
{
  m_run_marker = true;
  for (uint i = 0; i < m_worker_threads.size(); ++i) {
    m_worker_threads[i]->set_work(&VDReceiver::do_work, this, std::ref(*m_receivers[i]));
  }
}

void
VDReceiver::do_stop(const data_t& args)
{
  m_run_marker = false;
  for (auto& thread : m_worker_threads) {
    while (!thread->get_readiness()) {
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
  }
}

void VDReceiver::do_work(UDPReceiver<dunedaq::readout::types::WIB_SUPERCHUNK_STRUCT>& receiver)
{
  dunedaq::readout::types::WIB_SUPERCHUNK_STRUCT element;
  while (m_run_marker) {
    int port = receiver.receive(element);
    if (port == 0) {
      TLOG() << "Could not receive element";
    } else {
      m_port_map[port]->push(element);
    }
  }
}

} // namespace ethreadout
} // namespace dunedaq

DEFINE_DUNE_DAQ_MODULE(dunedaq::ethreadout::VDReceiver)