// The schema used by classes in the appfwk code tests.
//
// It is an example of the lowest layer schema below that of the "cmd"
// and "app" and which defines the final command object structure as
// consumed by instances of specific DAQModule implementations (ie,
// the test/Fake* modules).

local moo = import "moo.jsonnet";

// A schema builder in the given path (namespace)
local ns = "dunedaq.ethreadout.vdreceiver";
local s = moo.oschema.schema(ns);

// Object structure used by the test/fake producer module
local vdreceiver = {
    size: s.number("Size", "u8",
                   doc="A count of very many things"),

    apa_number: s.number("APANumber", "u4",
                         doc="An APA number"),

    link_number: s.number("LinkNumber", "u4",
                        doc="A link number"),

    count : s.number("Count", "i4",
                     doc="A count of not too many things"),

    pct : s.number("Percent", "f4",
                   doc="Testing float number"),

    queue_name : s.string("QueueName", moo.re.ident,
                  doc="A string field"),

    string : s.string("String", '^(?:[0-9]{1,3}.){3}[0-9]{1,3}$',
                  doc="A string field"),

    choice : s.boolean("Choice"),

    link_conf : s.record("LinkConfiguration", [
            s.field("source_port", self.count, doc="Source port"),
            s.field("queue_name", self.queue_name,
                doc="Name of the output queue")
            ], doc="Configuration for one link"),

    link_conf_list : s.sequence("link_conf_list", self.link_conf, doc="Link configuration list"),

    conf: s.record("Conf", [
        s.field("link_confs", self.link_conf_list,
                        doc="Link configurations"),
        s.field("ip", self.string,
                doc="IP to listen on"),
        s.field("port", self.count,
                doc="Port to use")
    ], doc="VD receiver config"),

};

moo.oschema.sort_select(vdreceiver, ns)
