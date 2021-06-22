cmake_minimum_required(VERSION 3.12)
project(ethreadout VERSION 0.1)

find_package(daq-cmake REQUIRED)

daq_setup_environment()

# ers: for exceptions and normal output
# TRACE: for low-level debugging statements
# Boost unit_test_framework: for the unit tests
# Boost program_options: to allow command line options with descriptions/default values

find_package(appfwk REQUIRED)
find_package(ers REQUIRED)
find_package(opmonlib REQUIRED)
find_package(dataformats REQUIRED)
find_package(readout REQUIRED)
find_package(TRACE REQUIRED)
find_package(Boost REQUIRED)
set(BOOST_LIBS ${Boost_SYSTEM_LIBRARY} ${Boost_THREAD_LIBRARY} ${Boost_LIBRARIES})

daq_codegen( vdemulator.jsonnet TEMPLATES Structs.hpp.j2 Nljs.hpp.j2 )
daq_codegen( *info.jsonnet DEP_PKGS opmonlib TEMPLATES opmonlib/InfoStructs.hpp.j2 opmonlib/InfoNljs.hpp.j2 )

##############################################################################
# Dependency sets
set(ETHREADOUT_DEPENDENCIES
        ers::ers
        logging::logging
        opmonlib::opmonlib
        dataformats::dataformats
        readout::readout
        )

##############################################################################
# Main library
daq_add_library(
        LINK_LIBRARIES appfwk::appfwk ${ETHREADOUT_DEPENDENCIES}
)

##############################################################################
# Note that when the ers library is linked in, it uses a "::" because
# it's in an external package, not a local CMake target. The semantics
# are <namespace>::<shared library / executable>
#daq_add_library(IntPrinter.cpp LINK_LIBRARIES ers::ers)

##############################################################################

# Plugins
daq_add_plugin(VDEmulator duneDAQModule LINK_LIBRARIES appfwk::appfwk ethreadout ${BOOST_LIBS})

##############################################################################

#daq_add_python_bindings(toy_wrapper.cpp LINK_LIBRARIES ${PROJECT_NAME})

##############################################################################
daq_add_application( udptest udptest.cxx TEST LINK_LIBRARIES appfwk::appfwk ethreadout ${BOOST_LIBS} )
daq_add_application( udp_receiver udp_receiver.cxx TEST LINK_LIBRARIES appfwk::appfwk ethreadout ${BOOST_LIBS} )
daq_add_application( udp_sender udp_sender.cxx TEST LINK_LIBRARIES appfwk::appfwk ethreadout ${BOOST_LIBS} )

##############################################################################
#daq_add_unit_test(ValueWrapper_test)

##############################################################################

# Could add "toylibrary_test_program" to the argument list here, but
# presumably end users wouldn't care about an integration test program
# and just want the shared object library installed

daq_install()



