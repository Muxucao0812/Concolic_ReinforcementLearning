#include <stdlib.h>
#include <string.h>
#include "config.h"
#include "concolic.h"
#include "smt_lib.h"


using namespace std;

static ivl_signal_t nexus_is_signal(ivl_scope_t scope, ivl_nexus_t nex,
		int*base, unsigned*array_word);

static unsigned is_local_nexus(ivl_scope_t scope, ivl_nexus_t nex) {
	unsigned idx, count = ivl_nexus_ptrs(nex);
	unsigned is_local = 1;
	unsigned has_output_driver = 0;
	for (idx = 0; idx < count; idx += 1) {
		ivl_nexus_ptr_t nex_ptr = ivl_nexus_ptr(nex, idx);
		ivl_signal_t sig = ivl_nexus_ptr_sig(nex_ptr);
		if (!sig) continue;
		/* Check to see if there is an output port driving into
		 * the local scope. */
		if ((scope == ivl_scope_parent(ivl_signal_scope(sig))) &&
				(ivl_signal_port(sig) == IVL_SIP_OUTPUT)) {
			has_output_driver = 1;
			continue;
		}
		if (scope != ivl_signal_scope(sig)) continue;
		if ((ivl_nexus_ptr_drive1(nex_ptr) != IVL_DR_HiZ) ||
				(ivl_nexus_ptr_drive0(nex_ptr) != IVL_DR_HiZ)) continue;
		if (ivl_signal_local(sig)) {
			is_local = 1;
		} else {
			is_local = 0;
			break;
		}
	}
	/* We return is_local=true only if there is not an output driving
	 * into this scope. This is needed since some module outputs are
	 * combined with a concatenation. */
	return is_local && !has_output_driver;
}

/*
 * This returns the nexus of the LPM if it is not a local signal.
 */
static ivl_nexus_t get_lpm_output(ivl_scope_t scope, ivl_lpm_t lpm) {
	ivl_nexus_t output = 0;
	switch (ivl_lpm_type(lpm)) {
		case IVL_LPM_ABS:
		case IVL_LPM_ADD:
		case IVL_LPM_ARRAY:
		case IVL_LPM_CAST_INT:
		case IVL_LPM_CAST_INT2:
		case IVL_LPM_CAST_REAL:
		case IVL_LPM_CMP_EEQ:
		case IVL_LPM_CMP_EQ:
		case IVL_LPM_CMP_EQX:
		case IVL_LPM_CMP_EQZ:
		case IVL_LPM_CMP_GE:
		case IVL_LPM_CMP_GT:
		case IVL_LPM_CMP_NE:
		case IVL_LPM_CMP_NEE:
		case IVL_LPM_CONCAT:
		case IVL_LPM_CONCATZ:
		case IVL_LPM_DIVIDE:
		case IVL_LPM_FF:
		case IVL_LPM_MOD:
		case IVL_LPM_MULT:
		case IVL_LPM_MUX:
		case IVL_LPM_PART_PV:
		case IVL_LPM_PART_VP:
		case IVL_LPM_POW:
		case IVL_LPM_RE_AND:
		case IVL_LPM_RE_NAND:
		case IVL_LPM_RE_NOR:
		case IVL_LPM_RE_OR:
		case IVL_LPM_RE_XOR:
		case IVL_LPM_RE_XNOR:
		case IVL_LPM_REPEAT:
		case IVL_LPM_SFUNC:
		case IVL_LPM_SHIFTL:
		case IVL_LPM_SHIFTR:
		case IVL_LPM_SIGN_EXT:
		case IVL_LPM_SUB:
		case IVL_LPM_SUBSTITUTE:
		case IVL_LPM_UFUNC:
			/* If the output of this LPM is a local signal then something
			 * else will request that this be emitted. */
			output = ivl_lpm_q(lpm);
			if (is_local_nexus(scope, output)) return 0;
			break;
		default:
			// HERE: Can this be a simple assert at some point in time?
			fprintf(g_out, "<unknown>");
			fprintf(stderr, "%s:%u: vlog95 error: Unknown LPM type (%d).\n",
					ivl_lpm_file(lpm), ivl_lpm_lineno(lpm),
					(int) ivl_lpm_type(lpm));
			g_errors += 1;
			return 0;
	}
	return output;
}

static SMTExpr* emit_logic_as_ca(ivl_scope_t scope, ivl_net_logic_t nlogic);
static SMTExpr* emit_lpm_as_ca(ivl_scope_t scope, ivl_lpm_t lpm,
		unsigned sign_extend);

/* For an undriven port look for the local signal to get the nexus name. */
static void emit_nexus_port_signal(ivl_scope_t scope, ivl_nexus_t nex) {
	unsigned idx, count = ivl_nexus_ptrs(nex);
	ivl_signal_t sig = 0;
	for (idx = 0; idx < count; idx += 1) {
		ivl_nexus_ptr_t nex_ptr = ivl_nexus_ptr(nex, idx);
		ivl_signal_t t_sig = ivl_nexus_ptr_sig(nex_ptr);
		if ((ivl_nexus_ptr_drive1(nex_ptr) != IVL_DR_HiZ) ||
				(ivl_nexus_ptr_drive0(nex_ptr) != IVL_DR_HiZ)) assert(0);
		if (t_sig) {
			if (scope != ivl_signal_scope(t_sig)) continue;
			assert(!sig);
			sig = t_sig;
		}
	}
	/* There will not be a signal for an empty port. */
	if (sig) emit_nexus_as_ca(scope, ivl_signal_nex(sig, 0), 0, 0);
	else fprintf(g_out, "/* Empty */");
}

static ivl_signal_t find_local_signal(ivl_scope_t scope, ivl_nexus_t nex,
		unsigned *word) {
	unsigned idx, count = ivl_nexus_ptrs(nex);
	ivl_signal_t sig = 0;
	*word = 0;
	for (idx = 0; idx < count; idx += 1) {
		ivl_nexus_ptr_t nex_ptr = ivl_nexus_ptr(nex, idx);
		ivl_signal_t t_sig = ivl_nexus_ptr_sig(nex_ptr);
		if (!t_sig) continue;
		if (ivl_signal_local(t_sig) &&
				(ivl_signal_port(t_sig) != IVL_SIP_INPUT)) continue;
		if (ivl_signal_scope(t_sig) != scope) continue;
		assert(!sig);
		sig = t_sig;
		*word = ivl_nexus_ptr_pin(nex_ptr);
	}
	return sig;
}

/*
 * Emit the input port driving expression.
 */
void emit_nexus_port_driver_as_ca(ivl_scope_t scope, ivl_nexus_t nex) {
	unsigned idx, count = ivl_nexus_ptrs(nex);
	ivl_lpm_t lpm = 0;
	ivl_net_const_t net_const = 0;
	ivl_net_logic_t nlogic = 0;
	ivl_signal_t sig = 0;
	unsigned word = 0;
	/* Look for the nexus driver. */
	for (idx = 0; idx < count; idx += 1) {
		ivl_nexus_ptr_t nex_ptr = ivl_nexus_ptr(nex, idx);
		ivl_lpm_t t_lpm = ivl_nexus_ptr_lpm(nex_ptr);
		ivl_net_const_t t_net_const = ivl_nexus_ptr_con(nex_ptr);
		ivl_net_logic_t t_nlogic = ivl_nexus_ptr_log(nex_ptr);
		ivl_signal_t t_sig = ivl_nexus_ptr_sig(nex_ptr);
		if ((ivl_nexus_ptr_drive1(nex_ptr) == IVL_DR_HiZ) &&
				(ivl_nexus_ptr_drive0(nex_ptr) == IVL_DR_HiZ)) continue;
		if (t_lpm) {
			assert(!lpm);
			lpm = t_lpm;
		}
		if (t_net_const) {
			assert(!net_const);
			net_const = t_net_const;
		}
		if (t_nlogic) {
			assert(!nlogic);
			nlogic = t_nlogic;
		}
		if (t_sig) {
			assert(!sig);
			sig = t_sig;
			word = ivl_nexus_ptr_pin(nex_ptr);
		}
	}
	/* An LPM is driving the nexus. */
	if (lpm) {
		assert(!net_const);
		assert(!nlogic);
		assert(!sig);
		/* If there is a signal in this scope that is also driven by
		 * the LPM then use the signal instead. */
		sig = find_local_signal(scope, ivl_lpm_q(lpm), &word);
		if (sig) emit_nexus_as_ca(scope, ivl_signal_nex(sig, word), 0, 0);
		else emit_lpm_as_ca(scope, lpm, 0);
		/* A constant is driving the nexus. */
	} else if (net_const) {
		assert(!nlogic);
		assert(!sig);
		/* If there is a signal in this scope that is also driven by
		 * the constant then use the signal instead. */
		sig = find_local_signal(scope, ivl_const_nex(net_const), &word);
		if (sig) emit_nexus_as_ca(scope, ivl_signal_nex(sig, word), 0, 0);
		else emit_const_nexus(scope, net_const);
		/* A logic gate is driving the nexus. */
	} else if (nlogic) {
		assert(!sig);
		/* If there is a signal in this scope that is also driven by
		 * the logic then use the signal instead. */
		sig = find_local_signal(scope, ivl_logic_pin(nlogic, 0), &word);
		if (sig) emit_nexus_as_ca(scope, ivl_signal_nex(sig, word), 0, 0);
		else emit_logic_as_ca(scope, nlogic);
		/* A signal is driving the nexus. */
	} else if (sig) {
		emit_nexus_as_ca(scope, ivl_signal_nex(sig, word), 0, 0);
		/* If there is no driver then look for a single signal that is
		 * driven by this nexus that has the correct scope. This is needed
		 * to translate top level ports. */
	} else {
		emit_nexus_port_signal(scope, nex);
	}
}

SMTExpr* emit_nexus_as_ca(ivl_scope_t scope, ivl_nexus_t nex, unsigned allow_UD, uint sign_extend) {
	/* If there is no nexus then there is nothing to print. */
	if (!nex) return NULL;
	/* A local nexus only has a single driver. */
	if (is_local_nexus(scope, nex)) {
		unsigned idx, count = ivl_nexus_ptrs(nex);
		unsigned must_be_sig = 0;
		unsigned out_of_scope_drive = 0;
		ivl_lpm_t lpm = 0;
		ivl_net_const_t net_const = 0;
		ivl_net_logic_t nlogic = 0;
		ivl_signal_t sig = 0;
		unsigned word = 0;
		for (idx = 0; idx < count; idx += 1) {
			ivl_nexus_ptr_t nex_ptr = ivl_nexus_ptr(nex, idx);
			ivl_lpm_t t_lpm = ivl_nexus_ptr_lpm(nex_ptr);
			ivl_net_const_t t_net_const = ivl_nexus_ptr_con(nex_ptr);
			ivl_net_logic_t t_nlogic = ivl_nexus_ptr_log(nex_ptr);
			ivl_signal_t t_sig = ivl_nexus_ptr_sig(nex_ptr);
			if ((ivl_nexus_ptr_drive1(nex_ptr) == IVL_DR_HiZ) &&
					(ivl_nexus_ptr_drive0(nex_ptr) == IVL_DR_HiZ)) {
				/* If we only have a single input then we want
				 * the nexus this signal is driven by. */
				if (count == 1) {
					must_be_sig = 1;
				} else continue;
			}
			if (t_lpm) {
				assert(!lpm);
				lpm = t_lpm;
			}
			if (t_net_const) {
				if (scope != ivl_const_scope(t_net_const)) {
					// HERE: Need to verify that this is not a parameter
					out_of_scope_drive = 1;
				}
				assert(!net_const);
				net_const = t_net_const;
			}
			if (t_nlogic) {
				assert(!nlogic);
				nlogic = t_nlogic;
			}
			if (t_sig) {
				assert(!sig);
				sig = t_sig;
				word = ivl_nexus_ptr_pin(nex_ptr);
			}
		}
		if (lpm) {
			assert(!net_const);
			assert(!nlogic);
			assert(!sig);
			assert(!must_be_sig);
			// HERE: I think we need special input code like the following.
#if 0
			/* If there is a signal in this scope that is also driven by
			 * the LPM then use the signal instead. */
			sig = find_local_signal(scope, ivl_lpm_q(lpm), &word);
			if (sig) emit_nexus_as_ca(scope, ivl_signal_nex(sig, word), 0, 0);
			else emit_lpm_as_ca(scope, lpm, sign_extend);
#endif
			return emit_lpm_as_ca(scope, lpm, sign_extend);
		} else if (net_const) {
			assert(!nlogic);
			assert(!sig);
			assert(!must_be_sig);
			if (out_of_scope_drive) {
				// HERE: An out of scope const drive that is not a parameter is really a
				//       port so look for and emit the local signal name (nexus_is_signal
				//       may work). The is_local_nexus code also needs to be changed to
				//       not emit the port expressions as a CA. Make sure this works
				//       correctly if the parameter is passed as a port argument.
				// For now report this as missing.
				fprintf(g_out, "<missing>");
			} else {
				return emit_const_nexus(scope, net_const);
			}
		} else if (nlogic) {
			assert(!sig);
			assert(!must_be_sig);
			return emit_logic_as_ca(scope, nlogic);
		} else if (sig) {
			// HERE: should these be allow_UD?
			if (must_be_sig) {
				return emit_nexus_as_ca(scope, ivl_signal_nex(sig, word), 0, 0);
			} else {
				return emit_name_of_nexus(scope, nex, 0);
			}
			// HERE: The assert causes pr1703959 to fail.
			//	    } else assert(0);
		} else {
			fprintf(stderr, "?:?: vlog95 error: Could not emit "
					"nexus as a CA.\n");
			g_errors += 1;
			fprintf(g_out, "<missing>");
		}
	} else {
		return emit_name_of_nexus(scope, nex, allow_UD);
	}
	return new SMTUnspecified();
}

static SMTNumber* emit_pull_const(char value, unsigned width) {
	char *val = new char[width+1];
	for (uint idx = 0; idx < width; idx += 1) {
		val[idx] = value;
	}
	
	SMTNumber* smt_num = new SMTNumber(val, width, false);
	smt_num->emit_verilog_value();
	return smt_num;
}

static SMTExpr* emit_logic_as_ca(ivl_scope_t scope, ivl_net_logic_t nlogic) {
	unsigned inputs = ivl_logic_pins(nlogic) - 1;
	char opcode = 'i';
	bool is_inverted = false;
	switch (ivl_logic_type(nlogic)) {
		case IVL_LO_BUF:
		case IVL_LO_BUFT:
		case IVL_LO_BUFZ:
			assert(inputs == 1);
			return emit_nexus_as_ca(scope, ivl_logic_pin(nlogic, 1), 0, 0);
		case IVL_LO_NOT:
		{
			assert(inputs == 1);
			SMTUnary* smt_un = new SMTUnary();
			smt_un->set_opcode('~');
			fprintf(g_out, "(~ ");
			smt_un->add(emit_nexus_as_ca(scope, ivl_logic_pin(nlogic, 1), 0, 0));
			fprintf(g_out, ")");
			return smt_un;
		}
		case IVL_LO_AND:
			opcode = '&';
			break;
		case IVL_LO_NAND:
			opcode = '&';
			is_inverted = true;
			break;
		case IVL_LO_NOR:
			opcode = '|';
			is_inverted = true;
			break;
		case IVL_LO_OR:
			opcode = '|';
			break;
		case IVL_LO_XNOR:
			opcode = '^';
			is_inverted = true;
			break;
		case IVL_LO_XOR:
			opcode = '^';
			break;
		case IVL_LO_PULLDOWN:
			/* A pull up/down at this point has been turned into an assignment
			 * with strength so just emit the appropriate constant. */
			return emit_pull_const('0', ivl_logic_width(nlogic));
		case IVL_LO_PULLUP:
			return emit_pull_const('1', ivl_logic_width(nlogic));
		default:
			fprintf(g_out, "<unknown>");
			fprintf(stderr, "%s:%u: vlog95 error: Unknown CA logic type "
					"(%d).\n",
					ivl_logic_file(nlogic), ivl_logic_lineno(nlogic),
					(int) ivl_logic_type(nlogic));
			g_errors += 1;
	}
	
	SMTLogic* smt_logic = new SMTLogic();
	smt_logic->set_opcode(opcode, is_inverted);
	if(is_inverted){
		fprintf(g_out, "~");
	}
	fprintf(g_out, "(");
	smt_logic->add(emit_nexus_as_ca(scope, ivl_logic_pin(nlogic, 1), 0, 0));
	for(uint index = 2; index <= inputs; index++){
		fprintf(g_out, " %c ", opcode);
		smt_logic->add(emit_nexus_as_ca(scope, ivl_logic_pin(nlogic, index), 0, 0));
	}
	fprintf(g_out, ")");
	return smt_logic;
}

static SMTConcat* emit_lpm_concat(ivl_scope_t scope, ivl_lpm_t lpm) {
	SMTConcat* smt_concat = new SMTConcat();
	unsigned idx, count = ivl_lpm_size(lpm);
	ivl_nexus_t nex;
	fprintf(g_out, "{");
	// HERE: Need to check for a zero repeat that was dropped from the concat.
	/* Check to see if this is a repeat. */
	nex = ivl_lpm_data(lpm, 0);
	for (idx = 1; idx < count; idx += 1) {
		if (nex != ivl_lpm_data(lpm, idx)) break;
	}
	/* If all the nexus match then we have a repeat. */
	if ((idx == count) && (count > 1)) {
		smt_concat->repeat = count;
		fprintf(g_out, "%u{", count);
		smt_concat->add(emit_nexus_as_ca(scope, ivl_lpm_data(lpm, 0), 0, 0));
		fprintf(g_out, "}");
		/* Icarus uses a concat to combine the output from multiple devices
		 * into a single vector, because of this we need to also look for
		 * the nexus driver outside the scope. emit_nexus_as_ca( , , 1, ) */
	} else {
		for (idx = count - 1; idx > 0; idx -= 1) {
			smt_concat->add(emit_nexus_as_ca(scope, ivl_lpm_data(lpm, idx), 1, 0));
			fprintf(g_out, ", ");
		}
		smt_concat->add(emit_nexus_as_ca(scope, ivl_lpm_data(lpm, 0), 1, 0));
	}
	fprintf(g_out, "}");
	return smt_concat;
}

/*
 * Look for an output signal in the nexus that is driving into this scope.
 */
static ivl_signal_t find_output_signal(ivl_scope_t scope, ivl_nexus_t nex,
		unsigned*array_word) {
	unsigned idx, count = ivl_nexus_ptrs(nex);
	(void) array_word; /* Parameter is not used. */
	for (idx = 0; idx < count; idx += 1) {
		ivl_nexus_ptr_t nex_ptr = ivl_nexus_ptr(nex, idx);
		ivl_signal_t t_sig = ivl_nexus_ptr_sig(nex_ptr);
		if (!t_sig) continue;
		/* The signal must not be a driver. */
		if ((ivl_nexus_ptr_drive1(nex_ptr) != IVL_DR_HiZ) ||
				(ivl_nexus_ptr_drive0(nex_ptr) != IVL_DR_HiZ)) continue;
		/* The signal must be an output. */
		if (ivl_signal_port(t_sig) != IVL_SIP_OUTPUT) continue;
		/* The signal must be driving into this scope. */
		if (ivl_scope_parent(ivl_signal_scope(t_sig)) == scope) {
			return t_sig;
		}
	}
	return 0;
}

static ivl_signal_t nexus_is_signal(ivl_scope_t scope, ivl_nexus_t nex,
		int*base, unsigned*array_word) {
	unsigned idx, count = ivl_nexus_ptrs(nex);
	ivl_lpm_t lpm = 0;
	ivl_net_const_t net_const = 0;
	ivl_net_logic_t nlogic = 0;
	ivl_signal_t sig;
	/* Look for a signal in the local scope first. */
	sig = find_local_signal(scope, nex, array_word);
	if (sig) return sig;
	/* Now look for an output signal driving into the local scope. */
	sig = find_output_signal(scope, nex, array_word);
	if (sig) return sig;
	/* Now scan the nexus looking for a driver. */
	for (idx = 0; idx < count; idx += 1) {
		ivl_nexus_ptr_t nex_ptr = ivl_nexus_ptr(nex, idx);
		ivl_lpm_t t_lpm = ivl_nexus_ptr_lpm(nex_ptr);
		ivl_net_const_t t_net_const = ivl_nexus_ptr_con(nex_ptr);
		ivl_net_logic_t t_nlogic = ivl_nexus_ptr_log(nex_ptr);
		ivl_signal_t t_sig = ivl_nexus_ptr_sig(nex_ptr);
		if ((ivl_nexus_ptr_drive1(nex_ptr) == IVL_DR_HiZ) &&
				(ivl_nexus_ptr_drive0(nex_ptr) == IVL_DR_HiZ)) continue;
		if (t_lpm) {
			assert(!lpm);
			/* The real signal could be hidden behind a select. */
			if (ivl_lpm_type(t_lpm) == IVL_LPM_PART_VP) {
				t_sig = nexus_is_signal(scope, ivl_lpm_data(t_lpm, 0),
						base, array_word);
			}

			if (t_sig) *base += ivl_lpm_base(t_lpm);
			else lpm = t_lpm;
		}
		if (t_net_const) {
			assert(!net_const);
			net_const = t_net_const;
		}
		if (t_nlogic) {
			assert(!nlogic);
			/* The real signal could be hidden behind a BUFZ gate. */
			if (ivl_logic_type(t_nlogic) == IVL_LO_BUFZ) {
				assert(ivl_logic_pins(t_nlogic) == 2);
				t_sig = nexus_is_signal(scope,
						ivl_logic_pin(t_nlogic, 1),
						base, array_word);
			} else nlogic = t_nlogic;
		}
		if (t_sig) {
			assert(!sig);
			sig = t_sig;
			/* This could be an array word so save the word. */
			*array_word = ivl_nexus_ptr_pin(nex_ptr);
		}
	}
	return sig;
}

static SMTExpr* emit_lpm_part_select(ivl_scope_t scope, ivl_lpm_t lpm, uint sign_extend) {
	unsigned width = ivl_lpm_width(lpm);
	unsigned array_word = 0;
	int base = ivl_lpm_base(lpm);
	int msb, lsb;
	ivl_signal_t sig = nexus_is_signal(scope, ivl_lpm_data(lpm, 0),
			&base, &array_word);

	// HERE: variable parameter select needs to be rebuilt.
	if (!sig) {
		if(base){
			error("check: emit_lpm_part_select (%s:%u)", ivl_lpm_file(lpm), ivl_lpm_lineno(lpm));
		}
		/* Check if the compiler used a select for a shift. */
		assert(base >= 0);
		if (base) fprintf(g_out, "(");
		SMTExpr* smt_expr = emit_nexus_as_ca(scope, ivl_lpm_data(lpm, 0), 0, 0);
		if (base) {
			fprintf(g_out, " ");
			if (sign_extend) fprintf(g_out, ">");
			fprintf(g_out, ">> %d)", base);
		}
		return smt_expr;
	}

	//if (sign_extend) fprintf(g_out, "(");
	emit_scope_call_path(scope, ivl_signal_scope(sig));
	emit_id(ivl_signal_basename(sig));
	if (ivl_signal_dimensions(sig)) {
		error("Arrays currently not supported");
		int array_idx = (int) array_word + ivl_signal_array_base(sig);
		fprintf(g_out, "[%d]", array_idx);
	}
    SMTSignal* smt_sig = new SMTSignal(sig);
	get_sig_msb_lsb(sig, &msb, &lsb);
	//if (sign_extend) {
	//	assert(base != lsb);
	//	if (msb >= lsb) base += lsb;
	//	else base = lsb - base;
	//	fprintf(g_out, " >>> %d)", base);
	//	return;
	//}

	if (width == 1) {
		ivl_nexus_t sel = ivl_lpm_data(lpm, 1);
		if (sel) {
			error("check: variable part select?");
			fprintf(g_out, "[");
			// HERE: Need to scale the select nexus.
			if ((msb >= lsb) && (lsb == 0)) {
				emit_nexus_as_ca(scope, sel, 0, 0);
			} else {
				fprintf(stderr, "%s:%u: vlog95 sorry: Non-zero based "
						"variable part selects are not "
						"supported.\n", ivl_lpm_file(lpm),
						ivl_lpm_lineno(lpm));
				g_errors += 1;
				fprintf(g_out, "<missing>");
			}
		} else {
			/* Skip a select of the entire bit. */
			if ((msb == lsb) && (lsb == base)) {
				smt_sig->msb = 0;
				smt_sig->lsb = 0;
				assert(smt_sig->parent->width == 1);
				return smt_sig;
			}
			fprintf(g_out, "[");
			if (msb >= lsb) {
				base += lsb;
			}
			else {
				base = lsb - base;
			}
			fprintf(g_out, "%d", base);
			smt_sig->msb = base;
			smt_sig->lsb = base;
		}
	} else {
		fprintf(g_out, "[");
		// HERE: No support for an indexed part select.
		ivl_nexus_t sel = ivl_lpm_data(lpm, 1);
		if (sel) {
			fprintf(stderr, "%s:%u: vlog95 sorry: Variable indexed part "
					"selects are not supported.\n",
					ivl_lpm_file(lpm), ivl_lpm_lineno(lpm));
			g_errors += 1;
			fprintf(g_out, "<missing>:<missing>");
		} else {
			if (msb >= lsb) {
				base += lsb;
				fprintf(g_out, "%d:%d", base + (int) width - 1, base);
				smt_sig->msb = base + width - 1;
				smt_sig->lsb = base;
			} else {
				base = lsb - base;
				fprintf(g_out, "%d:%d", base - (int) width + 1, base);
				smt_sig->msb = base - width + 1;
				smt_sig->lsb = base;
			}
		}
	}
	fprintf(g_out, "]");
	
	return smt_sig;
}

static SMTExpr* emit_lpm_as_ca(ivl_scope_t scope, ivl_lpm_t lpm, uint sign_extend) {
	switch (ivl_lpm_type(lpm)) {
		case IVL_LPM_ADD:
		{
			SMTBinary* smt_bi = new SMTBinary();
			smt_bi->set_opcode('+');
			fprintf(g_out, "(");
			smt_bi->add(emit_nexus_as_ca(scope, ivl_lpm_data(lpm, 0), 0, 0));
			fprintf(g_out, " + ");
			smt_bi->add(emit_nexus_as_ca(scope, ivl_lpm_data(lpm, 1), 0, 0));
			fprintf(g_out, ")");
			return smt_bi;
		}
		//case IVL_LPM_ARRAY:
		//	emit_lpm_array(scope, lpm);
		//	break;
		//case IVL_LPM_CAST_INT:
		//case IVL_LPM_CAST_INT2:
		//case IVL_LPM_CAST_REAL:
		//	emit_nexus_as_ca(scope, ivl_lpm_data(lpm, 0), 0, 0);
		//	break;
		case IVL_LPM_CMP_EEQ:
            error("check: IVL_LPM_CMP_EEQ");
			fprintf(g_out, "(");
			emit_nexus_as_ca(scope, ivl_lpm_data(lpm, 0), 0, 0);
			fprintf(g_out, " === ");
			emit_nexus_as_ca(scope, ivl_lpm_data(lpm, 1), 0, 0);
			fprintf(g_out, ")");
			break;
		case IVL_LPM_CMP_EQ:
        {
            SMTBinary* smt_bi = new SMTBinary();
            smt_bi->set_opcode('e');
			fprintf(g_out, "(");
			smt_bi->add(emit_nexus_as_ca(scope, ivl_lpm_data(lpm, 0), 0, 0));
			fprintf(g_out, " == ");
			smt_bi->add(emit_nexus_as_ca(scope, ivl_lpm_data(lpm, 1), 0, 0));
			fprintf(g_out, ")");
			return smt_bi;
        }
		case IVL_LPM_CMP_GE:
        {
            SMTBinary* smt_bi = new SMTBinary();
			smt_bi->is_signed = ivl_lpm_signed(lpm);
            smt_bi->set_opcode('G');
			fprintf(g_out, "(");
			smt_bi->add(emit_nexus_as_ca(scope, ivl_lpm_data(lpm, 0), 0, 0));
			fprintf(g_out, " >= ");
			smt_bi->add(emit_nexus_as_ca(scope, ivl_lpm_data(lpm, 1), 0, 0));
			fprintf(g_out, ")");
			return smt_bi;
        }
		case IVL_LPM_CMP_GT:
        {
            SMTBinary* smt_bi = new SMTBinary();
			smt_bi->is_signed = ivl_lpm_signed(lpm);
            smt_bi->set_opcode('>');
			fprintf(g_out, "(");
			smt_bi->add(emit_nexus_as_ca(scope, ivl_lpm_data(lpm, 0), 0, 0));
			fprintf(g_out, " > ");
			smt_bi->add(emit_nexus_as_ca(scope, ivl_lpm_data(lpm, 1), 0, 0));
			fprintf(g_out, ")");
			return smt_bi;
        }
		case IVL_LPM_CMP_NE:
        {
            SMTBinary* smt_bi = new SMTBinary();
            smt_bi->set_opcode('n');
			fprintf(g_out, "(");
			smt_bi->add(emit_nexus_as_ca(scope, ivl_lpm_data(lpm, 0), 0, 0));
			fprintf(g_out, " != ");
			smt_bi->add(emit_nexus_as_ca(scope, ivl_lpm_data(lpm, 1), 0, 0));
			fprintf(g_out, ")");
			return smt_bi;
        }
		case IVL_LPM_CMP_NEE:
            error("check IVL_LPM_CMP_NEE");
			fprintf(g_out, "(");
			emit_nexus_as_ca(scope, ivl_lpm_data(lpm, 0), 0, 0);
			fprintf(g_out, " !== ");
			emit_nexus_as_ca(scope, ivl_lpm_data(lpm, 1), 0, 0);
			fprintf(g_out, ")");
			break;
		case IVL_LPM_CONCAT:
		case IVL_LPM_CONCATZ:
		{
			SMTConcat* smt_concat = emit_lpm_concat(scope, lpm);
			return smt_concat;
		}
		case IVL_LPM_DIVIDE:
            error("IVL_LPM_DIVIDE used");
			fprintf(g_out, "(");
			emit_nexus_as_ca(scope, ivl_lpm_data(lpm, 0), 0, 0);
			fprintf(g_out, " / ");
			emit_nexus_as_ca(scope, ivl_lpm_data(lpm, 1), 0, 0);
			fprintf(g_out, ")");
			break;
		case IVL_LPM_MOD:
			// HERE: Need to check if this LPM is IVL_VT_REAL.
			//if (0) {
				fprintf(stderr, "%s:%u: vlog95 error: Real modulus operator "
						"is not supported.\n",
						ivl_lpm_file(lpm), ivl_lpm_lineno(lpm));
				g_errors += 1;
			//}
			//fprintf(g_out, "(");
			//emit_nexus_as_ca(scope, ivl_lpm_data(lpm, 0), 0, 0);
			//fprintf(g_out, " %% ");
			//emit_nexus_as_ca(scope, ivl_lpm_data(lpm, 1), 0, 0);
			//fprintf(g_out, ")");
			break;
		case IVL_LPM_MULT:
        {
            SMTBinary* smt_bi = new SMTBinary();
            smt_bi->set_opcode('*');
			fprintf(g_out, "(");
			smt_bi->add(emit_nexus_as_ca(scope, ivl_lpm_data(lpm, 0), 0, 0));
			fprintf(g_out, " * ");
			smt_bi->add(emit_nexus_as_ca(scope, ivl_lpm_data(lpm, 1), 0, 0));
			fprintf(g_out, ")");
			return smt_bi;
        }
		case IVL_LPM_MUX:
        {
            SMTTernary* smt_ter = new SMTTernary();
			fprintf(g_out, "(");
			smt_ter->add(emit_nexus_as_ca(scope, ivl_lpm_select(lpm), 0, 0));
			fprintf(g_out, " ? ");
			smt_ter->add(emit_nexus_as_ca(scope, ivl_lpm_data(lpm, 1), 0, 0));
			fprintf(g_out, " : ");
			smt_ter->add(emit_nexus_as_ca(scope, ivl_lpm_data(lpm, 0), 0, 0));
			fprintf(g_out, ")");
			return smt_ter;
        }
		case IVL_LPM_PART_PV:
			return emit_nexus_as_ca(scope, ivl_lpm_data(lpm, 0), 1, 0);
		case IVL_LPM_PART_VP:
			return emit_lpm_part_select(scope, lpm, sign_extend);
		case IVL_LPM_POW:
        {
            SMTBinary* smt_bi = new SMTBinary();
            smt_bi->set_opcode('p');
			fprintf(g_out, "(");
			smt_bi->add(emit_nexus_as_ca(scope, ivl_lpm_data(lpm, 0), 0, 0));
			fprintf(g_out, " ** ");
			smt_bi->add(emit_nexus_as_ca(scope, ivl_lpm_data(lpm, 1), 0, 0));
			fprintf(g_out, ")");
			return smt_bi;
        }
		case IVL_LPM_RE_AND:
        {
            SMTUnary* smt_un = new SMTUnary();
            smt_un->set_opcode('&');
			fprintf(g_out, "(&");
			smt_un->add(emit_nexus_as_ca(scope, ivl_lpm_data(lpm, 0), 0, 0));
			fprintf(g_out, ")");
			return smt_un;
        }
		case IVL_LPM_RE_NAND:
        {
            SMTUnary* smt_un = new SMTUnary();
            smt_un->set_opcode('A');
			fprintf(g_out, "(~&");
			smt_un->add(emit_nexus_as_ca(scope, ivl_lpm_data(lpm, 0), 0, 0));
			fprintf(g_out, ")");
			return smt_un;
        }
		case IVL_LPM_RE_NOR:
        {
            SMTUnary* smt_un = new SMTUnary();
            smt_un->set_opcode('N');
			fprintf(g_out, "(~|");
			smt_un->add(emit_nexus_as_ca(scope, ivl_lpm_data(lpm, 0), 0, 0));
			fprintf(g_out, ")");
			return smt_un;
        }
		case IVL_LPM_RE_OR:
        {
            SMTUnary* smt_un = new SMTUnary();
            smt_un->set_opcode('|');
			fprintf(g_out, "(|");
			smt_un->add(emit_nexus_as_ca(scope, ivl_lpm_data(lpm, 0), 0, 0));
			fprintf(g_out, ")");
			return smt_un;
        }
		case IVL_LPM_RE_XOR:
        {
            SMTUnary* smt_un = new SMTUnary();
            smt_un->set_opcode('^');
			fprintf(g_out, "(^");
			smt_un->add(emit_nexus_as_ca(scope, ivl_lpm_data(lpm, 0), 0, 0));
			fprintf(g_out, ")");
			return smt_un;
        }
		case IVL_LPM_RE_XNOR:
        {
            SMTUnary* smt_un = new SMTUnary();
            smt_un->set_opcode('X');
			fprintf(g_out, "(~^");
			smt_un->add(emit_nexus_as_ca(scope, ivl_lpm_data(lpm, 0), 0, 0));
			fprintf(g_out, ")");
			return smt_un;
        }
		case IVL_LPM_REPEAT:
        {
            SMTConcat* smt_con = new SMTConcat();
            smt_con->repeat = ivl_lpm_size(lpm);
			fprintf(g_out, "{%u{", ivl_lpm_size(lpm));
			smt_con->add(emit_nexus_as_ca(scope, ivl_lpm_data(lpm, 0), 0, 0));
			fprintf(g_out, "}}");
			return smt_con;
        }
		//case IVL_LPM_SFUNC:
		//	fprintf(g_out, "%s", ivl_lpm_string(lpm));
		//	emit_lpm_func(scope, lpm);
		//	break;
		case IVL_LPM_SHIFTL:
        {
            SMTBinary* smt_bi = new SMTBinary();
            smt_bi->set_opcode('l');
			fprintf(g_out, "(");
			smt_bi->add(emit_nexus_as_ca(scope, ivl_lpm_data(lpm, 0), 0, 0));
			fprintf(g_out, " << ");
			smt_bi->add(emit_nexus_as_ca(scope, ivl_lpm_data(lpm, 1), 0, 0));
			fprintf(g_out, ")");
			return smt_bi;
        }
		case IVL_LPM_SHIFTR:
        {
            SMTBinary* smt_bi = new SMTBinary();
			if(ivl_lpm_signed(lpm)){
				smt_bi->set_opcode('R');
			}
            else{
				smt_bi->set_opcode('r');
			}
			fprintf(g_out, "(");
			smt_bi->add(emit_nexus_as_ca(scope, ivl_lpm_data(lpm, 0), 0, 0));
			fprintf(g_out, " ");
			if (ivl_lpm_signed(lpm)) {
				fprintf(g_out, ">");
			}
			fprintf(g_out, ">> ");
			smt_bi->add(emit_nexus_as_ca(scope, ivl_lpm_data(lpm, 1), 0, 0));
			fprintf(g_out, ")");
			return smt_bi;
        }
		//case IVL_LPM_SIGN_EXT:
		//	assert(!sign_extend);
		//	emit_nexus_as_ca(scope, ivl_lpm_data(lpm, 0), 1, 1);
		//	break;
		case IVL_LPM_SUB:
        {
            SMTBinary* smt_bi = new SMTBinary();
            smt_bi->set_opcode('-');
			fprintf(g_out, "(");
			smt_bi->add(emit_nexus_as_ca(scope, ivl_lpm_data(lpm, 0), 0, 0));
			fprintf(g_out, " - ");
			smt_bi->add(emit_nexus_as_ca(scope, ivl_lpm_data(lpm, 1), 0, 0));
			fprintf(g_out, ")");
			return smt_bi;
        }
		default:
			fprintf(g_out, "<unknown>");
			fprintf(stderr, "%s:%u: vlog95 error: Unknown LPM type (%d).\n",
					ivl_lpm_file(lpm), ivl_lpm_lineno(lpm),
					(int) ivl_lpm_type(lpm));
			g_errors += 1;
	}

	return new SMTUnspecified();
}

static ivl_signal_t get_output_from_nexus(ivl_scope_t scope, ivl_nexus_t nex,
		int64_t*array_idx) {
	ivl_signal_t use_sig = 0;
	unsigned is_array = 0;
	unsigned idx, count;
	count = ivl_nexus_ptrs(nex);
	for (idx = 0; idx < count; idx += 1) {
		ivl_nexus_ptr_t nex_ptr = ivl_nexus_ptr(nex, idx);
		ivl_signal_t sig = ivl_nexus_ptr_sig(nex_ptr);
		if (!sig) continue;
		if (ivl_signal_local(sig)) {
			/* If the local signal is another receiver skip it. */
			if ((ivl_nexus_ptr_drive1(nex_ptr) == IVL_DR_HiZ) &&
					(ivl_nexus_ptr_drive0(nex_ptr) == IVL_DR_HiZ)) {
				continue;
			}
			assert(0);
		}
		/* The signal must be in the correct scope. */
		if (scope != ivl_signal_scope(sig)) continue;
		/* Since we are looking for the output signal it is a receiver. */
		if ((ivl_nexus_ptr_drive1(nex_ptr) != IVL_DR_HiZ) &&
				(ivl_nexus_ptr_drive0(nex_ptr) != IVL_DR_HiZ)) {
			continue;
		}
		if (use_sig) {
			// HERE: Which one should we use? For now it's the first one found.
			//       I believe this needs to be solved (see the inout.v test).
			fprintf(stderr, "%s:%u: vlog95 warning: Duplicate name (%s",
					ivl_signal_file(sig), ivl_signal_lineno(sig),
					ivl_signal_basename(sig));
			if (ivl_signal_dimensions(sig) > 0) {
				int64_t tmp_idx = ivl_nexus_ptr_pin(nex_ptr);
				tmp_idx += ivl_signal_array_base(sig);
				fprintf(stderr, "[%" PRId64"]", tmp_idx);
			}
			fprintf(stderr, ") found for nexus (%s",
					ivl_signal_basename(use_sig));
			if (is_array) fprintf(stderr, "[%" PRId64"]", *array_idx);
			fprintf(stderr, ")\n");
		} else {
			/* We have a signal that can be used to find the name. */
			use_sig = sig;
			if (ivl_signal_dimensions(sig) > 0) {
				is_array = 1;
				*array_idx = ivl_nexus_ptr_pin(nex_ptr);
				*array_idx += ivl_signal_array_base(sig);
			}
		}
	}

	return use_sig;
}

static SMTSignal* emit_lpm_part_pv(ivl_scope_t scope, ivl_lpm_t lpm) {
	unsigned width = ivl_lpm_width(lpm);
	int64_t array_word = 0;
	int base = ivl_lpm_base(lpm);
	int msb, lsb;
	ivl_signal_t sig = get_output_from_nexus(scope, ivl_lpm_q(lpm),
			&array_word);
	assert(sig);
	assert(ivl_lpm_data(lpm, 1) == 0);
	emit_id(ivl_signal_basename(sig));
    SMTSignal* smt_sig = new SMTSignal(sig);
	if (ivl_signal_dimensions(sig)) {
        error("array used");
		fprintf(g_out, "[%" PRId64"]", array_word);
	}
	get_sig_msb_lsb(sig, &msb, &lsb);
	fprintf(g_out, "[");
	if (width == 1) {
		if (msb >= lsb) base += lsb;
		else base = lsb - base;
		fprintf(g_out, "%d", base);
        smt_sig->msb = base;
        smt_sig->lsb = base;
	} else {
		if (msb >= lsb) {
			base += lsb;
			fprintf(g_out, "%d:%d", base + (int) width - 1, base);
            smt_sig->msb = base + width - 1;
            smt_sig->lsb = base;
		} else {
			base = lsb - base;
			fprintf(g_out, "%d:%d", base - (int) width + 1, base);
            smt_sig->msb = base - width + 1;
            smt_sig->lsb = base;
		}
	}
	fprintf(g_out, "]");
    return smt_sig;
}

static unsigned output_is_module_instantiation_input(ivl_scope_t scope,
		ivl_nexus_t nex) {
	unsigned idx, count = ivl_nexus_ptrs(nex);
	unsigned rtn = 0;
	for (idx = 0; idx < count; idx += 1) {
		ivl_nexus_ptr_t nex_ptr = ivl_nexus_ptr(nex, idx);
		/* Skip drivers. */
		if ((ivl_nexus_ptr_drive1(nex_ptr) != IVL_DR_HiZ) ||
				(ivl_nexus_ptr_drive0(nex_ptr) != IVL_DR_HiZ)) continue;
		ivl_signal_t t_sig = ivl_nexus_ptr_sig(nex_ptr);
		/* If the nexus is driving other things or signals that are
		 * not a module instantiation input then return false. */
		// HERE: debug this to see if the output can drive other things local to the
		//       module that is being called.
		//	    if (! t_sig) return 0;
		if (!t_sig) continue;
		if (ivl_signal_port(t_sig) != IVL_SIP_INPUT) return 0;
		if (ivl_scope_parent(ivl_signal_scope(t_sig)) != scope) return 0;
		if (rtn) return 0;
		rtn = 1;
	}
	return rtn;
}

void emit_lpm(ivl_scope_t scope, ivl_lpm_t lpm) {
	ivl_nexus_t output = get_lpm_output(scope, lpm);
	ivl_lpm_type_t type = ivl_lpm_type(lpm);
	/* If the output is local then someone else will output this lpm. */
	if (!output) return;
	/* If the LPM is a D-FF then we need to emit it as a UDP. */
	if (type == IVL_LPM_FF) {
        error("LPM is a D-FF");
		return;
	}
	// HERE: Look for a select passed to a pull device (pr2019553).
	/* Skip assignments to a module instantiation input. */
	if (output_is_module_instantiation_input(scope, output)) return;
    SMTProcess::curr_proc = new SMTProcess();
	
	fprintf(g_out, "%*calways @(*) begin\n", g_ind, ' ');
	g_ind += g_ind_incr;
	fprintf(g_out, "%*c", g_ind, ' ');
	SMTExpr* lval;
	if (type == IVL_LPM_PART_PV) {
		lval = emit_lpm_part_pv(scope, lpm);
	}
	else {
		lval = emit_name_of_nexus(scope, output, 0);
	}

    fprintf(g_out, " = ");
	SMTExpr* rval = emit_lpm_as_ca(scope, lpm, 0);
	fprintf(g_out, ";");
	SMTAssign* smt_assign = new SMTBlockingAssign(lval, rval);
	smt_assign->add_to_cont();
	g_ind -= g_ind_incr;
	fprintf(g_out, "%*cend\n\n", g_ind, ' ');
	
    SMTProcess::curr_proc = NULL;
}

/*
 * A BUFZ is a simple variable assignment possibly with strength and/or delay.
 */
static void emit_bufz(ivl_scope_t scope, ivl_net_logic_t nlogic) {
	assert(ivl_logic_pins(nlogic) == 2);
    
	SMTProcess::curr_proc = new SMTProcess();
	fprintf(g_out, "always @(*) begin\n");
	g_ind += g_ind_incr;
	fprintf(g_out, "%*c", g_ind, ' ');
	SMTExpr* lval = emit_name_of_nexus(scope, ivl_logic_pin(nlogic, 0), 0);
	fprintf(g_out, " = ");
	SMTExpr* rval = emit_nexus_as_ca(scope, ivl_logic_pin(nlogic, 1), 0, 0);
	fprintf(g_out, ";");
	SMTAssign* smt_assign = new SMTBlockingAssign(lval, rval);
	smt_assign->add_to_cont();
	g_ind -= g_ind_incr;
	fprintf(g_out, "%*cend\n\n", g_ind, ' ');
    
    SMTProcess::curr_proc = NULL;
}

static void emit_name_of_logic_nexus(ivl_scope_t scope, ivl_net_logic_t nlogic,
		ivl_nexus_t nex) {
	if (nex) {
		emit_name_of_nexus(scope, nex, 0);
	} else {
		if (ivl_logic_type(nlogic) != IVL_LO_UDP) {
			fprintf(stderr, "%s:%u: vlog95 warning: Missing logic pin "
					"for (%d) named: %s.\n",
					ivl_logic_file(nlogic),
					ivl_logic_lineno(nlogic),
					ivl_logic_type(nlogic),
					ivl_logic_basename(nlogic));
		}
		fprintf(g_out, "1'bz");
	}
}

void emit_logic(ivl_scope_t scope, ivl_net_logic_t nlogic) {
	// HERE: We need to investigate if this is really the base of a CA. A real
	//       gate only allows a signal or a signal bit select for the output(s)
	//       and a scalar expression for the input. We also need to modify the
	//       compiler to support logical 'and' and logical 'or' since they
	//       short circuit. Verify input count.
	unsigned idx, count, strength_type = 2;
	unsigned outputs = 1;
	const char *name;
	/* Skip gates that have a local nexus as the output since they are
	 * part of a continuous assignment. */
	if (is_local_nexus(scope, ivl_logic_pin(nlogic, 0))) return;
	fprintf(g_out, "%*c", g_ind, ' ');
	/* Check to see if this logical should really be emitted as/was
	 * generated from a continuous assignment. */
	if (ivl_logic_is_cassign(nlogic)) {
		
		unsigned pin_count = 2;
		if (ivl_logic_type(nlogic) != IVL_LO_NOT) pin_count += 1;
		assert(ivl_logic_pins(nlogic) == pin_count);
		
        SMTProcess::curr_proc = new SMTProcess();
		
		fprintf(g_out, "always @(*) begin\n");
		g_ind += g_ind_incr;
		fprintf(g_out, "%*c", g_ind, ' ');
		
		SMTExpr* lval = emit_name_of_nexus(scope, ivl_logic_pin(nlogic, 0), 0);
		fprintf(g_out, " = ");
		SMTExpr* rval = emit_logic_as_ca(scope, nlogic);
		fprintf(g_out, ";");
		SMTAssign* smt_assign = new SMTBlockingAssign(lval, rval);
		smt_assign->add_to_cont();
		g_ind -= g_ind_incr;
		fprintf(g_out, "%*cend\n\n", g_ind, ' ');
		
        SMTProcess::curr_proc = NULL;   
		return;
	}
	//error("Check gate generation");
	if(ivl_logic_type(nlogic) == IVL_LO_BUFZ){
		emit_bufz(scope, nlogic);
		return;
	}
	else{
		error("Check gate generation");
	}
	switch (ivl_logic_type(nlogic)) {
		case IVL_LO_AND:
			fprintf(g_out, "and");
			break;
		case IVL_LO_BUF:
			fprintf(g_out, "buf");
			outputs = 0;
			break;
		case IVL_LO_BUFIF0:
			fprintf(g_out, "bufif0");
			break;
		case IVL_LO_BUFIF1:
			fprintf(g_out, "bufif1");
			break;
			//	case IVL_LO_BUFT:
		case IVL_LO_BUFZ:
			emit_bufz(scope, nlogic);
			return;
		case IVL_LO_CMOS:
			fprintf(g_out, "cmos");
			break;
		case IVL_LO_NAND:
			fprintf(g_out, "nand");
			break;
		case IVL_LO_NMOS:
			fprintf(g_out, "nmos");
			break;
		case IVL_LO_NOR:
			fprintf(g_out, "nor");
			break;
		case IVL_LO_NOT:
			fprintf(g_out, "not");
			outputs = 0;
			break;
		case IVL_LO_NOTIF0:
			fprintf(g_out, "notif0");
			break;
		case IVL_LO_NOTIF1:
			fprintf(g_out, "notif1");
			break;
		case IVL_LO_OR:
			fprintf(g_out, "or");
			break;
		case IVL_LO_PMOS:
			fprintf(g_out, "pmos");
			break;
		case IVL_LO_PULLDOWN:
			fprintf(g_out, "pulldown");
			outputs = 0;
			strength_type = 0;
			break;
		case IVL_LO_PULLUP:
			fprintf(g_out, "pullup");
			outputs = 0;
			strength_type = 1;
			break;
		case IVL_LO_RCMOS:
			fprintf(g_out, "rcmos");
			break;
		case IVL_LO_RNMOS:
			fprintf(g_out, "rnmos");
			break;
		case IVL_LO_RPMOS:
			fprintf(g_out, "rpmos");
			break;
		case IVL_LO_XNOR:
			fprintf(g_out, "xnor");
			break;
		case IVL_LO_XOR:
			fprintf(g_out, "xor");
			break;
		default:
			fprintf(g_out, "<unknown>(");
			fprintf(stderr, "%s:%u: vlog95 error: Unsupported logic type "
					"(%d) named: %s.\n", ivl_logic_file(nlogic),
					ivl_logic_lineno(nlogic), ivl_logic_type(nlogic),
					ivl_logic_basename(nlogic));
			g_errors += 1;
			break;
	}
	// HERE: The name has the location information encoded in it. We need to
	//       remove this and rebuild the instance array. For now we just strip
	//       this encoding and create an zero based range. Need to skip the
	//       local names _s<digits>.
	//       This can also be an escaped id.
	name = ivl_logic_basename(nlogic);
	if (name && *name) {
		char *fixed_name = strdup(name);
		unsigned lp = strlen(name) - 1;
		unsigned width = ivl_logic_width(nlogic);
		if (fixed_name[lp] == '>') {
			while (fixed_name[lp] != '<') {
				assert(lp > 0);
				lp -= 1;
			}
			fixed_name[lp] = 0;
		}
		fprintf(g_out, " ");
		emit_id(fixed_name);
		free(fixed_name);
		if (width > 1) {
			fprintf(g_out, " [%u:0]", width - 1);
		}
	}
	fprintf(g_out, " (");
	count = ivl_logic_pins(nlogic);
	count -= 1;
	if (outputs == 0) outputs = count;
	for (idx = 0; idx < outputs; idx += 1) {
		emit_name_of_logic_nexus(scope, nlogic, ivl_logic_pin(nlogic, idx));
		fprintf(g_out, ", ");
	}
	for (/* None */; idx < count; idx += 1) {
		emit_nexus_as_ca(scope, ivl_logic_pin(nlogic, idx), 0, 0);
		fprintf(g_out, ", ");
	}
	if (strength_type == 2) {
		emit_nexus_as_ca(scope, ivl_logic_pin(nlogic, idx), 0, 0);
	} else {
		/* A pull gate only has a single output connection. */
		assert(count == 0);
		emit_name_of_logic_nexus(scope, nlogic, ivl_logic_pin(nlogic, idx));
	}
	fprintf(g_out, ");\n");
}

void emit_signal_net_const_as_ca(ivl_scope_t scope, ivl_signal_t sig) {
	ivl_nexus_t nex = ivl_signal_nex(sig, 0);
	unsigned idx, count = ivl_nexus_ptrs(nex);
	unsigned long emitted = (uintptr_t) ivl_nexus_get_private(nex);
	for (idx = 0; idx < count; idx += 1) {
		ivl_nexus_ptr_t nex_ptr = ivl_nexus_ptr(nex, idx);
		ivl_net_const_t net_const = ivl_nexus_ptr_con(nex_ptr);
		if (!net_const) continue;
		if (scope != ivl_const_scope(net_const)) continue;
		/* Found the constant so emit it if it has not been emitted. */
		if (emitted) {
			--emitted;
			continue;
		}
		
        SMTProcess::curr_proc = new SMTProcess();
		fprintf(g_out, "%*cinitial begin\n", g_ind, ' ');
		g_ind += g_ind_incr;
		fprintf(g_out, "%*c", g_ind, ' ');
		emit_id(ivl_signal_basename(sig));
		SMTExpr* lval = new SMTSignal(sig);
		fprintf(g_out, " = ");
		SMTExpr* rval = emit_const_nexus(scope, net_const);
		fprintf(g_out, ";");
		new SMTBlockingAssign(lval, rval);
		g_ind -= g_ind_incr;
		fprintf(g_out, "%*cend\n\n", g_ind, ' ');
        SMTProcess::curr_proc = NULL;
        
		/* Increment the emitted constant count by one. */
		ivl_nexus_set_private(nex,
				(void *) ((uintptr_t) ivl_nexus_get_private(nex) + 1U));
		return;
	}
	/* We must find the constant in the nexus. */
	assert(0);

}

/*
 * Routine to dump the nexus information.
 */
void dump_nexus_information(ivl_scope_t scope, ivl_nexus_t nex) {
	unsigned idx, count;
	if ((scope == 0) || (nex == 0)) return;
	count = ivl_nexus_ptrs(nex);
	fprintf(stderr, "Dumping nexus %p from scope: %s\n", nex,
			ivl_scope_name(scope));
	for (idx = 0; idx < count; idx += 1) {
		ivl_nexus_ptr_t nex_ptr = ivl_nexus_ptr(nex, idx);
		ivl_lpm_t lpm = ivl_nexus_ptr_lpm(nex_ptr);
		ivl_net_const_t net_const = ivl_nexus_ptr_con(nex_ptr);
		ivl_net_logic_t nlogic = ivl_nexus_ptr_log(nex_ptr);
		ivl_signal_t sig = ivl_nexus_ptr_sig(nex_ptr);
		if (lpm) {
			ivl_scope_t lpm_scope = ivl_lpm_scope(lpm);
			assert(!net_const);
			assert(!nlogic);
			assert(!sig);
			fprintf(stderr, "LPM: ");
			fprintf(stderr, "{%s:%u} ", ivl_lpm_file(lpm),
					ivl_lpm_lineno(lpm));
			if (scope != lpm_scope) fprintf(stderr, "(%s) ",
					ivl_scope_name(lpm_scope));
			switch (ivl_lpm_type(lpm)) {
				case IVL_LPM_ABS: fprintf(stderr, "abs");
					break;
				case IVL_LPM_ADD: fprintf(stderr, "add");
					break;
				case IVL_LPM_ARRAY: fprintf(stderr, "array");
					break;
				case IVL_LPM_CAST_INT: fprintf(stderr, "<int>");
					break;
				case IVL_LPM_CAST_INT2: fprintf(stderr, "<int2>");
					break;
				case IVL_LPM_CAST_REAL: fprintf(stderr, "<real>");
					break;
				case IVL_LPM_CONCAT: fprintf(stderr, "concat");
					break;
				case IVL_LPM_CONCATZ: fprintf(stderr, "concatz");
					break;
				case IVL_LPM_CMP_EEQ: fprintf(stderr, "eeq");
					break;
				case IVL_LPM_CMP_EQ: fprintf(stderr, "eq");
					break;
				case IVL_LPM_CMP_GE: fprintf(stderr, "ge");
					break;
				case IVL_LPM_CMP_GT: fprintf(stderr, "gt");
					break;
				case IVL_LPM_CMP_NE: fprintf(stderr, "ne");
					break;
				case IVL_LPM_CMP_NEE: fprintf(stderr, "nee");
					break;
				case IVL_LPM_DIVIDE: fprintf(stderr, "divide");
					break;
				case IVL_LPM_FF: fprintf(stderr, "dff");
					break;
				case IVL_LPM_MOD: fprintf(stderr, "mod");
					break;
				case IVL_LPM_MULT: fprintf(stderr, "mult");
					break;
				case IVL_LPM_MUX: fprintf(stderr, "mux");
					break;
				case IVL_LPM_PART_VP: fprintf(stderr, "part-VP");
					fprintf(stderr, "(%u+%u)", ivl_lpm_base(lpm),
							ivl_lpm_width(lpm));
					break;
				case IVL_LPM_PART_PV: fprintf(stderr, "part-PV");
					break;
				case IVL_LPM_POW: fprintf(stderr, "pow");
					break;
				case IVL_LPM_RE_AND: fprintf(stderr, "R-AND");
					break;
				case IVL_LPM_RE_NAND: fprintf(stderr, "R-NAND");
					break;
				case IVL_LPM_RE_OR: fprintf(stderr, "R-OR");
					break;
				case IVL_LPM_RE_NOR: fprintf(stderr, "R-NOR");
					break;
				case IVL_LPM_RE_XNOR: fprintf(stderr, "R-XNOR");
					break;
				case IVL_LPM_RE_XOR: fprintf(stderr, "R-XOR");
					break;
				case IVL_LPM_REPEAT: fprintf(stderr, "repeat");
					break;
				case IVL_LPM_SFUNC: fprintf(stderr, "S-func");
					break;
				case IVL_LPM_SHIFTL: fprintf(stderr, "shiftl");
					break;
				case IVL_LPM_SHIFTR: fprintf(stderr, "shiftr");
					break;
				case IVL_LPM_SIGN_EXT: fprintf(stderr, "sign");
					break;
				case IVL_LPM_SUB: fprintf(stderr, "sub");
					break;
				case IVL_LPM_UFUNC: fprintf(stderr, "U-func");
					break;
				default: fprintf(stderr, "<%d>", ivl_lpm_type(lpm));
			}
			if (ivl_lpm_signed(lpm)) fprintf(stderr, " <signed>");
		} else if (net_const) {
			ivl_scope_t const_scope = ivl_const_scope(net_const);
			assert(!nlogic);
			assert(!sig);
			fprintf(stderr, "Const:");
			if (scope != const_scope) {
				fprintf(stderr, " (%s)", ivl_scope_name(const_scope));
			}
			if (ivl_const_signed(net_const)) fprintf(stderr, " <signed>");
		} else if (nlogic) {
			ivl_scope_t logic_scope = ivl_logic_scope(nlogic);
			ivl_logic_t logic_type = ivl_logic_type(nlogic);
			assert(!sig);
			fprintf(stderr, "Logic: ");
			fprintf(stderr, "{%s:%u} ", ivl_logic_file(nlogic),
					ivl_logic_lineno(nlogic));
			if (scope != logic_scope) {
				fprintf(stderr, "(%s) ", ivl_scope_name(logic_scope));
			}
			switch (logic_type) {
				case IVL_LO_AND: fprintf(stderr, "and");
					break;
				case IVL_LO_BUF: fprintf(stderr, "buf");
					break;
				case IVL_LO_BUFIF0: fprintf(stderr, "bufif0");
					break;
				case IVL_LO_BUFIF1: fprintf(stderr, "bufif1");
					break;
				case IVL_LO_BUFT: fprintf(stderr, "buft");
					break;
				case IVL_LO_BUFZ: fprintf(stderr, "bufz");
					break;
				case IVL_LO_CMOS: fprintf(stderr, "cmos");
					break;
				case IVL_LO_NAND: fprintf(stderr, "nand");
					break;
				case IVL_LO_NMOS: fprintf(stderr, "nmos");
					break;
				case IVL_LO_NOR: fprintf(stderr, "nor");
					break;
				case IVL_LO_NOT: fprintf(stderr, "not");
					break;
				case IVL_LO_NOTIF0: fprintf(stderr, "notif0");
					break;
				case IVL_LO_NOTIF1: fprintf(stderr, "notif1");
					break;
				case IVL_LO_OR: fprintf(stderr, "or");
					break;
				case IVL_LO_PMOS: fprintf(stderr, "pmos");
					break;
				case IVL_LO_PULLDOWN: fprintf(stderr, "pulldown");
					break;
				case IVL_LO_PULLUP: fprintf(stderr, "pullup");
					break;
				case IVL_LO_RCMOS: fprintf(stderr, "rcmos");
					break;
				case IVL_LO_RNMOS: fprintf(stderr, "rnmos");
					break;
				case IVL_LO_RPMOS: fprintf(stderr, "rpmos");
					break;
				case IVL_LO_UDP:
				{
					ivl_udp_t udp = ivl_logic_udp(nlogic);
					assert(udp);
					fprintf(stderr, "UDP %s", ivl_udp_name(udp));
					break;
				}
				case IVL_LO_XNOR: fprintf(stderr, "xnor");
					break;
				case IVL_LO_XOR: fprintf(stderr, "xor");
					break;
				default: fprintf(stderr, "<%d>", ivl_logic_type(nlogic));
			}
			/* The BUF and NOT gates can have multiple outputs and a
			 * single input. . */
			if ((logic_type == IVL_LO_BUF) ||
					(logic_type == IVL_LO_NOT)) {
				unsigned outputs = ivl_logic_pins(nlogic) - 1;
				if (outputs == 1) fprintf(stderr, "(1 output)");
				else fprintf(stderr, "(%u outputs)", outputs);
				/* The rest of the gates have a single output and can
				 * have zero or more inputs. */
			} else {
				unsigned inputs = ivl_logic_pins(nlogic) - 1;
				if (inputs == 1) fprintf(stderr, "(1 input)");
				else fprintf(stderr, "(%u inputs)", inputs);
			}
		} else if (sig) {
			ivl_scope_t sig_scope = ivl_signal_scope(sig);
			fprintf(stderr, "Signal: \"");
			if (scope != sig_scope) fprintf(stderr, "%s.",
					ivl_scope_name(sig_scope));
			fprintf(stderr, "%s", ivl_signal_basename(sig));
			if (ivl_signal_dimensions(sig) > 0) {
				fprintf(stderr, "[]");
			}
			fprintf(stderr, "\"");
			// HERE: Do we need to add support for an array word or is that an LPM.
			if (ivl_signal_local(sig)) fprintf(stderr, " {local}");
			else fprintf(stderr, " {%s:%u}", ivl_signal_file(sig),
					ivl_signal_lineno(sig));
			switch (ivl_signal_port(sig)) {
				case IVL_SIP_INPUT: fprintf(stderr, " input");
					break;
				case IVL_SIP_OUTPUT: fprintf(stderr, " output");
					break;
				case IVL_SIP_INOUT: fprintf(stderr, " inout");
					break;
				case IVL_SIP_NONE: break;
			}
			switch (ivl_signal_type(sig)) {
				case IVL_SIT_NONE: fprintf(stderr, " <no type>");
					break;
				case IVL_SIT_REG: fprintf(stderr, " reg");
					break;
				case IVL_SIT_TRI: fprintf(stderr, " tri");
					break;
				case IVL_SIT_TRI0: fprintf(stderr, " tri0");
					break;
				case IVL_SIT_TRI1: fprintf(stderr, " tri1");
					break;
				case IVL_SIT_TRIAND: fprintf(stderr, " triand");
					break;
				case IVL_SIT_TRIOR: fprintf(stderr, " trior");
					break;
				case IVL_SIT_UWIRE: fprintf(stderr, " uwire");
					break;
			}
			switch (ivl_signal_data_type(sig)) {
				case IVL_VT_VOID: fprintf(stderr, " <void>");
					break;
				case IVL_VT_NO_TYPE: fprintf(stderr, " <no type>");
					break;
				case IVL_VT_REAL: fprintf(stderr, " real");
					break;
				case IVL_VT_BOOL: fprintf(stderr, " bool");
					break;
				case IVL_VT_LOGIC: fprintf(stderr, " logic");
					break;
				case IVL_VT_STRING: fprintf(stderr, " string");
					break;
				case IVL_VT_DARRAY: fprintf(stderr, " dynamic array");
				case IVL_VT_CLASS: fprintf(stderr, " class");
				case IVL_VT_QUEUE: fprintf(stderr, " queue");
					break;
			}
			if (ivl_signal_signed(sig)) fprintf(stderr, " <signed>");
		} else fprintf(stderr, "Error: No/missing information!");
		fprintf(stderr, " (%u)\n", ivl_nexus_ptr_pin(nex_ptr));
	}
}
