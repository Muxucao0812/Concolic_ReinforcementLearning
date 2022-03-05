module conquest_tb();

    // Generated top module signals
    reg  eql = 1'b0;
    reg  clock;
    reg  reset;
    reg  cont_eql = 1'b0;
    wire [1:0] cc_mux;
    wire [1:0] uscite;
    wire enable_count;
    wire ackout;
    reg  __obs;

    // Generated top module instance
    b06 _conc_top_inst(
            .eql       ( eql ),
            .clock     ( clock ),
            .reset     ( reset ),
            .cont_eql  ( cont_eql ),
            .cc_mux    ( cc_mux ),
            .uscite    ( uscite ),
            .enable_count( enable_count ),
            .ackout    ( ackout ),
            .__obs     ( __obs ));

    // Generated internal use signals
    reg  [31:0] _conc_pc;
    reg  [2:0] _conc_opcode;
    reg  [2:0] _conc_ram[0:10];


    // Generated clock pulse
    always begin
        #5 clock = ~clock;
    end

    // Generated program counter
    always @(posedge clock) begin
        _conc_opcode = _conc_ram[_conc_pc];
        __obs <= #5 _conc_opcode[2];
        cont_eql <= #5 _conc_opcode[1];
        eql <= #5 _conc_opcode[0];
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
        #10000 $finish;
    end

endmodule
