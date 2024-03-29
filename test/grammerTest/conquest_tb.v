module conquest_tb();

    // Generated top module signals
    reg  clk;
    reg  reset;
    reg  [31:0] in = 32'b0;
    reg  [3:0] count1 = 4'b0;
    reg  [3:0] count = 4'b0;
    reg  register = 1'b0;
    wire [31:0] out;
    wire [31:0] out1;
    reg  __obs;

    // Generated top module instance
    grammerTest _conc_top_inst(
            .clk       ( clk ),
            .reset     ( reset ),
            .in        ( in ),
            .count1    ( count1 ),
            .count     ( count ),
            .register  ( register ),
            .out       ( out ),
            .out1      ( out1 ),
            .__obs     ( __obs ));

    // Generated internal use signals
    reg  [31:0] _conc_pc;
    reg  [41:0] _conc_opcode;
    reg  [41:0] _conc_ram[0:0];


    // Generated clock pulse
    always begin
        #5 clk = ~clk;
    end

    // Generated program counter
    always @(posedge clk) begin
        _conc_opcode = _conc_ram[_conc_pc];
        __obs <= #5 _conc_opcode[41];
        count <= #5 _conc_opcode[39:36];
        count1 <= #5 _conc_opcode[35:32];
        in <= #5 _conc_opcode[31:0];
        register <= #5 _conc_opcode[40];
        _conc_pc = _conc_pc + 32'b1;
        $strobe(";_C %d", _conc_pc);
        $strobe(";_Input %b", _conc_opcode);
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
        #120 $finish;
    end

endmodule
