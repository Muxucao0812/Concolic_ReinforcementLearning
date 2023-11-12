#include <inttypes.h>
#include <string.h>
#include "concolic.h"
#include "smt_lib.h"

static SMTBinary* emit_expr_binary(ivl_scope_t scope, ivl_expr_t expr) {
	ivl_expr_t oper1 = ivl_expr_oper1(expr);
	ivl_expr_t oper2 = ivl_expr_oper2(expr);
	
	SMTBinary* smt_binary = new SMTBinary();
	smt_binary->is_signed = ivl_expr_signed(oper1) && ivl_expr_signed(oper2);
	
	SMTExpr* tmp_expr1 = NULL;
	SMTExpr* tmp_expr2 = NULL;
	smt_binary->set_opcode_from_expr(expr);
	const char *oper = "<invalid>";
	
	switch (ivl_expr_opcode(expr)) {
		case '+': oper = "+";
			break;
		case '-': oper = "-";
			break;
		case '*': oper = "*";
			break;
		case '/': oper = "/";
			break;
		case '%': oper = "%";
			break;
		case 'p': oper = "**";
			break;
		case 'E': oper = "===";
			break;
		case 'e': oper = "==";
			break;
		case 'N': oper = "!==";
			break;
		case 'n': oper = "!=";
			break;
		case '<': oper = "<";
			break;
		case 'L': oper = "<=";
			break;
		case '>': oper = ">";
			break;
		case 'G': oper = ">=";
			break;
		case '&': oper = "&";
			break;
		case '|': oper = "|";
			break;
		case '^': oper = "^";
			break;
		case 'A': oper = "&";
			break;
		case 'O': oper = "|";
			break;
		case 'X': oper = "~^";
			break;
		case 'a': oper = "&&";
			break;
		case 'o': oper = "||";
			break;
		case 'l': oper = "<<";
			break;
		case 'r': oper = ">>";
			break;
		case 'R': oper = ">>>";
			break;
		case 'm': oper = "min";
			break;
		case 'M': oper = "max";
			break;
	}

	fprintf(g_out, "(");
	switch (ivl_expr_opcode(expr)) {
		case '%':
			if (ivl_expr_value(expr) == IVL_VT_REAL) {
				fprintf(stderr, "%s:%u: vlog95 error: Real modulus operator "
						"is not supported.\n",
						ivl_expr_file(expr), ivl_expr_lineno(expr));
				g_errors += 1;
			}
		case '+':
		case '-':
		case '*':
		case '/':
			tmp_expr1 = emit_expr(scope, oper1);
			fprintf(g_out, " %s ", oper);
			tmp_expr2 = emit_expr(scope, oper2);
			break;
		case '&':
		case '|':
		case '^':
		case 'X':
			tmp_expr1 = emit_expr(scope, oper1);
			fprintf(g_out, " %s ", oper);
			tmp_expr2 = emit_expr(scope, oper2);
			break;
		case 'E':
		case 'e':
		case 'N':
		case 'n':
		case '<':
		case 'L':
		case '>':
		case 'G':
		case 'a':
		case 'o':
			tmp_expr1 = emit_expr(scope, oper1);
			fprintf(g_out, " %s ", oper);
			tmp_expr2 = emit_expr(scope, oper2);
			break;
		case 'R':
		case 'l':
		case 'r':
			tmp_expr1 = emit_expr(scope, oper1);
			fprintf(g_out, " %s ", oper);
			tmp_expr2 = emit_expr(scope, oper2);
			break;
		case 'A':
		case 'O':
			fprintf(g_out, "~(");
			tmp_expr1 = emit_expr(scope, oper1);
			fprintf(g_out, " %s ", oper);
			tmp_expr2 = emit_expr(scope, oper2);
			fprintf(g_out, ")");
			break;
		case 'p':
			error("TODO: opcode **");
            tmp_expr1 = emit_expr(scope, oper1);
            fprintf(g_out, " ** ");
            tmp_expr2 = emit_expr(scope, oper2);
			break;
			/* Convert the Verilog-A min() or max() functions. */
		case 'm':
		case 'M':
			error("$min/$max not supported");
			break;
		default:
			error("Unknown binary");
			emit_expr(scope, oper1);
			fprintf(g_out, "<unknown>");
			emit_expr(scope, oper2);
			fprintf(stderr, "%s:%u: vlog95 error: Unknown expression "
					"operator (%c).\n",
					ivl_expr_file(expr),
					ivl_expr_lineno(expr),
					ivl_expr_opcode(expr));
			g_errors += 1;
			break;
	}
	fprintf(g_out, ")");
	if(tmp_expr1 == NULL){
		tmp_expr1 = new SMTUnspecified();
	}
	if(tmp_expr2 == NULL){
		tmp_expr2 = new SMTUnspecified();
	}
	smt_binary->add(tmp_expr1);
	smt_binary->add(tmp_expr2);
	return smt_binary;
}

static SMTConcat* emit_expr_concat(ivl_scope_t scope, ivl_expr_t expr) {
	unsigned repeat = ivl_expr_repeat(expr);
	unsigned idx, count = ivl_expr_parms(expr);
	SMTConcat* smt_concat = new SMTConcat();

	if (repeat != 1) {
		fprintf(g_out, "{%u", repeat);
		smt_concat->repeat = repeat;
	}
	fprintf(g_out, "{");
	count -= 1;
	for (idx = 0; idx < count; idx += 1) {
		smt_concat->add(emit_expr(scope, ivl_expr_parm(expr, idx)));
		fprintf(g_out, ", ");
	}
	smt_concat->add(emit_expr(scope, ivl_expr_parm(expr, count)));
	fprintf(g_out, "}");
	if (repeat != 1) fprintf(g_out, "}");
    return smt_concat;
}

/*
 * A number can also be a parameter reference. If it is a parameter
 * reference then emit the appropriate parameter name instead of the
 * numeric value unless this is the actual parameter definition.
 */
static SMTNumber* emit_expr_number(ivl_expr_t expr) {
	return emit_number(ivl_expr_bits(expr), ivl_expr_width(expr), ivl_expr_signed(expr));
}

static SMTExpr* emit_select_name(ivl_scope_t scope, ivl_expr_t expr) {
	/* A select of a number is really a parameter select. */
	if (ivl_expr_type(expr) == IVL_EX_NUMBER) {
		error("Select on parameter not supported");
		ivl_parameter_t param = ivl_expr_parameter(expr);
		if (param) {
			emit_scope_call_path(scope, ivl_parameter_scope(param));
			emit_id(ivl_parameter_basename(param));
		} else {
			fprintf(g_out, "<missing>");
			fprintf(stderr, "%s:%u: vlog95 error: Unable to find "
					"parameter for select expression \n",
					ivl_expr_file(expr), ivl_expr_lineno(expr));
			g_errors += 1;
		}
		return NULL;
	} else {
		return emit_expr(scope, expr);
	}
}

static SMTExpr* emit_expr_select(ivl_scope_t scope, ivl_expr_t expr) {
	ivl_expr_t sel_expr = ivl_expr_oper2(expr);
	ivl_expr_t sig_expr = ivl_expr_oper1(expr);
	ivl_select_type_t sel_type = ivl_expr_sel_type(expr);
	/* If this is a dynamic array or queue select, translate the
	 * select differently. */
	if ((ivl_expr_type(sig_expr) == IVL_EX_SIGNAL) &&
			((ivl_signal_data_type(ivl_expr_signal(sig_expr)) == IVL_VT_DARRAY) ||
			(ivl_signal_data_type(ivl_expr_signal(sig_expr)) == IVL_VT_QUEUE))) {
		error("Dynamic array or queue not supported");
		assert(sel_expr);
		emit_select_name(scope, sig_expr);
		fprintf(g_out, "[");
		emit_expr(scope, sel_expr);
		fprintf(g_out, "]");
		return NULL;
	}
	
    ivl_expr_type_t sig_type = ivl_expr_type(sig_expr);
    if(sig_type == IVL_EX_SIGNAL){
        SMTSignal* smt_signal = NULL;
        if (sel_expr) {
            //smt_signal = new SMTSignal(ivl_expr_signal(sig_expr));
            unsigned width = ivl_expr_width(expr);
            assert(width > 0);
            
            /* A constant/parameter must be zero based in 1364-1995
             * so keep the compiler generated normalization. This
             * does not always work for selects before the parameter
             * since 1364-1995 does not support signed math. */
            int msb;
            int lsb;
            get_sig_msb_lsb(ivl_expr_signal(sig_expr), &msb, &lsb);
            /* A bit select. */
            if (width == 1) {
                //check_select_signed(sig_expr, sel_expr, msb, lsb);
                smt_signal = dynamic_cast<SMTSignal*>(emit_select_name(scope, sig_expr));
				assert(smt_signal);
                fprintf(g_out, "[");
                emit_scaled_expr(scope, sel_expr, msb, lsb, smt_signal);
                fprintf(g_out, "]");
            } else if (ivl_expr_type(sel_expr) == IVL_EX_NUMBER) {
                /* A constant part select. */
                smt_signal = dynamic_cast<SMTSignal*>(emit_select_name(scope, sig_expr));
				assert(smt_signal);
                emit_scaled_range(scope, sel_expr, width, msb, lsb, smt_signal);
            } else if (sel_type == IVL_SEL_OTHER) {
                error("select type other");
                /* A packed array access. */
                //alif: unsupported
                /*assert(lsb == 0);
                assert(msb >= 0);
                emit_expr_packed(scope, sig_expr, sel_expr, width);*/
            } else {
                error("select type other");
                /* An indexed part select. */
                //alif: unsupported
                /*check_select_signed(sig_expr, sel_expr, msb, lsb);
                emit_expr_ips(scope, sig_expr, sel_expr, sel_type,
                        width, msb, lsb, type == IVL_EX_NUMBER);*/
            }
        } else {
            //error("Select expression is null %s:%u", ivl_expr_file(expr), ivl_expr_lineno(expr));
            // HERE: Should this sign extend if the expression is signed?
            unsigned width = ivl_expr_width(expr);				//width of selection
            unsigned sig_wid = ivl_expr_width(sig_expr);		//total signal width
            smt_signal = dynamic_cast<SMTSignal*>(emit_expr(scope, sig_expr));
            assert(smt_signal);

            // Select part of a signal when needed.
            if (width < sig_wid) {
                int msb, lsb;
                int64_t value;
                get_sig_msb_lsb(ivl_expr_signal(sig_expr), &msb, &lsb);
                value = lsb;
                if (msb >= lsb) value += width - 1;
                else value -= width - 1;
                fprintf(g_out, "[%" PRId64":%u]", value, lsb);
                smt_signal->msb = value;
                smt_signal->lsb = lsb;
            }
        }
        return smt_signal;
    } else {
        return emit_expr(scope, sig_expr);
    }
}

static SMTSignal* emit_expr_signal(ivl_scope_t scope, ivl_expr_t expr) {
	ivl_signal_t sig = ivl_expr_signal(expr);
    SMTSignal* smt_signal = new SMTSignal(sig);
	emit_scope_call_path(scope, ivl_signal_scope(sig));
	emit_id(ivl_signal_basename(sig));
	
	if (ivl_signal_dimensions(sig)) {
		info("Signal dimension not zero");
		int lsb = ivl_signal_array_base(sig);
		int msb = lsb + ivl_signal_array_count(sig);
		fprintf(g_out, "[");
		emit_scaled_expr(scope, ivl_expr_oper1(expr), msb, lsb, smt_signal);
		fprintf(g_out, "]");
	}
	return smt_signal;
}

static SMTTernary* emit_expr_ternary(ivl_scope_t scope, ivl_expr_t expr) {
	SMTTernary* smt_ternary = new SMTTernary();
	ivl_expr_t oper2 = ivl_expr_oper2(expr);
	ivl_expr_t oper3 = ivl_expr_oper3(expr);
	fprintf(g_out, "(");
	smt_ternary->add(emit_expr(scope, ivl_expr_oper1(expr)));
	fprintf(g_out, " ? ");
	// HERE: Do these two emits need to use get_cast_width() like the binary
	//       arithmetic operators?
	smt_ternary->add(emit_expr(scope, oper2));
	fprintf(g_out, " : ");
	smt_ternary->add(emit_expr(scope, oper3));
	fprintf(g_out, ")");
	return smt_ternary;
}

static SMTUnary* emit_expr_unary(ivl_scope_t scope, ivl_expr_t expr) {
	SMTUnary* smt_unary = new SMTUnary();
	SMTExpr* tmp_expr = NULL;
	smt_unary->set_opcode(expr);
	const char *oper = "invalid";
	ivl_expr_t oper1 = ivl_expr_oper1(expr);
	switch (ivl_expr_opcode(expr)) {
		case '-': oper = "-";
			break;
		case '~': oper = "~";
			break;
		case '&': oper = "&";
			break;
		case '|': oper = "|";
			break;
		case '^': oper = "^";
			break;
		case 'A': oper = "~&";
			break;
		case 'N': oper = "~|";
			break;
		case 'X': oper = "~^";
			break;
		case '!': oper = "!";
			break;
	}
	switch (ivl_expr_opcode(expr)) {
		case '-':
		case '~':
		case '&':
		case '|':
		case '^':
		case 'A':
		case 'N':
		case 'X':
		case '!':
			fprintf(g_out, "(%s", oper);
			tmp_expr = emit_expr(scope, oper1);
			fprintf(g_out, ")");
			break;
		default:
			error("Unknown unary");
			fprintf(g_out, "<unknown>");
			emit_expr(scope, oper1);
			fprintf(stderr, "%s:%u: vlog95 error: Unknown unary "
					"operator (%c).\n",
					ivl_expr_file(expr),
					ivl_expr_lineno(expr),
					ivl_expr_opcode(expr));
			g_errors += 1;
			break;
	}
	if(tmp_expr == NULL){
		tmp_expr = new SMTUnspecified();
	}
	smt_unary->add(tmp_expr);
	return smt_unary;
}

static SMTExpr* emit_expr_string(ivl_expr_t expr){
	const char* str = ivl_expr_string(expr);
	fprintf(g_out, "\"%s\"", str);
	return new SMTString(str);
}

SMTExpr* emit_expr(ivl_scope_t scope, ivl_expr_t expr) {
	/* Emit the expression. */
	switch (ivl_expr_type(expr)) {
		//case IVL_EX_ARRAY:
		//	emit_expr_array(scope, expr, wid);
		//	break;
		case IVL_EX_BINARY:		//done
			return emit_expr_binary(scope, expr);
		case IVL_EX_CONCAT:		//done
			return emit_expr_concat(scope, expr);
		case IVL_EX_NUMBER:		//done
			return emit_expr_number(expr);
		case IVL_EX_SELECT:
			return emit_expr_select(scope, expr);
		case IVL_EX_SIGNAL:		//done
			return emit_expr_signal(scope, expr);
		case IVL_EX_TERNARY:	//done
			return emit_expr_ternary(scope, expr);
		case IVL_EX_UNARY:		//done
			return emit_expr_unary(scope, expr);
		case IVL_EX_STRING:
			return emit_expr_string(expr);
		default:
			fprintf(g_out, "<unsupported>");
			fprintf(stderr, "%s:%u: vlog95 error: Unsupported expression "
					"type (%d).\n",
					ivl_expr_file(expr),
					ivl_expr_lineno(expr),
					(int) ivl_expr_type(expr));
			g_errors += 1;
	}
	
	return NULL;
}
