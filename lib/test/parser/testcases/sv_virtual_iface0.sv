module sv_virtual_iface0;

  virtual handshake_if v0;
  virtual interface handshake_if v1;
  virtual handshake_if #(.WIDTH(8)) v2;
  virtual handshake_if.master v3;
  virtual handshake_if #(8) .slave v4;

endmodule
