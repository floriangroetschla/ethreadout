// The schema used by classes in the appfwk code tests.
//
// It is an example of the lowest layer schema below that of the "cmd"
// and "app" and which defines the final command object structure as
// consumed by instances of specific DAQModule implementations (ie,
// the test/Fake* modules).

local moo = import "moo.jsonnet";

// A schema builder in the given path (namespace)
local ns = "dunedaq.ethreadout.vdemulator";
local s = moo.oschema.schema(ns);

// Object structure used by the test/fake producer module
local vdemulator = {
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

    file_name : s.string("FileName", moo.re.ident,
                  doc="A string field"),

    string : s.string("String", moo.re.ident,
                  doc="A string field"),

    choice : s.boolean("Choice"),

    conf: s.record("Conf", [
        s.field("source_port", self.count,
                doc="Port number to use for the sender"),
        s.field("destination_port", self.count,
                doc="Port number to use for the receiver")
    ], doc="VD Emulator config"),

};

moo.oschema.sort_select(vdemulator, ns)
