#ifndef _ASM_X86_ALTERNATIVE_ASM_H
#define _ASM_X86_ALTERNATIVE_ASM_H

#ifdef __ASSEMBLY__

/*
 * Issue one struct alt_instr descriptor entry (need to put it into
 * the section .altinstructions, see below). This entry contains
 * enough information for the alternatives patching code to patch an
 * instruction. See apply_alternatives().
 */
.macro altinstruction_entry orig alt feature orig_len alt_len pad_len.
    long \orig -..long \alt -..word \ feature.byte \ orig_len.byte \ alt_len.
    long \pad_len.endm
/*
 * Define an alternative between two instructions. If @feature is
 * present, early code in apply_alternatives() replaces @oldinstr with
 * @newinstr. ".skip" directive takes care of proper instruction padding
 * in case @newinstr is longer than @oldinstr.
 */
.macro ALTERNATIVE oldinstr, newinstr, feature
    140:
    \oldinstr
    141:.skip - (((144f - 143f) - (141 b - 140 b)) >
                 0) * ((144f - 143f) - (141 b - 140 b)),
    0x90 142:.pushsection.altinstructions, "a" altinstruction_entry 140 b, 143f,
    \feature, 142 b - 140 b, 144f - 143f,
    142 b - 141 b.popsection.pushsection.altinstr_replacement,
    "ax" 143: \newinstr 144:.popsection.endm
#define old_len			141b-140b
#define new_len1		144f-143f
#define new_len2		145f-144f
/*
 * max without conditionals. Idea adapted from:
 * http://graphics.stanford.edu/~seander/bithacks.html#IntegerMinOrMax
 */
#define alt_max_short(a, b)	((a) ^ (((a) ^ (b)) & -(-((a) < (b)))))
/*
 * Same as ALTERNATIVE macro above but for two alternatives. If CPU
 * has @feature1, it replaces @oldinstr with @newinstr1. If CPU has
 * @feature2, it replaces @oldinstr with @feature2.
 */
.macro ALTERNATIVE_2 oldinstr, newinstr1, feature1, newinstr2, feature2
    140:
    \oldinstr
    141:.skip - ((alt_max_short(new_len1, new_len2) - (old_len)) > 0) *
    (alt_max_short(new_len1, new_len2) - (old_len)), 0x90
    142:.pushsection.altinstructions, "a"
    altinstruction_entry 140 b, 143f, \feature1, 142 b - 140 b, 144f - 143f,
    142 b - 141 b altinstruction_entry 140 b, 144f, \feature2, 142 b - 140 b,
    145f - 144f, 142 b - 141 b.popsection.pushsection.altinstr_replacement,
    "ax" 143: \newinstr1 144: \newinstr2 145:.popsection.endm
#endif                          /*  __ASSEMBLY__  */
#endif                          /* _ASM_X86_ALTERNATIVE_ASM_H */
