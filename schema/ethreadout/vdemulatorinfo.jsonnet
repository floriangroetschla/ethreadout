// This is the application info schema used by the VDEmulator module.
// It describes the information object structure passed by the application
// for operational monitoring

local moo = import "moo.jsonnet";
local s = moo.oschema.schema("dunedaq.ethreadout.vdemulatorinfo");

local info = {
   uint8  : s.number("uint8", "u8",
                  doc="An unsigned of 8 bytes"),
   float8 : s.number("float8", "f8",
                  doc="A float of 8 bytes"),

   info: s.record("Info", [
       s.field("packets_sent", self.uint8, 0, doc="Number of packets sent"),
       s.field("packets_received", self.uint8, 0, doc="Number of received packets"),
       s.field("send_errors", self.uint8, 0, doc="Number of send errors")
   ], doc="VDEmulator information")
};

moo.oschema.sort_select(info)