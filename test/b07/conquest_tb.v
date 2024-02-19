module conquest_tb();

    // Generated top module signals
    reg  clock;
    reg  reset;
    reg  start = 1'b0;
    wire [7:0] punti_retta;
    reg  __obs;

    // Generated top module instance
    b07 _conc_top_inst(
            .clock     ( clock ),
            .reset     ( reset ),
            .start     ( start ),
            .punti_retta( punti_retta ),
            .__obs     ( __obs ));

    // Generated internal use signals
    reg  [31:0] _conc_pc;
    reg  [1:0] _conc_opcode;
    reg  [1:0] _conc_ram[0:9];


    // Generated clock pulse
    always begin
        #5 clock = ~clock;
    end

    // Generated program counter
    always @(posedge clock) begin
        _conc_opcode = _conc_ram[_conc_pc];
        __obs <= #5 _conc_opcode[1];
        start <= #5 _conc_opcode[0];
        _conc_pc = _conc_pc + 32'b1;
        $strobe(";_C %d", _conc_pc);
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
        #10000 $finish;
    end

endmodule
