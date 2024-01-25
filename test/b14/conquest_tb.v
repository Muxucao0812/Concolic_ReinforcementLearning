module conquest_tb();

    // Generated top module signals
    reg  clock;
    reg  reset;
    reg  [30:0] datai = 31'b0;
    wire [19:0] addr;
    wire [30:0] datao;
    wire rd;
    wire wr;
    reg  __obs;

    // Generated top module instance
    b14 _conc_top_inst(
            .clock     ( clock ),
            .reset     ( reset ),
            .datai     ( datai ),
            .addr      ( addr ),
            .datao     ( datao ),
            .rd        ( rd ),
            .wr        ( wr ),
            .__obs     ( __obs ));

    // Generated internal use signals
    reg  [31:0] _conc_pc;
    reg  [31:0] _conc_opcode;
    reg  [31:0] _conc_ram[0:500];


    // Generated clock pulse
    always begin
        #5 clock = ~clock;
    end

    // Generated program counter
    always @(posedge clock) begin
        _conc_opcode = _conc_ram[_conc_pc];
        __obs <= #5 _conc_opcode[31];
        datai <= #5 _conc_opcode[30:0];
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
        #1000 $finish;
    end

endmodule
