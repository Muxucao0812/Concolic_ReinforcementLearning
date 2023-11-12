#include <stdlib.h>
#include <string.h>

#include "concolic.h"

void emit_event(ivl_scope_t scope, ivl_statement_t stmt) {
	unsigned eidx, nevents, first = 1;
	nevents = ivl_stmt_nevent(stmt);
    assert(nevents == 1);
	for (eidx = 0; eidx < nevents; eidx += 1) {
		SMTSignal* smt_expr;
		unsigned idx, count;
		ivl_event_t event = ivl_stmt_events(stmt, eidx);

        bool have_edge_trig = false, have_level_trig = false;
        
		/* Check for any edge events. */
		count = ivl_event_nany(event);
		for (idx = 0; idx < count; idx += 1) {
            have_level_trig = true;
			if (first) first = 0;
			else fprintf(g_out, " or ");
			emit_nexus_as_ca(scope, ivl_event_any(event, idx), 0, 0);
		}

		/* Check for positive edge events. */
		count = ivl_event_npos(event);
		for (idx = 0; idx < count; idx += 1) {
            have_edge_trig = true;
			if (first) first = 0;
			else fprintf(g_out, " or ");
			fprintf(g_out, "posedge ");
			smt_expr = dynamic_cast<SMTSignal*>(emit_nexus_as_ca(scope, ivl_event_pos(event, idx), 0, 0));
			assert(smt_expr);
			SMTProcess::curr_proc->is_edge_triggered = true;
		}

		/* Check for negative edge events. */
		count = ivl_event_nneg(event);
		for (idx = 0; idx < count; idx += 1) {
            have_edge_trig = true;
			if (first) first = 0;
			else fprintf(g_out, " or ");
			fprintf(g_out, "negedge ");
			smt_expr = dynamic_cast<SMTSignal*>(emit_nexus_as_ca(scope, ivl_event_neg(event, idx), 0, 0));
			assert(smt_expr);
			SMTProcess::curr_proc->is_edge_triggered = true;
		}

		/* We have a named event if there were no edge or level events. */
		if (!have_edge_trig && !have_level_trig) {
			error("Check: named event");
			ivl_scope_t ev_scope = ivl_event_scope(event);
			if (first) first = 0;
			else fprintf(g_out, " or ");
			emit_scope_module_path(scope, ev_scope);
			emit_id(ivl_event_basename(event));
		}
        
        //check if both level and edge triggered
        if(have_edge_trig && have_level_trig){
            error("Trigger is both edge and level sensitive");
        }
        
        if(have_level_trig && enable_obs_padding){
        	fprintf(g_out, " or __obs");
    	}
	}
}
