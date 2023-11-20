module conquest_tb();

    // Generated top module signals
    reg  clk;
    reg  reset;
    reg  in = 1'b0;
    wire [7:0] out;
    reg  __obs;

    // Generated top module instance
    grammerTest _conc_top_inst(
            .clk       ( clk ),
            .reset     ( reset ),
            .in        ( in ),
            .out       ( out ),
            .__obs     ( __obs ));

    // Generated internal use signals
    reg  [31:0] _conc_pc;
    reg  [1:0] _conc_opcode;
    reg  [1:0] _conc_ram[0:10];


    // Generated clock pulse
    always begin
        #5 clk = ~clk;
    end

    // Generated program counter
    always @(posedge clk) begin
        _conc_opcode = _conc_ram[_conc_pc];
        __obs <= #5 _conc_opcode[1];
        in <= #5 _conc_opcode[0];
        _conc_pc = _conc_pc + 32'b1;
        $strobe(";_C %d", _conc_pc);
    end

    // Generated initial block
    initial begin
        clk = 1'b0;
        reset = 1'b0;
        _conc_pc = 32'b0;
        $readmemb("data.mem", _conc_ram);
        #2 clk = 1'b1;
        reset = 1'b1;
        #5 reset = 1'b0;
        #100 $finish;
    end

endmodule
