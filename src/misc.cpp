#include <ctype.h>
#include <cstdlib>
#include <cstring>
#include <cstdarg>
#include <sys/stat.h>
//#include <sys/sendfile.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include "concolic.h"
#include "smt_lib.h"

using namespace std;

static int64_t get_in_range_int64_from_number(ivl_expr_t expr, int *rtype,
		const char *msg) {
	int64_t value = get_int64_from_number(expr, rtype);
	if (*rtype > 0) {
		fprintf(g_out, "<invalid>");
		fprintf(stderr, "%s:%u: vlog95 error: Scaled %s is greater than "
				"64 bits (%d) and cannot be safely represented.\n",
				ivl_expr_file(expr), ivl_expr_lineno(expr),
				msg, *rtype);
		g_errors += 1;
	}
	return value;
}

void emit_scaled_range(ivl_scope_t scope, ivl_expr_t expr, unsigned width,
		int msb, int lsb, SMTSignal* smt_sig) {
	int rtype;
	int64_t value = get_in_range_int64_from_number(expr, &rtype,
			"range value");
	(void) scope; /* Parameter is not used. */
	if (rtype < 0) fprintf(g_out, "[1'bx:1'bx]");
	if (rtype) return;

	if (msb >= lsb) {
		value += lsb;
		fprintf(g_out, "[%" PRId64":%" PRId64"]",
				value + (int64_t) (width - 1), value);
		smt_sig->lsb = value;
		smt_sig->msb = value + (int64_t) (width - 1);
	} else {
		error("emit_scaled_range: msb < lsb");
		value = (int64_t) lsb - value;
		fprintf(g_out, "[%" PRId64":%" PRId64"]",
				value - (int64_t) (width - 1), value);
		smt_sig->lsb = value;
		smt_sig->msb = value - (int64_t) (width - 1);
	}
}

void emit_scaled_expr(ivl_scope_t scope, ivl_expr_t expr, int msb, int lsb, SMTSignal* tmp_sig) {
	int64_t value = 0;
	if (msb >= lsb) {
		if (ivl_expr_type(expr) == IVL_EX_NUMBER) {
			int rtype;
			value = get_in_range_int64_from_number(expr, &rtype,
					"value");
			if (rtype < 0) fprintf(g_out, "1'bx");
			if (rtype) return;
			value += lsb;
			fprintf(g_out, "%" PRId64, value);
		} /*else if (lsb == 0) {
			// If the LSB is zero then there is no scale.
			emit_expr(scope, expr, 0, 0, 0, 1);
		} else {
			if (is_scaled_expr(expr, msb, lsb)) {
				emit_expr(scope, ivl_expr_oper1(expr), 0, 0, 0, 1);
			}
		}*/
		//alif: made non number expressions invalid
		else{
			if(ivl_expr_type(expr) == IVL_EX_SIGNAL){
				tmp_sig->is_index_term = true;
				tmp_sig->_index = emit_expr(scope, expr);
			}
			info("Invalid expression for signal select");
			info("[EXAMPLE] nl[data_out]");
		}
	} else {
		if (ivl_expr_type(expr) == IVL_EX_NUMBER) {
			int rtype;
			value = get_in_range_int64_from_number(expr, &rtype,
					"value");
			if (rtype < 0) fprintf(g_out, "1'bx");
			if (rtype) return;
			value = (int64_t) lsb - value;
			fprintf(g_out, "%" PRId64, value);
		} /*else {
			if (is_scaled_expr(expr, msb, lsb)) {
				emit_expr(scope, ivl_expr_oper2(expr), 0, 0, 0, 1);
			}
		}*/
		//alif: made non number expressions invalid
		else{
			error("Invalid expression for signal select");
		}
	}
	tmp_sig->lsb = value;
	tmp_sig->msb = value;
}


void emit_scaled_expr(ivl_scope_t scope, ivl_expr_t expr, int msb, int lsb, SMTArray** array) {
	int64_t value = 0;
	if (msb >= lsb) {
		if (ivl_expr_type(expr) == IVL_EX_NUMBER) {
			int rtype;
			value = get_in_range_int64_from_number(expr, &rtype,
					"value");
			if (rtype < 0) fprintf(g_out, "1'bx");
			if (rtype) return;
			value += lsb;
			// fprintf(g_out, "%" PRId64, value);
			//ivl_signal_t sig = ivl_expr_signal(expr);
			//(*array)->parent = SMTSigCore::get_parent(sig);
			(*array)->array_index = value;
			string bit_str(ivl_expr_bits(expr));
			int num_zeros = (*array)->parent->index_width - bit_str.size();
			for(int i = 0;i < num_zeros;++i){
				bit_str += "0";
			}
			//DANGEROUS: ARRAY INDEX IS ALWAYS UNSIGNED
			(*array)->_index = emit_number(bit_str.c_str(), (*array)->parent->index_width, 0);
		} /*else if (lsb == 0) {
			// If the LSB is zero then there is no scale.
			emit_expr(scope, expr, 0, 0, 0, 1);
		} else {
			if (is_scaled_expr(expr, msb, lsb)) {
				emit_expr(scope, ivl_expr_oper1(expr), 0, 0, 0, 1);
			}
		}
		*/
		else if(ivl_expr_type(expr) == IVL_EX_SELECT){
			ivl_expr_t expr1 = ivl_expr_oper1(expr);
			const char* expr_name = ivl_expr_name(expr1);
			(*array)->is_index_term = true;
			ivl_signal_t sig = ivl_expr_signal(expr1);
			(*array)->index_term = SMTSigCore::get_parent(sig);
			(*array)->array_index_str = expr_name;
			// fprintf(g_out,"%s", expr_name);

			(*array)->_index = emit_expr_select(scope,expr);
			
			//ziyue: Array Problem
			//error("Invalid expression for signal select");
		}
		//alif: made non number expressions invalid
		else{
			(*array)->_index = emit_expr_binary(scope,expr);
			info("Invalid expression for signal select");
		}
	} else {
		if (ivl_expr_type(expr) == IVL_EX_NUMBER) {
			int rtype;
			value = get_in_range_int64_from_number(expr, &rtype,
					"value");
			if (rtype < 0) fprintf(g_out, "1'bx");
			if (rtype) return;
			value = (int64_t) lsb - value;
			fprintf(g_out, "%" PRId64, value);
		} /*else {
			if (is_scaled_expr(expr, msb, lsb)) {
				emit_expr(scope, ivl_expr_oper2(expr), 0, 0, 0, 1);
			}
		}*/
		//alif: made non number expressions invalid
		else{
			error("Invalid expression for signal select");
		}
	}
}


static SMTSignal* find_signal_in_nexus(ivl_scope_t scope, ivl_nexus_t nex) {
	ivl_signal_t use_sig = 0;
	unsigned is_driver = 0;
	unsigned is_array = 0;
	int64_t array_idx = 0;
	unsigned idx, count = ivl_nexus_ptrs(nex);

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
		/* We have a signal that can be used to find the name. */
		if (scope == ivl_signal_scope(sig)) {
			if (use_sig) {
				/* Swap a receiver for a driver. */
				if (is_driver &&
						(ivl_nexus_ptr_drive1(nex_ptr) == IVL_DR_HiZ) &&
						(ivl_nexus_ptr_drive0(nex_ptr) == IVL_DR_HiZ)) {
					use_sig = sig;
					is_driver = 0;
					if (ivl_signal_dimensions(sig) > 0) {
						is_array = 1;
						array_idx = ivl_nexus_ptr_pin(nex_ptr);
						array_idx += ivl_signal_array_base(sig);
					}
					continue;
				}
				// HERE: Which one should we use? For now it's the first one found.
				//       I believe this needs to be solved (see the inout.v test).
				fprintf(stderr, "%s:%u: vlog95 warning: Duplicate "
						"name (%s",
						ivl_signal_file(sig),
						ivl_signal_lineno(sig),
						ivl_signal_basename(sig));
				if (ivl_signal_dimensions(sig) > 0) {
					int64_t tmp_idx = ivl_nexus_ptr_pin(nex_ptr);
					tmp_idx += ivl_signal_array_base(sig);
					fprintf(stderr, "[%" PRId64"]", tmp_idx);
				}
				fprintf(stderr, ") found for nexus (%s",
						ivl_signal_basename(use_sig));
				if (is_array) fprintf(stderr, "[%" PRId64"]", array_idx);
				fprintf(stderr, ")\n");
			} else {
				use_sig = sig;
				/* This signal is a driver. */
				if ((ivl_nexus_ptr_drive1(nex_ptr) != IVL_DR_HiZ) ||
						(ivl_nexus_ptr_drive0(nex_ptr) != IVL_DR_HiZ)) {
					is_driver = 1;
				}
				if (ivl_signal_dimensions(sig) > 0) {
					is_array = 1;
					array_idx = ivl_nexus_ptr_pin(nex_ptr);
					array_idx += ivl_signal_array_base(sig);
				}
			}
		}
	}

	if (use_sig) {
		emit_id(ivl_signal_basename(use_sig));
		if (is_array) {
			error("Array currently not supported");
			fprintf(g_out, "[%" PRId64"]", array_idx);
		}
		return new SMTSignal(use_sig);
	}

	return NULL;
}

static void emit_number_as_string(ivl_net_const_t net_const) {
	const char *bits = ivl_const_bits(net_const);
	unsigned count = ivl_const_width(net_const);
	int idx;

	assert((count % 8) == 0);
	fprintf(g_out, "\"");
	for (idx = (int) count - 1; idx >= 0; idx -= 8) {
		unsigned bit;
		char val = 0;
		for (bit = 0; bit < 8; bit += 1) {
			val |= (bits[idx - bit] == '1') ? 1 << (7 - bit) : 0x00;
		}

		/* Skip any NULL bytes. */
		if (val == 0) continue;
		/* Print some values that can be escaped. */
		if (val == '"') fprintf(g_out, "\\\"");
		else if (val == '\\') fprintf(g_out, "\\\\");
		else if (val == '\n') fprintf(g_out, "\\n");
		else if (val == '\t') fprintf(g_out, "\\t");
			/* Print the printable characters. */
		else if (isprint((int) val)) fprintf(g_out, "%c", val);
			/* Print the non-printable characters as an octal escape. */
		else fprintf(g_out, "\\%03o", val);
	}
	fprintf(g_out, "\"");
}

/*static SMTSignal* emit_as_input(ivl_scope_t scope, ivl_net_const_t net_const) {
	ivl_scope_t const_scope = ivl_const_scope(net_const);
	ivl_scope_t parent = ivl_scope_parent(scope);

	// Look to see if the constant scope is a parent of this scope.
	while (parent) {
		if (parent == const_scope) break;
		parent = ivl_scope_parent(parent);
	}

	// If the constant scope is a parent then look for an input in
	// this scope and use that for the name. 
	if (parent) {
		ivl_nexus_t nex = ivl_const_nex(net_const);
		unsigned idx, count = ivl_nexus_ptrs(nex);
		for (idx = 0; idx < count; idx += 1) {
			ivl_nexus_ptr_t nex_ptr = ivl_nexus_ptr(nex, idx);
			ivl_signal_t sig = ivl_nexus_ptr_sig(nex_ptr);
			if (sig && (ivl_signal_port(sig) == IVL_SIP_INPUT)) {
				emit_id(ivl_signal_basename(sig));
				return new SMTSignal(sig);
			}
		}
	}

	return NULL;
}*/

SMTNumber* emit_const_nexus(ivl_scope_t scope, ivl_net_const_t net_const) {
	(void)scope;
	//ivl_scope_t const_scope = ivl_const_scope(net_const);
	//unsigned idx, count, lineno;
	//const char *file;
	//count = ivl_scope_params(const_scope);
	//file = ivl_const_file(net_const);
	//lineno = ivl_const_lineno(net_const);
	/* Look to see if the constant matches a parameter in its scope. */
	/*for (idx = 0; idx < count; idx += 1) {
		ivl_parameter_t par = ivl_scope_param(const_scope, idx);
		if (lineno != ivl_parameter_lineno(par)) continue;
		if (strcmp(file, ivl_parameter_file(par)) == 0) {
			// Check that the appropriate expression bits match the
			// original parameter bits.
			// HERE: Verify that the values match and then print the name.
			//       Does this work with out of scope references? Check real parameters.
			emit_id(ivl_parameter_basename(par));
			return;
		}
	}*/

	/* If the scopes don't match then we assume this is an empty port. */
	/*if (const_scope != scope) {
		// This constant could really be from an input port.
		if (emit_as_input(scope, net_const)) return;
		fprintf(g_out, " ");
		return;
	}*/
	//SMTNumber* smt_number = NULL;
	switch (ivl_const_type(net_const)) {
		case IVL_VT_LOGIC:
		case IVL_VT_BOOL:
			return emit_number(ivl_const_bits(net_const), ivl_const_width(net_const), ivl_const_signed(net_const));
		case IVL_VT_STRING:
			error("emit number as string (%s:%u)", ivl_const_file(net_const), ivl_const_lineno(net_const));
			emit_number_as_string(net_const);
			break;
		default:
			fprintf(g_out, "<invalid>");
			fprintf(stderr, "%s:%u: vlog95 error: Unknown constant type "
					"(%d).\n",
					ivl_const_file(net_const),
					ivl_const_lineno(net_const),
					(int) ivl_const_type(net_const));
			g_errors += 1;
			break;
	}
	return NULL;
}

static SMTNumber* find_const_nexus(ivl_scope_t scope, ivl_nexus_t nex) {
	unsigned idx, count;
	error("check: find_const_nexus");
	count = ivl_nexus_ptrs(nex);
	for (idx = 0; idx < count; idx += 1) {
		ivl_nexus_ptr_t nex_ptr = ivl_nexus_ptr(nex, idx);
		ivl_net_const_t net_const = ivl_nexus_ptr_con(nex_ptr);
		// HERE: Do we need to check for duplicates?
		if (net_const) {
			assert(!ivl_nexus_ptr_pin(nex_ptr));
			return emit_const_nexus(scope, net_const);
		}
	}
	return NULL;
}

static SMTSignal* find_driving_signal(ivl_scope_t scope, ivl_nexus_t nex) {
	ivl_signal_t sig = 0;
	unsigned is_array = 0;
	int64_t array_idx = 0;
	unsigned idx, count = ivl_nexus_ptrs(nex);

	for (idx = 0; idx < count; idx += 1) {
		ivl_nexus_ptr_t nex_ptr = ivl_nexus_ptr(nex, idx);
		ivl_signal_t t_sig = ivl_nexus_ptr_sig(nex_ptr);
		if (!t_sig) continue;
		if (ivl_signal_local(t_sig)) continue;
		/* An output can be used if it is driven by this nexus. */
		if ((ivl_nexus_ptr_drive1(nex_ptr) == IVL_DR_HiZ) &&
				(ivl_nexus_ptr_drive0(nex_ptr) == IVL_DR_HiZ) &&
				(ivl_signal_port(t_sig) != IVL_SIP_OUTPUT)) {
			continue;
		}
		/* We have a signal that can be used to find the name. */
		if (sig) {
			// HERE: Which one should we use? For now it's the first one found.
			//       I believe this needs to be solved (see above).
			fprintf(stderr, "%s:%u: vlog95 warning: Duplicate name (%s",
					ivl_signal_file(t_sig), ivl_signal_lineno(t_sig),
					ivl_signal_basename(t_sig));
			if (ivl_signal_dimensions(t_sig) > 0) {
				int64_t tmp_idx = ivl_nexus_ptr_pin(nex_ptr);
				tmp_idx += ivl_signal_array_base(t_sig);
				fprintf(stderr, "[%" PRId64"]", tmp_idx);
			}
			fprintf(stderr, ") found for nexus (%s",
					ivl_signal_basename(sig));
			if (is_array) fprintf(stderr, "[%" PRId64"]", array_idx);
			fprintf(stderr, ")\n");
		} else {
			sig = t_sig;
			if (ivl_signal_dimensions(sig) > 0) {
				is_array = 1;
				array_idx = ivl_nexus_ptr_pin(nex_ptr);
				array_idx += ivl_signal_array_base(sig);
			}
		}
	}

	if (sig) {
		emit_scope_call_path(scope, ivl_signal_scope(sig));
		emit_id(ivl_signal_basename(sig));
		if (is_array) {
			error("Currently array not supported");
			fprintf(g_out, "[%" PRId64"]", array_idx);
		}
		return new SMTSignal(sig);
	}

	return NULL;
}

static SMTSignal* is_local_input(ivl_scope_t scope, ivl_nexus_t nex) {
	ivl_signal_t sig = 0;
	unsigned idx, count = ivl_nexus_ptrs(nex);

	(void) scope; /* Parameter is not used. */
	for (idx = 0; idx < count; idx += 1) {
		ivl_nexus_ptr_t nex_ptr = ivl_nexus_ptr(nex, idx);
		ivl_signal_t t_sig = ivl_nexus_ptr_sig(nex_ptr);
		if (!t_sig) continue;
		if (!ivl_signal_local(t_sig)) continue;
		if (ivl_signal_port(t_sig) != IVL_SIP_INPUT) continue;
		assert(!sig);
		assert(ivl_signal_dimensions(t_sig) == 0);
		sig = t_sig;
	}
	if (sig) {
		fprintf(g_out, "ivlog%s", ivl_signal_basename(sig));
		return new SMTSignal(sig);
	}
	return NULL;
}

// HERE: Does this work correctly with an array reference created from @*?

SMTExpr* emit_name_of_nexus(ivl_scope_t scope, ivl_nexus_t nex, unsigned allow_UD) {
	ivl_scope_t mod_scope;
    SMTExpr* smt_expr;
	/* First look in the local scope for the nexus name. */
	if ((smt_expr = find_signal_in_nexus(scope, nex))) return smt_expr;

	/* If the signal was not found in the passed scope then look in
	 * the module scope if the passed scope was not the module scope. */
	mod_scope = get_module_scope(scope);
	if (mod_scope != scope) {
		if ((smt_expr = find_signal_in_nexus(mod_scope, nex))) return smt_expr;
	}

	/* Look to see if this is a up/down reference. */
	if (allow_UD && (smt_expr = find_driving_signal(scope, nex))) return smt_expr;

	/* If there is no signals driving this then look for a constant. */
	if ((smt_expr = find_const_nexus(scope, nex))) return smt_expr;

	/* Module inputs that are split (arg[7:4], arg[3:0]) need to use
	 * the local signal names. */
	if ((smt_expr = is_local_input(scope, nex))) return smt_expr;

	// HERE: Need to check arr[var]? Can this be rebuilt?
	//       Then look for down scopes and then any scope. For all this warn if
	//       multiples are found in a given scope. This all needs to be before
	//       the constant code.

	/* It is possible that the nexus does not have a name. For this
	 * case do not print an actual name. */
	fprintf(g_out, "/* Empty */");
	//      dump_nexus_information(scope, nex);
    return new SMTUnspecified();
}

/*
 * This function traverses the scope tree looking for the enclosing module
 * scope. When it is found the module scope is returned. As far as this
 * translation is concerned a package is a special form of a module
 * definition and a class is also a top level scope.
 */
ivl_scope_t get_module_scope(ivl_scope_t scope) {

	while ((ivl_scope_type(scope) != IVL_SCT_MODULE) &&
			(ivl_scope_type(scope) != IVL_SCT_PACKAGE) &&
			(ivl_scope_type(scope) != IVL_SCT_CLASS)) {
		ivl_scope_t pscope = ivl_scope_parent(scope);
		assert(pscope);
		scope = pscope;
	}
	return scope;
}

/*
 * A package is emitted as a module with a special name. This routine
 * calculates the name for the package. The returned string must be freed
 * by the calling routine.
 */
char * get_package_name(ivl_scope_t scope) {
	char *package_name;
	const char *name = ivl_scope_basename(scope);
	package_name = (char *) malloc(strlen(name) + 13);
	strcpy(package_name, "ivl_package_");
	strcat(package_name, name);
	return package_name;
}

static void emit_scope_piece(ivl_scope_t scope, ivl_scope_t call_scope) {
	ivl_scope_t parent = ivl_scope_parent(call_scope);
	/* If we are not at the top of the scope (parent != 0) and the two
	 * scopes do not match then print the parent scope. */
	if ((parent != 0) && (scope != parent)) {
		emit_scope_piece(scope, parent);
	}
	/* If the scope is a package then add the special part of the name. */
	if (ivl_scope_type(call_scope) == IVL_SCT_PACKAGE) {
		char *package_name = get_package_name(call_scope);
		emit_id(package_name);
		free(package_name);
		/* Print the base scope. */
	} else emit_id(ivl_scope_basename(call_scope));
	fprintf(g_out, ".");
}

/*
 * This routine emits the appropriate string to call the call_scope from the
 * given scope. If the module scopes for the two match then do nothing. If
 * the module scopes are different, but the call_scope begins with the
 * entire module scope of scope then we can trim the top off the call_scope
 * (it is a sub-scope of the module that contains scope). Otherwise we need
 * to print the entire path of call_scope.
 */
void emit_scope_module_path(ivl_scope_t scope, ivl_scope_t call_scope) {
	ivl_scope_t mod_scope = get_module_scope(scope);
	ivl_scope_t call_mod_scope = get_module_scope(call_scope);

	if (mod_scope == call_mod_scope) return;
	emit_scope_piece(mod_scope, call_mod_scope);
}

/* This is the same as emit_scope_module_path() except we need to add down
 * references for variables, etc. */
void emit_scope_call_path(ivl_scope_t scope, ivl_scope_t call_scope) {
	ivl_scope_t mod_scope, call_mod_scope;

	if (scope == call_scope) return;

	mod_scope = get_module_scope(scope);
	call_mod_scope = get_module_scope(call_scope);

	if (mod_scope != call_mod_scope) {
		emit_scope_piece(mod_scope, call_mod_scope);
	} else if (scope != call_scope) {
		ivl_scope_t parent;
		/* We only emit a scope path if the scope is a parent of the
		 * call scope. */
		for (parent = ivl_scope_parent(call_scope);
				parent != 0;
				parent = ivl_scope_parent(parent)) {
			if (parent == scope) {
				emit_scope_piece(scope, call_scope);
				return;
			}
		}
	}
}

static void emit_scope_path_piece(ivl_scope_t scope, ivl_scope_t call_scope) {
	ivl_scope_t parent = ivl_scope_parent(call_scope);
	/* If we are not at the top of the scope (parent != 0) and the two
	 * scopes do not match then print the parent scope. */
	if ((parent != 0) && (scope != parent)) {
		emit_scope_path_piece(scope, parent);
		fprintf(g_out, ".");
	}
	/* If the scope is a package then add the special part of the name. */
	if (ivl_scope_type(call_scope) == IVL_SCT_PACKAGE) {
		char *package_name = get_package_name(call_scope);
		emit_id(package_name);
		free(package_name);
		/* Print the base scope. */
	} else emit_id(ivl_scope_basename(call_scope));
}

/*
 * This routine emits the appropriate string to call the call_scope from the
 * given scope. If the module scopes for the two match then just return the
 * base name of the call_scope. If the module scopes are different, but the
 * call_scope begins with the entire module scope of scope then we can trim
 * the top off the call_scope (it is a sub-scope of the module that contains
 * scope). Otherwise we need to print the entire path of call_scope.
 */
void emit_scope_path(ivl_scope_t scope, ivl_scope_t call_scope) {
	ivl_scope_t mod_scope, call_mod_scope;

	/* Check to see if this is a root scope task or function. */
	if (ivl_scope_parent(call_scope) == 0) {
		fprintf(g_out, "ivl_root_scope.");
		mod_scope = 0;
		call_mod_scope = 0;
	} else {
		mod_scope = get_module_scope(scope);
		call_mod_scope = get_module_scope(call_scope);
	}

	if (mod_scope == call_mod_scope) {
		emit_id(ivl_scope_basename(call_scope));
	} else {
		emit_scope_path_piece(mod_scope, call_scope);
	}
}

void emit_id(const char *id) {
	fprintf(g_out, "%s", id);
}

/*
 * Get the correct MSB and LSB for a signal.
 */
void get_sig_msb_lsb(ivl_signal_t sig, int *msb, int *lsb) {
	switch (ivl_signal_packed_dimensions(sig)) {
			/* For a scalar we use zero for both the MSB and LSB. */
		case 0:
			*msb = 0;
			*lsb = 0;
			break;
		case 1:
			/* For a vector we use the real MSB and LSB. */
			*msb = ivl_signal_packed_msb(sig, 0);
			*lsb = ivl_signal_packed_lsb(sig, 0);
			break;
			/* For a packed vector we use the normalized MSB and LSB. */
		default:
			*msb = ivl_signal_width(sig) - 1;
			*lsb = 0;
			break;
	}
}

void error(const char *fmt, ...) {
	va_list args;

	printf("[ERROR] ");
	va_start(args, fmt);
	vprintf(fmt, args);
	putchar('\n');
	va_end(args);
	
	if(g_out){
		fclose(g_out);
		g_out = NULL;
	}
	
	exit(0);
}

void info(const char *fmt, ...) {
	va_list args;

	va_start(args, fmt);

	printf("[INFO] ");
	vprintf(fmt, args);
	putchar('\n');

	va_end(args);
}

bool is_file_exists(const char* file){
    struct stat buffer;
    return (stat(file, &buffer) == 0);
}
