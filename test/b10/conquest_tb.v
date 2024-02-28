module conquest_tb();

    // Generated top module signals
    reg  r_button = 1'b0;
    reg  g_button = 1'b0;
    reg  key = 1'b0;
    reg  start = 1'b0;
    reg  reset;
    reg  test = 1'b0;
    wire cts;
    wire ctr;
    reg  rts = 1'b0;
    reg  rtr = 1'b0;
    reg  clock;
    reg  [3:0] v_in = 4'b0;
    wire [3:0] v_out;
    reg  __obs;

    // Generated top module instance
    b10 _conc_top_inst(
            .r_button  ( r_button ),
            .g_button  ( g_button ),
            .key       ( key ),
            .start     ( start ),
            .reset     ( reset ),
            .test      ( test ),
            .cts       ( cts ),
            .ctr       ( ctr ),
            .rts       ( rts ),
            .rtr       ( rtr ),
            .clock     ( clock ),
            .v_in      ( v_in ),
            .v_out     ( v_out ),
            .__obs     ( __obs ));

    // Generated internal use signals
    reg  [31:0] _conc_pc;
    reg  [11:0] _conc_opcode;
    reg  [11:0] _conc_ram[0:10];


    // Generated clock pulse
    always begin
        #5 clock = ~clock;
    end

    // Generated program counter
    always @(posedge clock) begin
        _conc_opcode = _conc_ram[_conc_pc];
        __obs <= #5 _conc_opcode[11];
        g_button <= #5 _conc_opcode[1];
        key <= #5 _conc_opcode[2];
        r_button <= #5 _conc_opcode[0];
        rtr <= #5 _conc_opcode[6];
        rts <= #5 _conc_opcode[5];
        start <= #5 _conc_opcode[3];
        test <= #5 _conc_opcode[4];
        v_in <= #5 _conc_opcode[10:7];
        _conc_pc = _conc_pc + 32'b1;
        $strobe(";_C %d", _conc_pc);
        $strobe(";_Input %b", _conc_opcode);
    end

    // Generated initial block
    initial begin
        clock = 1'b0;
        reset = 1'b0;
        _conc_pc = 32'b0;
        $readmemb("data.mem", _conc_ram);
        #2 clock = 1'b1;
        reset = 1'b1;
        #5 reset = 1'b0;
        #12000 $finish;
    end

endmodule
