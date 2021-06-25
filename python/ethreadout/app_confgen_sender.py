# Set moo schema search path
from dunedaq.env import get_moo_model_path
import moo.io
moo.io.default_load_path = get_moo_model_path()

# Load configuration types
import moo.otypes
moo.otypes.load_types('cmdlib/cmd.jsonnet')
moo.otypes.load_types('rcif/cmd.jsonnet')
moo.otypes.load_types('appfwk/cmd.jsonnet')
moo.otypes.load_types('appfwk/app.jsonnet')
moo.otypes.load_types('readout/fakecardreader.jsonnet')
moo.otypes.load_types('readout/datalinkhandler.jsonnet')
moo.otypes.load_types('readout/datarecorder.jsonnet')
moo.otypes.load_types('ethreadout/vdemulator.jsonnet')
moo.otypes.load_types('ethreadout/vdreceiver.jsonnet')
moo.otypes.load_types('flxlibs/felixcardreader.jsonnet')

# Import new types
import dunedaq.cmdlib.cmd as basecmd # AddressedCmd,
import dunedaq.rcif.cmd as rccmd # AddressedCmd,
import dunedaq.appfwk.app as app # AddressedCmd,
import dunedaq.appfwk.cmd as cmd # AddressedCmd,
import dunedaq.readout.fakecardreader as fcr
import dunedaq.readout.datalinkhandler as dlh
import dunedaq.readout.datarecorder as bfs
import dunedaq.ethreadout.vdemulator as vde
import dunedaq.ethreadout.vdreceiver as vdr
import dunedaq.flxlibs.felixcardreader as flxcr

from appfwk.utils import mcmd, mrccmd, mspec

import json
import math
# Time to waait on pop()
QUEUE_POP_WAIT_MS=100;
# local clock speed Hz
CLOCK_SPEED_HZ = 50000000;

def generate(
        FRONTEND_TYPE='wib',
        NUMBER_OF_DATA_PRODUCERS=1,
        NUMBER_OF_TP_PRODUCERS=1,
        DATA_RATE_SLOWDOWN_FACTOR = 1,
        RUN_NUMBER = 333,
        DATA_FILE="./frames.bin",
        USE_FELIX = False
):

    # Define modules and queues
    queue_bare_specs = [
                           app.QueueSpec(inst=f"{FRONTEND_TYPE}_link_emulator_{idx}", kind='FollySPSCQueue', capacity=100000)
                           for idx in range(NUMBER_OF_DATA_PRODUCERS)
                       ]


    # Only needed to reproduce the same order as when using jsonnet
    queue_specs = app.QueueSpecs(sorted(queue_bare_specs, key=lambda x: x.inst))


    mod_specs = [
                    mspec(f"vdemulator_{idx}", "VDEmulator", [
                        app.QueueInfo(name=f"input_queue", inst=f"{FRONTEND_TYPE}_link_emulator_{idx}", dir="input")
                    ]) for idx in range(NUMBER_OF_DATA_PRODUCERS)
                ]

    if USE_FELIX:
        mod_specs.append(mspec("flxcard_0", "FelixCardReader", [
            app.QueueInfo(name=f"output_{idx}", inst=f"{FRONTEND_TYPE}_link_emulator_{idx}", dir="output")
            for idx in range(min(5, NUMBER_OF_DATA_PRODUCERS))
        ]))
        if NUMBER_OF_DATA_PRODUCERS > 5 :
            mod_specs.append(mspec("flxcard_1", "FelixCardReader", [
                app.QueueInfo(name=f"output_{idx}", inst=f"{FRONTEND_TYPE}_link_emulator_{idx}", dir="output")
                for idx in range(5, NUMBER_OF_DATA_PRODUCERS)
            ]))
    else:
        mod_specs.append(mspec("fake_source", "FakeCardReader", [
            app.QueueInfo(name=f"output_{idx}", inst=f"{FRONTEND_TYPE}_link_emulator_{idx}", dir="output")
            for idx in range(NUMBER_OF_DATA_PRODUCERS)
        ]))

    init_specs = app.Init(queues=queue_specs, modules=mod_specs)

    jstr = json.dumps(init_specs.pod(), indent=4, sort_keys=True)
    print(jstr)

    initcmd = rccmd.RCCommand(
        id=basecmd.CmdId("init"),
        entry_state="NONE",
        exit_state="INITIAL",
        data=init_specs
    )

    CARDID = 0

    confcmd = mrccmd("conf", "INITIAL", "CONFIGURED", [
        ("flxcard_0",flxcr.Conf(card_id=CARDID,
                                logical_unit=0,
                                dma_id=0,
                                chunk_trailer_size= 32,
                                dma_block_size_kb= 4,
                                dma_memory_size_gb= 4,
                                numa_id=0,
                                num_links=min(5,NUMBER_OF_DATA_PRODUCERS))),
        ("flxcard_1",flxcr.Conf(card_id=CARDID,
                                logical_unit=1,
                                dma_id=0,
                                chunk_trailer_size= 32,
                                dma_block_size_kb= 4,
                                dma_memory_size_gb= 4,
                                numa_id=0,
                                num_links=max(0, NUMBER_OF_DATA_PRODUCERS - 5))),

        ("fake_source",fcr.Conf(
            link_confs=[fcr.LinkConfiguration(
                geoid=fcr.GeoID(system="TPC", region=0, element=idx),
                slowdown=DATA_RATE_SLOWDOWN_FACTOR,
                queue_name=f"output_{idx}"
            ) for idx in range(NUMBER_OF_DATA_PRODUCERS)],
            # input_limit=10485100, # default
            queue_timeout_ms = QUEUE_POP_WAIT_MS,
            set_t0_to = 0
        )),
    ] + [
                         (f"vdemulator_{idx}", vde.Conf(
                             source_ip = "192.168.10.100",
                             source_port = 30000 + idx,
                             destination_ip = "192.168.10.101",
                             destination_port = 30000
                         )) for idx in range(NUMBER_OF_DATA_PRODUCERS)
                     ])

    jstr = json.dumps(confcmd.pod(), indent=4, sort_keys=True)
    print(jstr)

    startpars = rccmd.StartParams(run=RUN_NUMBER)
    startcmd = mrccmd("start", "CONFIGURED", "RUNNING", [
        ("fake_source", startpars),
        ("vdemulator_.*", startpars),
        ("flxcard.*", startpars),
    ])

    jstr = json.dumps(startcmd.pod(), indent=4, sort_keys=True)
    print("="*80+"\nStart\n\n", jstr)


    stopcmd = mrccmd("stop", "RUNNING", "CONFIGURED", [
        ("fake_source", None),
        ("vdemulator_.*", None),
        ("flxcard.*", None),
    ])

    jstr = json.dumps(stopcmd.pod(), indent=4, sort_keys=True)
    print("="*80+"\nStop\n\n", jstr)

    scrapcmd = mrccmd("scrap", "CONFIGURED", "INITIAL", [
        ("", None)
    ])

    jstr = json.dumps(scrapcmd.pod(), indent=4, sort_keys=True)
    print("="*80+"\nScrap\n\n", jstr)

    # Create a list of commands
    cmd_seq = [initcmd, confcmd, startcmd, stopcmd, scrapcmd]


    # Print them as json (to be improved/moved out)
    jstr = json.dumps([c.pod() for c in cmd_seq], indent=4, sort_keys=True)
    return jstr

if __name__ == '__main__':
    # Add -h as default help option
    CONTEXT_SETTINGS = dict(help_option_names=['-h', '--help'])

    import click

    @click.command(context_settings=CONTEXT_SETTINGS)
    @click.option('-f', '--frontend-type', type=click.Choice(['wib', 'wib2', 'pds_queue', 'pds_list'], case_sensitive=True), default='wib')
    @click.option('-n', '--number-of-data-producers', default=1)
    @click.option('-t', '--number-of-tp-producers', default=0)
    @click.option('-s', '--data-rate-slowdown-factor', default=10)
    @click.option('-r', '--run-number', default=333)
    @click.option('-d', '--data-file', type=click.Path(), default='./frames.bin')
    @click.option('-x', '--use-felix', is_flag=True)
    @click.argument('json_file', type=click.Path(), default='fake_readout.json')
    def cli(frontend_type, number_of_data_producers, number_of_tp_producers, data_rate_slowdown_factor, run_number, data_file, use_felix, json_file):
        """
          JSON_FILE: Input raw data file.
          JSON_FILE: Output json configuration file.
        """

        with open(json_file, 'w') as f:
            f.write(generate(
                FRONTEND_TYPE = frontend_type,
                NUMBER_OF_DATA_PRODUCERS = number_of_data_producers,
                NUMBER_OF_TP_PRODUCERS = number_of_tp_producers,
                DATA_RATE_SLOWDOWN_FACTOR = data_rate_slowdown_factor,
                RUN_NUMBER = run_number,
                DATA_FILE = data_file,
                USE_FELIX = use_felix
            ))

        print(f"'{json_file}' generation completed.")

    cli()