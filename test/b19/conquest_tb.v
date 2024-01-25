module conquest_tb();

    // Generated top module signals
    reg  clock;
    reg  reset;
    reg  bs = 1'b0;
    reg  na = 1'b0;
    reg  hold = 1'b0;
    reg  [10:0] in1 = 11'b0;
    reg  [10:0] in2 = 11'b0;
    reg  [19:0] in3 = 20'b0;
    wire [29:0] ris;
    reg  __obs;

    // Generated top module instance
    b19 _conc_top_inst(
            .clock     ( clock ),
            .reset     ( reset ),
            .bs        ( bs ),
            .na        ( na ),
            .hold      ( hold ),
            .in1       ( in1 ),
            .in2       ( in2 ),
            .in3       ( in3 ),
            .ris       ( ris ),
            .__obs     ( __obs ));

    // Generated internal use signals
    reg  [31:0] _conc_pc;
    reg  [45:0] _conc_opcode;
    reg  [45:0] _conc_ram[0:30];


    // Generated clock pulse
    always begin
        #5 clock = ~clock;
    end

    // Generated program counter
    always @(posedge clock) begin
        _conc_opcode = _conc_ram[_conc_pc];
        __obs <= #5 _conc_opcode[45];
        bs <= #5 _conc_opcode[0];
        hold <= #5 _conc_opcode[2];
        in1 <= #5 _conc_opcode[13:3];
        in2 <= #5 _conc_opcode[24:14];
        in3 <= #5 _conc_opcode[44:25];
        na <= #5 _conc_opcode[1];
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
        #300 $finish;
    end

endmodule
