module conquest_tb();

    // Generated top module signals
    reg  [5:0] x_in = 6'b0;
    reg  stbi = 1'b0;
    reg  clock;
    reg  reset;
    wire [5:0] x_out;
    reg  __obs;

    // Generated top module instance
    b11 _conc_top_inst(
            .x_in      ( x_in ),
            .stbi      ( stbi ),
            .clock     ( clock ),
            .reset     ( reset ),
            .x_out     ( x_out ),
            .__obs     ( __obs ));

    // Generated internal use signals
    reg  [31:0] _conc_pc;
    reg  [7:0] _conc_opcode;
    reg  [7:0] _conc_ram[0:10000];


    // Generated clock pulse
    always begin
        #5 clock = ~clock;
    end

    // Generated program counter
    always @(posedge clock) begin
        _conc_opcode = _conc_ram[_conc_pc];
        __obs <= #5 _conc_opcode[7];
        stbi <= #5 _conc_opcode[6];
        x_in <= #5 _conc_opcode[5:0];
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
        #100000 $finish;
    end

endmodule
