// This is the application info schema used by the VDReceiver module.
// It describes the information object structure passed by the application
// for operational monitoring

local moo = import "moo.jsonnet";
local s = moo.oschema.schema("dunedaq.ethreadout.vdreceiverinfo");

local info = {
   uint8  : s.number("uint8", "u8",
                  doc="An unsigned of 8 bytes"),
   float8 : s.number("float8", "f8",
                  doc="A float of 8 bytes"),

   info: s.record("Info", [
       s.field("unknown_source_port", self.uint8, 0, doc="Number of packets received from unknown source port"),
       s.field("packets_received", self.uint8, 0, doc="Number of received packets"),
       s.field("receive_failed", self.uint8, 0, doc="Number of failed receives"),
       s.field("packets_out_of_order", self.uint8, 0, doc="Number of packets received out of order"),
       s.field("packets_pushed_to_queue", self.uint8, 0, doc="Packets pushed to queue")
   ], doc="VDEmulator information")
};

moo.oschema.sort_select(info)