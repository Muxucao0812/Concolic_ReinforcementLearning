#include <cinttypes>
#include <cstring>
#include <vector>
#include "concolic.h"
#include <cstdlib>
#include <cmath>

using namespace std;

static void emit_sig_id(ivl_signal_t sig) {
	emit_id(ivl_signal_basename(sig));
	fprintf(g_out, " = %u\'b0;\n", ivl_signal_width(sig));
	//fprintf(g_out, ";\n");
}

static void emit_var_def(ivl_signal_t sig) {
	fprintf(g_out, "%*c", g_ind, ' ');
	int msb, lsb;
	get_sig_msb_lsb(sig, &msb, &lsb);
	fprintf(g_out, "reg ");
	if(ivl_signal_signed(sig)){
		fprintf(g_out, "signed ");
	}
	if (msb != 0 || lsb != 0) fprintf(g_out, "[%d:%d] ", msb, lsb);
	emit_id(ivl_signal_basename(sig));
	if (ivl_signal_dimensions(sig) > 0) {
		unsigned wd_count = ivl_signal_array_count(sig);
		int first = ivl_signal_array_base(sig);
		int last = first + wd_count - 1;
		if (ivl_signal_array_addr_swapped(sig)) {
			fprintf(g_out, " [%d:%d]", last, first);
		} else {
			fprintf(g_out, " [%d:%d]", first, last);
		}
	}
	if(ivl_signal_array_count(sig)==1){
		fprintf(g_out, " = %u\'b0;\n", ivl_signal_width(sig));
	}else{
		fprintf(g_out, ";\n");
	}
}

/*
 * Keep a list of constants that drive nets and need to be emitted as
 * a continuous assignment.
 */
static vector<ivl_signal_t> net_const_list;

static void add_net_const_to_list(ivl_signal_t net_const) {
	net_const_list.push_back(net_const);
}

static unsigned emit_and_free_net_const_list(ivl_scope_t scope) {
	const uint count = net_const_list.size();
	for (uint idx = 0; idx < count; idx++) {
		emit_signal_net_const_as_ca(scope, net_const_list[idx]);
	}
	net_const_list.clear();
	return count;
}

static void save_net_constants(ivl_scope_t scope, ivl_signal_t sig) {
	ivl_nexus_t nex = ivl_signal_nex(sig, 0);
	unsigned idx, count = ivl_nexus_ptrs(nex);
	for (idx = 0; idx < count; idx += 1) {
		ivl_nexus_ptr_t nex_ptr = ivl_nexus_ptr(nex, idx);
		ivl_net_const_t net_const = ivl_nexus_ptr_con(nex_ptr);
		if (!net_const) continue;
		if (scope != ivl_const_scope(net_const)) continue;
		add_net_const_to_list(sig);
	}
}

static void emit_net_def(ivl_scope_t scope, ivl_signal_t sig) {
	int msb, lsb;
	get_sig_msb_lsb(sig, &msb, &lsb);
	if (ivl_signal_local(sig)) return;
	fprintf(g_out, "%*c", g_ind, ' ');
	switch (ivl_signal_type(sig)) {
		case IVL_SIT_TRI:
		case IVL_SIT_UWIRE:
			// HERE: Need to add support for supply nets. Probably supply strength
			//       with a constant 0/1 driver for all the bits.
			fprintf(g_out, "reg ");
			break;
		default:
			fprintf(g_out, "<unknown> ");
			fprintf(stderr, "%s:%u: vlog95 error: Unknown net type "
					"(%d).\n", ivl_signal_file(sig),
					ivl_signal_lineno(sig), (int) ivl_signal_type(sig));
			g_errors += 1;
			break;
	}
	if(ivl_signal_signed(sig)){
		fprintf(g_out, "signed ");
	}
	if (msb != 0 || lsb != 0) fprintf(g_out, "[%d:%d] ", msb, lsb);
	emit_sig_id(sig);
	/* A constant driving a net does not create an lpm or logic
	 * element in the design so save them from the definition. */
	save_net_constants(scope, sig);
}

/*
 * This function is called for each process in the design so that we
 * can extract the processes for the given scope.
 */
static int find_process(ivl_process_t proc, void* scope) {
	if (scope == ivl_process_scope(proc)) {
		emit_process((ivl_scope_t)scope, proc);
	}
	return 0;
}

void emit_scope_variables(ivl_scope_t scope) {
	// Output the signals for this scope.
	const uint count = ivl_scope_sigs(scope);
	for (uint idx = 0; idx < count; idx += 1) {
		ivl_signal_t sig = ivl_scope_sig(scope, idx);
        if (ivl_signal_local(sig)) continue;
        
        new SMTSigCore(sig);    //create and register signal
		if (ivl_signal_type(sig) == IVL_SIT_REG) {
			emit_var_def(sig);
		}
        else if (ivl_signal_port(sig) != IVL_SIP_INPUT){
			emit_net_def(scope, sig);
		}
	}
	if (count) fprintf(g_out, "\n");
	if (emit_and_free_net_const_list(scope)){
		fprintf(g_out, "\n");
	}
	// SMTSigCore::print_state_variables(std::cout);
}

static void emit_scope_file_line(ivl_scope_t scope) {
	fprintf(g_out, "// Following code segment is generated from %s:%u\n",
			ivl_scope_file(scope), ivl_scope_lineno(scope));
}

static void emit_top_module_ports(ivl_scope_t module) {
	uint count = ivl_scope_mod_module_ports(module);
	if (count == 0) return;
	fprintf(g_out, "(%s", ivl_scope_mod_module_port_name(module, 0));
	for (uint i = 1; i < count; i++) {
		fprintf(g_out, ", %s", ivl_scope_mod_module_port_name(module, i));
	}
	if(enable_obs_padding){
		fprintf(g_out, ", __obs");
	}
	fprintf(g_out, ");\n");
}

static ivl_signal_t get_port_from_nexus(ivl_scope_t scope, ivl_nexus_t nex,
		unsigned *word) {
	assert(nex);
	unsigned idx, count = ivl_nexus_ptrs(nex);
	ivl_signal_t sig = 0;
	*word = 0;
	for (idx = 0; idx < count; idx += 1) {
		ivl_nexus_ptr_t nex_ptr = ivl_nexus_ptr(nex, idx);
		ivl_signal_t t_sig = ivl_nexus_ptr_sig(nex_ptr);
		if (t_sig) {
			if (ivl_signal_scope(t_sig) != scope) continue;
			assert(!sig);
			sig = t_sig;
			*word = ivl_nexus_ptr_pin(nex_ptr);
		}
	}
	return sig;
}

static void emit_sig_type(ivl_signal_t sig) {
	ivl_signal_type_t type = ivl_signal_type(sig);
	if (ivl_signal_dimensions(sig) != 0) {
		fprintf(stderr, "%s:%u: vlog95 error: Array ports (%s) are not "
				"supported.\n",
				ivl_signal_file(sig),
				ivl_signal_lineno(sig),
				ivl_signal_basename(sig));
		g_errors += 1;
	}
	/* Check to see if we have a variable (reg) or a net. */
	if (type == IVL_SIT_REG) {
		/* The variable data type will be declared later, so here
	   we just want to declare the range and whether or not it
	   is signed. */
		if (ivl_signal_integer(sig)) {
			/* nothing to do */
		} else if (ivl_signal_data_type(sig) == IVL_VT_REAL) {
			/* nothing to do */
		} else {
			int msb, lsb;
			get_sig_msb_lsb(sig, &msb, &lsb);
			if (ivl_signal_signed(sig)) {
				fprintf(g_out, " signed");
			}
			if (msb != 0 || lsb != 0) {
				fprintf(g_out, " [%d:%d]", msb, lsb);
			}
		}
	} else {
		assert((type == IVL_SIT_TRI) ||
				(type == IVL_SIT_TRI0) ||
				(type == IVL_SIT_TRI1) ||
				(type == IVL_SIT_UWIRE));
		if (ivl_signal_data_type(sig) == IVL_VT_REAL) {
			fprintf(stderr, "%s:%u: vlog95 error: Real net ports (%s) "
					"are not supported.\n",
					ivl_signal_file(sig),
					ivl_signal_lineno(sig),
					ivl_signal_basename(sig));
			g_errors += 1;
		} else {
			int msb, lsb;
			get_sig_msb_lsb(sig, &msb, &lsb);
			if (ivl_signal_signed(sig)) {
				fprintf(g_out, " signed");
			}
			if (msb != 0 || lsb != 0) {
				fprintf(g_out, " [%d:%d]", msb, lsb);
			}
		}
	}
}

static void emit_port(ivl_signal_t port) {
	assert(port);
	fprintf(g_out, "%*c", g_ind, ' ');

	switch (ivl_signal_port(port)) {
		case IVL_SIP_INPUT:
			fprintf(g_out, "input");
			break;
		case IVL_SIP_OUTPUT:
			fprintf(g_out, "output");
			break;
		case IVL_SIP_INOUT:
			fprintf(g_out, "inout");
			break;
		default:
			fprintf(g_out, "<unknown>");
			fprintf(stderr, "%s:%u: vlog95 error: Unknown port direction (%d) "
					"for signal %s.\n", ivl_signal_file(port),
					ivl_signal_lineno(port), (int) ivl_signal_port(port),
					ivl_signal_basename(port));
			g_errors += 1;
			break;
	}
	emit_sig_type(port);
	fprintf(g_out, " ");
	/* Split port (arg[7:4],arg[3:0]) are generated using local signals. */
	if (ivl_signal_local(port)) {
        error("Check local port");
		fprintf(g_out, "ivlog%s", ivl_signal_basename(port));
	} else {
		emit_id(ivl_signal_basename(port));
	}
	fprintf(g_out, ";\n");
}

static void emit_module_port_defs(ivl_scope_t scope) {
	unsigned word, idx, count = ivl_scope_ports(scope);
	for (idx = 0; idx < count; idx += 1) {
		ivl_nexus_t nex = ivl_scope_mod_port(scope, idx);
		ivl_signal_t port = get_port_from_nexus(scope, nex, &word);
		if (port){
            emit_port(port);
        }
		else {
			error("Port not found");
		}
	}
	if(enable_obs_padding){
		fprintf(g_out, "%*cinput __obs;\n", g_ind, ' ');
	}
	if (count) fprintf(g_out, "\n");
}

void emit_module_variables_display(){
	fprintf(g_out, "%*c// Displaying module variables\n", g_ind, ' ');
	fprintf(g_out, "%*cbegin\n", g_ind, ' ');
	g_ind += g_ind_incr;
    for(auto sig: SMTSigCore::get_reg_list()){
		if(sig->is_array == false){
        	fprintf(g_out, "%*c$display(\";R %s = %%b\", %s);\n", g_ind, ' ', sig->get_name().c_str(), sig->get_name().c_str());
		}else{
			for (uint i = 0; i < static_cast<uint>(std::pow(2, sig->index_width)); i++) {
				fprintf(g_out, "%*c$display(\";R %s[%d] = %%b\", %s[%d]);\n", g_ind, ' ', sig->get_name().c_str(), i, sig->get_name().c_str(), i);
			}
		}
	}
	g_ind -= g_ind_incr;
	fprintf(g_out, "%*cend\n", g_ind, ' ');
};

// Emit root module
void emit_root(ivl_scope_t root) {
    //check if hierarchy is flattened 
	assert(ivl_scope_childs(root) == 0);

	// Output module declaration
	emit_scope_file_line(root);
	fprintf(g_out, "module %s", ivl_scope_basename(root));
	emit_top_module_ports(root);

	// Output module port definitions
	g_ind = g_ind_incr;
	emit_module_port_defs(root);
	emit_scope_variables(root);

	uint count = ivl_scope_lpms(root);
	for (uint idx = 0; idx < count; idx += 1) {
		emit_lpm(root, ivl_scope_lpm(root, idx));
	}

	count = ivl_scope_logs(root);
	for (uint idx = 0; idx < count; idx += 1) {
		emit_logic(root, ivl_scope_log(root, idx));
	}

	count = ivl_scope_switches(root);
	for (uint idx = 0; idx < count; idx += 1) {
		error("check: emit_tran");
	}

	/* Output the initial/always blocks for this module. */
	ivl_design_process(g_design, (ivl_process_f) find_process, root);

	// Output the endmodule
	fprintf(g_out, "endmodule\n\n");
}

