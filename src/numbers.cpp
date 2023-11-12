#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "concolic.h"

/*
 * Extract an int32_t value from the given bit information. If the result
 * type is 0 then the returned value is valid. If it is positive then the
 * value was too large and if it is negative then the value had undefined
 * bits. -2 is all bits z and -3 is all bits x.
 */
static int32_t get_int32_from_bits(const char *bits, unsigned nbits,
		unsigned is_signed, int *result_type) {
	unsigned trim_wid = nbits - 1;
	const char msb = is_signed ? bits[trim_wid] : '0';
	unsigned idx;
	int32_t value = 0;
	/* Trim any duplicate bits from the MSB. */
	for (/* none */; trim_wid > 0; trim_wid -= 1) {
		if (msb != bits[trim_wid]) {
			trim_wid += 1;
			break;
		}
	}
	if (trim_wid < nbits) trim_wid += 1;
	/* Check to see if the value is too large. */
	if (trim_wid > 32U) {
		*result_type = trim_wid;
		return 0;
	}
	/* Now build the value from the bits. */
	for (idx = 0; idx < trim_wid; idx += 1) {
		if (bits[idx] == '1') value |= (int32_t) 1 << idx;
		else if (bits[idx] != '0') {
			*result_type = -1;
			/* If the value is entirely x/z then return -2 or -3. */
			if (trim_wid == 1) {
				if (bits[idx] == 'x') *result_type -= 1;
				*result_type -= 1;
			}
			return 0;
		}
	}
	/* Sign extend as needed. */
	// HERE: Need to emit 1 instead of -1 for some of the constants.
	//      if (is_signed && (nbits > 1) && (msb == '1') && (trim_wid < 32U)) {
	if (is_signed && (msb == '1') && (trim_wid < 32U)) {
		value |= ~(((int32_t) 1 << trim_wid) - (int32_t) 1);
	}
	*result_type = 0;
	return value;
}

SMTNumber* emit_number(const char *bits, unsigned nbits, bool is_signed) {
	SMTNumber* smt_number = new SMTNumber(bits, nbits, is_signed);
	smt_number->emit_verilog_value();
	return smt_number;
}

/*
 * Extract an uint64_t value from the given number expression. If the result
 * type is 0 then the returned value is valid. If it is positive then the
 * value was too large and if it is negative then the value had undefined
 * bits.
 */
uint64_t get_uint64_from_number(ivl_expr_t expr, int *result_type) {
	unsigned nbits = ivl_expr_width(expr);
	unsigned trim_wid = nbits - 1;
	const char *bits = ivl_expr_bits(expr);
	unsigned idx;
	uint64_t value = 0;
	assert(ivl_expr_type(expr) == IVL_EX_NUMBER);
	assert(!ivl_expr_signed(expr));
	/* Trim any '0' bits from the MSB. */
	for (/* none */; trim_wid > 0; trim_wid -= 1) {
		if ('0' != bits[trim_wid]) {
			trim_wid += 1;
			break;
		}
	}
	if (trim_wid < nbits) trim_wid += 1;
	/* Check to see if the value is too large. */
	if (trim_wid > 64U) {
		*result_type = trim_wid;
		return 0;
	}
	/* Now build the value from the bits. */
	for (idx = 0; idx < trim_wid; idx += 1) {
		if (bits[idx] == '1') value |= (uint64_t) 1 << idx;
		else if (bits[idx] != '0') {
			*result_type = -1;
			/* If the value is entirely x/z then return -2 or -3. */
			if (trim_wid == 1) {
				if (bits[idx] == 'x') *result_type -= 1;
				*result_type -= 1;
			}
			return 0;
		}
	}
	*result_type = 0;
	return value;
}

/*
 * Extract an int64_t value from the given number expression. If the result
 * type is 0 then the returned value is valid. If it is positive then the
 * value was too large and if it is negative then the value had undefined
 * bits. -2 is all bits z and -3 is all bits x.
 */
int64_t get_int64_from_number(ivl_expr_t expr, int *result_type) {
	unsigned is_signed = ivl_expr_signed(expr);
	unsigned nbits = ivl_expr_width(expr);
	unsigned trim_wid = nbits - 1;
	const char *bits = ivl_expr_bits(expr);
	const char msb = is_signed ? bits[trim_wid] : '0';
	unsigned idx;
	int64_t value = 0;
	assert(ivl_expr_type(expr) == IVL_EX_NUMBER);
	/* Trim any duplicate bits from the MSB. */
	for (/* none */; trim_wid > 0; trim_wid -= 1) {
		if (msb != bits[trim_wid]) {
			trim_wid += 1;
			break;
		}
	}
	if (trim_wid < nbits) trim_wid += 1;
	/* Check to see if the value is too large. */
	if (trim_wid > 64U) {
		*result_type = trim_wid;
		return 0;
	}
	/* Now build the value from the bits. */
	for (idx = 0; idx < trim_wid; idx += 1) {
		if (bits[idx] == '1') value |= (int64_t) 1 << idx;
		else if (bits[idx] != '0') {
			*result_type = -1;
			/* If the value is entirely x/z then return -2 or -3. */
			if (trim_wid == 1) {
				if (bits[idx] == 'x') *result_type -= 1;
				*result_type -= 1;
			}
			return 0;
		}
	}
	/* Sign extend as needed. */
	if (is_signed && (msb == '1') && (trim_wid < 64U)) {
		value |= ~(((int64_t) 1 << trim_wid) - (int64_t) 1);
	}
	*result_type = 0;
	return value;
}

/*
 * Extract an int32_t value from the given number expression. If the result
 * type is 0 then the returned value is valid. If it is positive then the
 * value was too large and if it is negative then the value had undefined
 * bits. -2 is all bits z and -3 is all bits x.
 */
int32_t get_int32_from_number(ivl_expr_t expr, int *result_type) {
	assert(ivl_expr_type(expr) == IVL_EX_NUMBER);
	return get_int32_from_bits(ivl_expr_bits(expr), ivl_expr_width(expr),
			ivl_expr_signed(expr), result_type);
}

