module sv_scoped_taskcall0;

  initial begin
    mypkg::do_it(1, 2);
    mypkg::reset();
    mypkg::tick;
  end

endmodule
