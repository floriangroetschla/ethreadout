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

namespace dunedaq {
namespace ethreadout {

VDEmulator::VDEmulator(const std::string& name)
  : DAQModule(name)
{
  register_command("conf", &VDEmulator::do_conf);
  register_command("start", &VDEmulator::do_start);
  register_command("stop", &VDEmulator::do_stop);
}

void
VDEmulator::init(const data_t& args)
{

}

void
VDEmulator::get_info(opmonlib::InfoCollector& ci, int level)
{
}

void
VDEmulator::do_conf(const data_t& args)
{
  vdemulator::Conf config = args.get<vdemulator::Conf>();
  //m_sender = std::make_unique<UDPSender>(config.source_port, config.destination_port);
}

void
VDEmulator::do_start(const data_t& args)
{
}

void
VDEmulator::do_stop(const data_t& args)
{
}

std::unique_ptr<UDPSender> m_sender;

} // namespace ethreadout
} // namespace dunedaq

DEFINE_DUNE_DAQ_MODULE(dunedaq::ethreadout::VDEmulator)