	.build_version macos, 16, 0	sdk_version 26, 0
	.section	__TEXT,__text,regular,pure_instructions
	.globl	__ZN10playground26get_right_triangle_triplesEv ; -- Begin function _ZN10playground26get_right_triangle_triplesEv
	.p2align	2
__ZN10playground26get_right_triangle_triplesEv: ; @_ZN10playground26get_right_triangle_triplesEv
Lfunc_begin0:
	.cfi_startproc
	.cfi_personality 155, ___gxx_personality_v0
	.cfi_lsda 16, Lexception0
; %bb.0:
	sub	sp, sp, #464
	stp	x22, x21, [sp, #416]            ; 16-byte Folded Spill
	stp	x20, x19, [sp, #432]            ; 16-byte Folded Spill
	stp	x29, x30, [sp, #448]            ; 16-byte Folded Spill
	add	x29, sp, #448
	.cfi_def_cfa w29, 16
	.cfi_offset w30, -8
	.cfi_offset w29, -16
	.cfi_offset w19, -24
	.cfi_offset w20, -32
	.cfi_offset w21, -40
	.cfi_offset w22, -48
	sub	x19, x29, #136
	sub	x8, x29, #145
	sub	x9, x29, #144
	stp	x8, x9, [sp, #112]
	mov	w20, #1                         ; =0x1
	str	x20, [sp, #128]
	strb	wzr, [sp, #136]
	strb	wzr, [sp, #144]
	strb	wzr, [sp, #280]
	mov	w8, #1000                       ; =0x3e8
	str	x8, [sp, #288]
	add	x8, sp, #112
	stp	xzr, x8, [x29, #-144]
	stur	x20, [x29, #-128]
	sturb	wzr, [x29, #-120]
	sturb	wzr, [x29, #-48]
	stur	x8, [x29, #-40]
	sub	x0, x29, #136
	bl	__ZNSt3__16ranges9join_viewINS0_14transform_viewB10llvm18_nuaINS0_9iota_viewIxNS_22unreachable_sentinel_tEEEZN10playground26get_right_triangle_triplesEvE3$_0EEE10__iteratorILb0EE9__satisfyB8ne200100Ev
	ldr	x8, [sp, #288]
	ldp	q0, q1, [x19, #64]
	stp	q0, q1, [sp, #64]
	ldur	x9, [x29, #-40]
	ldp	q0, q1, [x19]
	stp	q0, q1, [sp]
	ldp	q1, q0, [x19, #32]
	stp	q1, q0, [sp, #32]
	stp	x9, x8, [sp, #96]
	mov	x19, #0                         ; =0x0
	cbz	x8, LBB0_27
; %bb.1:
	movi.2d	v1, #0000000000000000
	b	LBB0_3
LBB0_2:                                 ;   in Loop: Header=BB0_3 Depth=1
	add	x19, x21, x19
	subs	x8, x8, #1
	str	x8, [sp, #104]
	b.eq	LBB0_27
LBB0_3:                                 ; =>This Loop Header: Depth=1
                                        ;     Child Loop BB0_7 Depth 2
                                        ;     Child Loop BB0_16 Depth 2
                                        ;       Child Loop BB0_18 Depth 3
	ldp	x14, x9, [sp, #40]
	ldr	x21, [x9, #16]
	add	x10, x9, #24
	str	x10, [sp, #48]
	ldr	x9, [sp, #64]
	mov	x11, x9
	ldrb	w12, [x11, #72]!
	cmp	w12, #1
	ccmp	x11, x10, #0, eq
	mov	x11, x14
	b.ne	LBB0_11
; %bb.4:                                ;   in Loop: Header=BB0_3 Depth=1
	add	x11, x14, #1
	str	x11, [sp, #40]
	ldr	x10, [x9, #40]
	cmp	x11, x10
	b.eq	LBB0_11
; %bb.5:                                ;   in Loop: Header=BB0_3 Depth=1
	add	x10, x9, #48
	ldr	x12, [sp, #32]
	ldr	x13, [x12, #8]
	ldr	x15, [x13]
	add	x15, x15, #1
	str	x15, [x13]
	mul	x17, x11, x11
	ldp	x15, x16, [x12, #16]
	madd	x17, x15, x15, x17
	mul	x0, x16, x16
	cmp	x17, x0
	b.eq	LBB0_10
; %bb.6:                                ;   in Loop: Header=BB0_3 Depth=1
	add	x11, x14, #2
LBB0_7:                                 ;   Parent Loop BB0_3 Depth=1
                                        ; =>  This Inner Loop Header: Depth=2
	stp	q1, q1, [x10]
	ldr	x14, [x9, #40]
	cmp	x11, x14
	b.eq	LBB0_26
; %bb.8:                                ;   in Loop: Header=BB0_7 Depth=2
	ldr	x14, [x13]
	add	x14, x14, #1
	str	x14, [x13]
	mul	x14, x11, x11
	ldp	x15, x16, [x12, #16]
	madd	x14, x15, x15, x14
	mul	x17, x16, x16
	add	x11, x11, #1
	cmp	x14, x17
	b.ne	LBB0_7
; %bb.9:                                ;   in Loop: Header=BB0_3 Depth=1
	strb	w20, [x9, #80]
	sub	x11, x11, #1
LBB0_10:                                ;   in Loop: Header=BB0_3 Depth=1
	stp	x15, x11, [x9, #48]
	str	x16, [x9, #64]
	strb	w20, [x9, #72]
	strb	w20, [x9, #80]
	stp	x11, x10, [sp, #40]
	strb	w20, [sp, #56]
LBB0_11:                                ;   in Loop: Header=BB0_3 Depth=1
	ldr	x10, [sp, #80]
	ldr	x9, [sp, #24]
	ldr	x12, [x10, #80]
	cmp	x11, x12
	b.ne	LBB0_24
LBB0_12:                                ;   in Loop: Header=BB0_3 Depth=1
	add	x9, x9, #1
	str	x9, [sp, #24]
	ldr	x11, [x10, #32]
	cmp	x9, x11
	b.eq	LBB0_24
; %bb.13:                               ;   in Loop: Header=BB0_3 Depth=1
	add	x11, x10, #40
	ldr	x12, [sp, #16]
	add	x13, x10, #88
                                        ; implicit-def: $x15
	b	LBB0_16
LBB0_14:                                ;   in Loop: Header=BB0_16 Depth=2
	mov	w16, #0                         ; =0x0
	and	x15, x15, #0xffffffffffffff00
	mov	x14, x9
LBB0_15:                                ;   in Loop: Header=BB0_16 Depth=2
	add	x9, x9, #1
	ldr	x17, [x10, #32]
	cmp	x9, x17
	b.eq	LBB0_23
LBB0_16:                                ;   Parent Loop BB0_3 Depth=1
                                        ; =>  This Loop Header: Depth=2
                                        ;       Child Loop BB0_18 Depth 3
	ldp	x16, x14, [x12, #8]
	ldr	q0, [x12]
	stur	q0, [x10, #40]
	stp	x9, x14, [x10, #56]
	stp	x9, x14, [x10, #72]
	strb	wzr, [x10, #88]
	cmp	x9, x14
	b.eq	LBB0_14
; %bb.17:                               ;   in Loop: Header=BB0_16 Depth=2
	ldr	x14, [x16]
	add	x15, x14, #1
	mov	x14, x9
LBB0_18:                                ;   Parent Loop BB0_3 Depth=1
                                        ;     Parent Loop BB0_16 Depth=2
                                        ; =>    This Inner Loop Header: Depth=3
	str	x15, [x16]
	mul	x1, x14, x14
	ldp	x17, x0, [x10, #56]
	madd	x1, x17, x17, x1
	mul	x2, x0, x0
	cmp	x1, x2
	b.eq	LBB0_21
; %bb.19:                               ;   in Loop: Header=BB0_18 Depth=3
	stp	q1, q1, [x13]
	add	x14, x14, #1
	ldr	x17, [x10, #80]
	add	x15, x15, #1
	cmp	x14, x17
	b.ne	LBB0_18
; %bb.20:                               ;   in Loop: Header=BB0_16 Depth=2
	mov	x15, #0                         ; =0x0
	mov	w16, #1                         ; =0x1
	b	LBB0_15
LBB0_21:                                ;   in Loop: Header=BB0_16 Depth=2
	stp	x17, x14, [x10, #88]
	str	x0, [x10, #104]
	mov	w16, #1                         ; =0x1
	strb	w16, [x10, #112]
	ldr	x17, [x10, #80]
	mov	x15, x13
	cmp	x17, x14
	b.eq	LBB0_15
; %bb.22:                               ;   in Loop: Header=BB0_3 Depth=1
	mov	x15, x13
LBB0_23:                                ;   in Loop: Header=BB0_3 Depth=1
	strb	w16, [x10, #120]
	strb	w20, [x10, #128]
	strb	w20, [sp, #72]
	stp	x14, x15, [sp, #40]
	strb	w16, [sp, #56]
	str	x11, [sp, #64]
	stp	x9, x11, [sp, #24]
LBB0_24:                                ;   in Loop: Header=BB0_3 Depth=1
	ldr	x10, [sp, #96]
	ldr	x10, [x10, #64]
	cmp	x9, x10
	b.ne	LBB0_2
; %bb.25:                               ;   in Loop: Header=BB0_3 Depth=1
	ldr	x8, [sp, #8]
	add	x8, x8, #1
	str	x8, [sp, #8]
	mov	x0, sp
	bl	__ZNSt3__16ranges9join_viewINS0_14transform_viewB10llvm18_nuaINS0_9iota_viewIxNS_22unreachable_sentinel_tEEEZN10playground26get_right_triangle_triplesEvE3$_0EEE10__iteratorILb0EE9__satisfyB8ne200100Ev
	movi.2d	v1, #0000000000000000
	ldr	x8, [sp, #104]
	b	LBB0_2
LBB0_26:                                ;   in Loop: Header=BB0_3 Depth=1
	strb	w20, [x9, #80]
	strb	w20, [sp, #56]
	stp	x11, xzr, [sp, #40]
	ldr	x10, [sp, #80]
	ldr	x9, [sp, #24]
	ldr	x12, [x10, #80]
	cmp	x11, x12
	b.eq	LBB0_12
	b	LBB0_24
LBB0_27:
Lloh0:
	adrp	x0, __ZNSt3__14coutE@GOTPAGE
Lloh1:
	ldr	x0, [x0, __ZNSt3__14coutE@GOTPAGEOFF]
Lloh2:
	adrp	x1, l_.str@PAGE
Lloh3:
	add	x1, x1, l_.str@PAGEOFF
	mov	w2, #10                         ; =0xa
	bl	__ZNSt3__124__put_character_sequenceB8ne200100IcNS_11char_traitsIcEEEERNS_13basic_ostreamIT_T0_EES7_PKS4_m
	ldur	x1, [x29, #-144]
	bl	__ZNSt3__113basic_ostreamIcNS_11char_traitsIcEEElsEx
Lloh4:
	adrp	x1, l_.str.1@PAGE
Lloh5:
	add	x1, x1, l_.str.1@PAGEOFF
	mov	w2, #6                          ; =0x6
	bl	__ZNSt3__124__put_character_sequenceB8ne200100IcNS_11char_traitsIcEEEERNS_13basic_ostreamIT_T0_EES7_PKS4_m
	mov	x1, x19
	bl	__ZNSt3__113basic_ostreamIcNS_11char_traitsIcEEElsEx
	mov	x19, x0
	ldr	x8, [x0]
	ldur	x9, [x8, #-24]
	add	x8, sp, #112
	add	x0, x0, x9
	bl	__ZNKSt3__18ios_base6getlocEv
Ltmp0:
Lloh6:
	adrp	x1, __ZNSt3__15ctypeIcE2idE@GOTPAGE
Lloh7:
	ldr	x1, [x1, __ZNSt3__15ctypeIcE2idE@GOTPAGEOFF]
	add	x0, sp, #112
	bl	__ZNKSt3__16locale9use_facetERNS0_2idE
Ltmp1:
; %bb.28:
	ldr	x8, [x0]
	ldr	x8, [x8, #56]
Ltmp2:
	mov	w1, #10                         ; =0xa
	blr	x8
Ltmp3:
; %bb.29:
	mov	x20, x0
	add	x0, sp, #112
	bl	__ZNSt3__16localeD1Ev
	mov	x0, x19
	mov	x1, x20
	bl	__ZNSt3__113basic_ostreamIcNS_11char_traitsIcEEE3putEc
	mov	x0, x19
	bl	__ZNSt3__113basic_ostreamIcNS_11char_traitsIcEEE5flushEv
	ldp	x29, x30, [sp, #448]            ; 16-byte Folded Reload
	ldp	x20, x19, [sp, #432]            ; 16-byte Folded Reload
	ldp	x22, x21, [sp, #416]            ; 16-byte Folded Reload
	add	sp, sp, #464
	ret
LBB0_30:
Ltmp4:
	mov	x19, x0
	add	x0, sp, #112
	bl	__ZNSt3__16localeD1Ev
	mov	x0, x19
	bl	__Unwind_Resume
	.loh AdrpLdrGot	Lloh6, Lloh7
	.loh AdrpAdd	Lloh4, Lloh5
	.loh AdrpAdd	Lloh2, Lloh3
	.loh AdrpLdrGot	Lloh0, Lloh1
Lfunc_end0:
	.cfi_endproc
	.section	__TEXT,__gcc_except_tab
	.p2align	2, 0x0
GCC_except_table0:
Lexception0:
	.byte	255                             ; @LPStart Encoding = omit
	.byte	255                             ; @TType Encoding = omit
	.byte	1                               ; Call site Encoding = uleb128
	.uleb128 Lcst_end0-Lcst_begin0
Lcst_begin0:
	.uleb128 Lfunc_begin0-Lfunc_begin0      ; >> Call Site 1 <<
	.uleb128 Ltmp0-Lfunc_begin0             ;   Call between Lfunc_begin0 and Ltmp0
	.byte	0                               ;     has no landing pad
	.byte	0                               ;   On action: cleanup
	.uleb128 Ltmp0-Lfunc_begin0             ; >> Call Site 2 <<
	.uleb128 Ltmp3-Ltmp0                    ;   Call between Ltmp0 and Ltmp3
	.uleb128 Ltmp4-Lfunc_begin0             ;     jumps to Ltmp4
	.byte	0                               ;   On action: cleanup
	.uleb128 Ltmp3-Lfunc_begin0             ; >> Call Site 3 <<
	.uleb128 Lfunc_end0-Ltmp3               ;   Call between Ltmp3 and Lfunc_end0
	.byte	0                               ;     has no landing pad
	.byte	0                               ;   On action: cleanup
Lcst_end0:
	.p2align	2, 0x0
                                        ; -- End function
	.section	__TEXT,__text,regular,pure_instructions
	.private_extern	___clang_call_terminate ; -- Begin function __clang_call_terminate
	.globl	___clang_call_terminate
	.weak_def_can_be_hidden	___clang_call_terminate
	.p2align	2
___clang_call_terminate:                ; @__clang_call_terminate
	.cfi_startproc
; %bb.0:
	stp	x29, x30, [sp, #-16]!           ; 16-byte Folded Spill
	mov	x29, sp
	.cfi_def_cfa w29, 16
	.cfi_offset w30, -8
	.cfi_offset w29, -16
	bl	___cxa_begin_catch
	bl	__ZSt9terminatev
	.cfi_endproc
                                        ; -- End function
	.p2align	2                               ; -- Begin function _ZNSt3__16ranges9join_viewINS0_14transform_viewB10llvm18_nuaINS0_9iota_viewIxNS_22unreachable_sentinel_tEEEZN10playground26get_right_triangle_triplesEvE3$_0EEE10__iteratorILb0EE9__satisfyB8ne200100Ev
__ZNSt3__16ranges9join_viewINS0_14transform_viewB10llvm18_nuaINS0_9iota_viewIxNS_22unreachable_sentinel_tEEEZN10playground26get_right_triangle_triplesEvE3$_0EEE10__iteratorILb0EE9__satisfyB8ne200100Ev: ; @"_ZNSt3__16ranges9join_viewINS0_14transform_viewB10llvm18_nuaINS0_9iota_viewIxNS_22unreachable_sentinel_tEEEZN10playground26get_right_triangle_triplesEvE3$_0EEE10__iteratorILb0EE9__satisfyB8ne200100Ev"
	.cfi_startproc
; %bb.0:
	mov	x2, #0                          ; =0x0
	ldr	x14, [x0, #8]
	mov	w8, #1                          ; =0x1
	movi.2d	v0, #0000000000000000
                                        ; implicit-def: $x9
                                        ; implicit-def: $w15
                                        ; implicit-def: $x13
                                        ; implicit-def: $x11
LBB2_1:                                 ; =>This Loop Header: Depth=1
                                        ;     Child Loop BB2_6 Depth 2
                                        ;       Child Loop BB2_8 Depth 3
	ldr	x10, [x0, #96]
	ldr	x12, [x0]
	ldp	x17, x1, [x12]
	mov	x12, x10
	str	x17, [x12, #32]!
	stp	x1, x14, [x10, #40]
	stp	x8, x14, [x10, #56]
	mov	x16, x10
	strb	wzr, [x16, #72]!
	strb	wzr, [x10, #160]
	strb	w8, [x10, #168]
	cmp	x14, #1
	b.ne	LBB2_3
; %bb.2:                                ;   in Loop: Header=BB2_1 Depth=1
	mov	w17, #0                         ; =0x0
	mov	x16, #0                         ; =0x0
	b	LBB2_14
LBB2_3:                                 ;   in Loop: Header=BB2_1 Depth=1
	mov	x9, x16
	add	x2, x10, #120
	mov	w14, #1                         ; =0x1
                                        ; implicit-def: $x13
	b	LBB2_6
LBB2_4:                                 ;   in Loop: Header=BB2_6 Depth=2
	mov	w15, #0                         ; =0x0
	and	x13, x13, #0xffffffffffffff00
	mov	x11, x14
LBB2_5:                                 ;   in Loop: Header=BB2_6 Depth=2
	add	x14, x14, #1
	ldr	x3, [x10, #64]
	cmp	x14, x3
	b.eq	LBB2_13
LBB2_6:                                 ;   Parent Loop BB2_1 Depth=1
                                        ; =>  This Loop Header: Depth=2
                                        ;       Child Loop BB2_8 Depth 3
	ldr	x11, [x10, #48]
	stp	x17, x1, [x10, #72]
	stp	x14, x11, [x10, #88]
	stp	x14, x11, [x10, #104]
	strb	wzr, [x10, #120]
	cmp	x14, x11
	b.eq	LBB2_4
; %bb.7:                                ;   in Loop: Header=BB2_6 Depth=2
	ldr	x11, [x1]
	add	x13, x11, #1
	mov	x11, x14
LBB2_8:                                 ;   Parent Loop BB2_1 Depth=1
                                        ;     Parent Loop BB2_6 Depth=2
                                        ; =>    This Inner Loop Header: Depth=3
	str	x13, [x1]
	mul	x4, x11, x11
	ldp	x15, x3, [x10, #88]
	madd	x4, x15, x15, x4
	mul	x5, x3, x3
	cmp	x4, x5
	b.eq	LBB2_11
; %bb.9:                                ;   in Loop: Header=BB2_8 Depth=3
	stp	q0, q0, [x2]
	add	x11, x11, #1
	ldr	x15, [x10, #112]
	add	x13, x13, #1
	cmp	x11, x15
	b.ne	LBB2_8
; %bb.10:                               ;   in Loop: Header=BB2_6 Depth=2
	mov	x13, #0                         ; =0x0
	mov	w15, #1                         ; =0x1
	b	LBB2_5
LBB2_11:                                ;   in Loop: Header=BB2_6 Depth=2
	stp	x15, x11, [x10, #120]
	str	x3, [x10, #136]
	mov	w15, #1                         ; =0x1
	strb	w15, [x10, #144]
	ldr	x3, [x10, #112]
	mov	x13, x2
	cmp	x3, x11
	b.eq	LBB2_5
; %bb.12:                               ;   in Loop: Header=BB2_1 Depth=1
	mov	x13, x2
LBB2_13:                                ;   in Loop: Header=BB2_1 Depth=1
	strb	w15, [x10, #152]
	mov	w17, #1                         ; =0x1
	strb	w17, [x10, #160]
	lsr	x2, x16, #8
	and	x16, x16, #0xff
LBB2_14:                                ;   in Loop: Header=BB2_1 Depth=1
	ldrb	w1, [x0, #88]
	stp	x12, x14, [x0, #16]
	orr	x16, x16, x2, lsl #8
	stp	x16, x11, [x0, #32]
	str	x13, [x0, #48]
	strb	w15, [x0, #56]
	str	x9, [x0, #64]
	strb	w17, [x0, #72]
	str	x12, [x0, #80]
	tbnz	w1, #0, LBB2_16
; %bb.15:                               ;   in Loop: Header=BB2_1 Depth=1
	strb	w8, [x0, #88]
LBB2_16:                                ;   in Loop: Header=BB2_1 Depth=1
	ldr	x10, [x10, #64]
	cmp	x14, x10
	b.ne	LBB2_18
; %bb.17:                               ;   in Loop: Header=BB2_1 Depth=1
	ldr	x10, [x0, #8]
	add	x14, x10, #1
	str	x14, [x0, #8]
	b	LBB2_1
LBB2_18:
	ret
	.cfi_endproc
                                        ; -- End function
	.private_extern	__ZNSt3__124__put_character_sequenceB8ne200100IcNS_11char_traitsIcEEEERNS_13basic_ostreamIT_T0_EES7_PKS4_m ; -- Begin function _ZNSt3__124__put_character_sequenceB8ne200100IcNS_11char_traitsIcEEEERNS_13basic_ostreamIT_T0_EES7_PKS4_m
	.globl	__ZNSt3__124__put_character_sequenceB8ne200100IcNS_11char_traitsIcEEEERNS_13basic_ostreamIT_T0_EES7_PKS4_m
	.weak_def_can_be_hidden	__ZNSt3__124__put_character_sequenceB8ne200100IcNS_11char_traitsIcEEEERNS_13basic_ostreamIT_T0_EES7_PKS4_m
	.p2align	2
__ZNSt3__124__put_character_sequenceB8ne200100IcNS_11char_traitsIcEEEERNS_13basic_ostreamIT_T0_EES7_PKS4_m: ; @_ZNSt3__124__put_character_sequenceB8ne200100IcNS_11char_traitsIcEEEERNS_13basic_ostreamIT_T0_EES7_PKS4_m
Lfunc_begin1:
	.cfi_startproc
	.cfi_personality 155, ___gxx_personality_v0
	.cfi_lsda 16, Lexception1
; %bb.0:
	sub	sp, sp, #112
	stp	x26, x25, [sp, #32]             ; 16-byte Folded Spill
	stp	x24, x23, [sp, #48]             ; 16-byte Folded Spill
	stp	x22, x21, [sp, #64]             ; 16-byte Folded Spill
	stp	x20, x19, [sp, #80]             ; 16-byte Folded Spill
	stp	x29, x30, [sp, #96]             ; 16-byte Folded Spill
	add	x29, sp, #96
	.cfi_def_cfa w29, 16
	.cfi_offset w30, -8
	.cfi_offset w29, -16
	.cfi_offset w19, -24
	.cfi_offset w20, -32
	.cfi_offset w21, -40
	.cfi_offset w22, -48
	.cfi_offset w23, -56
	.cfi_offset w24, -64
	.cfi_offset w25, -72
	.cfi_offset w26, -80
	mov	x21, x2
	mov	x20, x1
	mov	x19, x0
Ltmp5:
	add	x0, sp, #8
	mov	x1, x19
	bl	__ZNSt3__113basic_ostreamIcNS_11char_traitsIcEEE6sentryC1ERS3_
Ltmp6:
; %bb.1:
	ldrb	w8, [sp, #8]
	cmp	w8, #1
	b.ne	LBB3_10
; %bb.2:
	ldr	x8, [x19]
	ldur	x8, [x8, #-24]
	add	x4, x19, x8
	ldr	x22, [x4, #40]
	ldr	w24, [x4, #8]
	ldr	w23, [x4, #144]
	cmn	w23, #1
	b.ne	LBB3_7
; %bb.3:
Ltmp8:
	add	x8, sp, #24
	mov	x25, x4
	mov	x0, x4
	bl	__ZNKSt3__18ios_base6getlocEv
Ltmp9:
; %bb.4:
Ltmp10:
Lloh8:
	adrp	x1, __ZNSt3__15ctypeIcE2idE@GOTPAGE
Lloh9:
	ldr	x1, [x1, __ZNSt3__15ctypeIcE2idE@GOTPAGEOFF]
	add	x0, sp, #24
	bl	__ZNKSt3__16locale9use_facetERNS0_2idE
Ltmp11:
; %bb.5:
	ldr	x8, [x0]
	ldr	x8, [x8, #56]
Ltmp12:
	mov	w1, #32                         ; =0x20
	blr	x8
Ltmp13:
; %bb.6:
	mov	x23, x0
	add	x0, sp, #24
	bl	__ZNSt3__16localeD1Ev
	mov	x4, x25
	str	w23, [x25, #144]
LBB3_7:
	mov	w8, #176                        ; =0xb0
	and	w8, w24, w8
	add	x3, x20, x21
	cmp	w8, #32
	csel	x2, x3, x20, eq
Ltmp15:
	sxtb	w5, w23
	mov	x0, x22
	mov	x1, x20
	bl	__ZNSt3__116__pad_and_outputB8ne200100IcNS_11char_traitsIcEEEENS_19ostreambuf_iteratorIT_T0_EES6_PKS4_S8_S8_RNS_8ios_baseES4_
Ltmp16:
; %bb.8:
	cbnz	x0, LBB3_10
; %bb.9:
	ldr	x8, [x19]
	ldur	x8, [x8, #-24]
	add	x0, x19, x8
	ldr	w8, [x0, #32]
	mov	w9, #5                          ; =0x5
Ltmp18:
	orr	w1, w8, w9
	bl	__ZNSt3__18ios_base5clearEj
Ltmp19:
LBB3_10:
	add	x0, sp, #8
	bl	__ZNSt3__113basic_ostreamIcNS_11char_traitsIcEEE6sentryD1Ev
LBB3_11:
	mov	x0, x19
	ldp	x29, x30, [sp, #96]             ; 16-byte Folded Reload
	ldp	x20, x19, [sp, #80]             ; 16-byte Folded Reload
	ldp	x22, x21, [sp, #64]             ; 16-byte Folded Reload
	ldp	x24, x23, [sp, #48]             ; 16-byte Folded Reload
	ldp	x26, x25, [sp, #32]             ; 16-byte Folded Reload
	add	sp, sp, #112
	ret
LBB3_12:
Ltmp20:
	b	LBB3_15
LBB3_13:
Ltmp14:
	mov	x20, x0
	add	x0, sp, #24
	bl	__ZNSt3__16localeD1Ev
	b	LBB3_16
LBB3_14:
Ltmp17:
LBB3_15:
	mov	x20, x0
LBB3_16:
	add	x0, sp, #8
	bl	__ZNSt3__113basic_ostreamIcNS_11char_traitsIcEEE6sentryD1Ev
	b	LBB3_18
LBB3_17:
Ltmp7:
	mov	x20, x0
LBB3_18:
	mov	x0, x20
	bl	___cxa_begin_catch
	ldr	x8, [x19]
	ldur	x8, [x8, #-24]
Ltmp21:
	add	x0, x19, x8
	bl	__ZNSt3__18ios_base33__set_badbit_and_consider_rethrowEv
Ltmp22:
; %bb.19:
	bl	___cxa_end_catch
	b	LBB3_11
LBB3_20:
Ltmp23:
	mov	x19, x0
Ltmp24:
	bl	___cxa_end_catch
Ltmp25:
; %bb.21:
	mov	x0, x19
	bl	__Unwind_Resume
LBB3_22:
Ltmp26:
	bl	___clang_call_terminate
	.loh AdrpLdrGot	Lloh8, Lloh9
Lfunc_end1:
	.cfi_endproc
	.section	__TEXT,__gcc_except_tab
	.p2align	2, 0x0
GCC_except_table3:
Lexception1:
	.byte	255                             ; @LPStart Encoding = omit
	.byte	155                             ; @TType Encoding = indirect pcrel sdata4
	.uleb128 Lttbase0-Lttbaseref0
Lttbaseref0:
	.byte	1                               ; Call site Encoding = uleb128
	.uleb128 Lcst_end1-Lcst_begin1
Lcst_begin1:
	.uleb128 Ltmp5-Lfunc_begin1             ; >> Call Site 1 <<
	.uleb128 Ltmp6-Ltmp5                    ;   Call between Ltmp5 and Ltmp6
	.uleb128 Ltmp7-Lfunc_begin1             ;     jumps to Ltmp7
	.byte	1                               ;   On action: 1
	.uleb128 Ltmp8-Lfunc_begin1             ; >> Call Site 2 <<
	.uleb128 Ltmp9-Ltmp8                    ;   Call between Ltmp8 and Ltmp9
	.uleb128 Ltmp17-Lfunc_begin1            ;     jumps to Ltmp17
	.byte	1                               ;   On action: 1
	.uleb128 Ltmp10-Lfunc_begin1            ; >> Call Site 3 <<
	.uleb128 Ltmp13-Ltmp10                  ;   Call between Ltmp10 and Ltmp13
	.uleb128 Ltmp14-Lfunc_begin1            ;     jumps to Ltmp14
	.byte	1                               ;   On action: 1
	.uleb128 Ltmp15-Lfunc_begin1            ; >> Call Site 4 <<
	.uleb128 Ltmp16-Ltmp15                  ;   Call between Ltmp15 and Ltmp16
	.uleb128 Ltmp17-Lfunc_begin1            ;     jumps to Ltmp17
	.byte	1                               ;   On action: 1
	.uleb128 Ltmp18-Lfunc_begin1            ; >> Call Site 5 <<
	.uleb128 Ltmp19-Ltmp18                  ;   Call between Ltmp18 and Ltmp19
	.uleb128 Ltmp20-Lfunc_begin1            ;     jumps to Ltmp20
	.byte	1                               ;   On action: 1
	.uleb128 Ltmp19-Lfunc_begin1            ; >> Call Site 6 <<
	.uleb128 Ltmp21-Ltmp19                  ;   Call between Ltmp19 and Ltmp21
	.byte	0                               ;     has no landing pad
	.byte	0                               ;   On action: cleanup
	.uleb128 Ltmp21-Lfunc_begin1            ; >> Call Site 7 <<
	.uleb128 Ltmp22-Ltmp21                  ;   Call between Ltmp21 and Ltmp22
	.uleb128 Ltmp23-Lfunc_begin1            ;     jumps to Ltmp23
	.byte	0                               ;   On action: cleanup
	.uleb128 Ltmp22-Lfunc_begin1            ; >> Call Site 8 <<
	.uleb128 Ltmp24-Ltmp22                  ;   Call between Ltmp22 and Ltmp24
	.byte	0                               ;     has no landing pad
	.byte	0                               ;   On action: cleanup
	.uleb128 Ltmp24-Lfunc_begin1            ; >> Call Site 9 <<
	.uleb128 Ltmp25-Ltmp24                  ;   Call between Ltmp24 and Ltmp25
	.uleb128 Ltmp26-Lfunc_begin1            ;     jumps to Ltmp26
	.byte	1                               ;   On action: 1
	.uleb128 Ltmp25-Lfunc_begin1            ; >> Call Site 10 <<
	.uleb128 Lfunc_end1-Ltmp25              ;   Call between Ltmp25 and Lfunc_end1
	.byte	0                               ;     has no landing pad
	.byte	0                               ;   On action: cleanup
Lcst_end1:
	.byte	1                               ; >> Action Record 1 <<
                                        ;   Catch TypeInfo 1
	.byte	0                               ;   No further actions
	.p2align	2, 0x0
                                        ; >> Catch TypeInfos <<
	.long	0                               ; TypeInfo 1
Lttbase0:
	.p2align	2, 0x0
                                        ; -- End function
	.section	__TEXT,__text,regular,pure_instructions
	.private_extern	__ZNSt3__116__pad_and_outputB8ne200100IcNS_11char_traitsIcEEEENS_19ostreambuf_iteratorIT_T0_EES6_PKS4_S8_S8_RNS_8ios_baseES4_ ; -- Begin function _ZNSt3__116__pad_and_outputB8ne200100IcNS_11char_traitsIcEEEENS_19ostreambuf_iteratorIT_T0_EES6_PKS4_S8_S8_RNS_8ios_baseES4_
	.globl	__ZNSt3__116__pad_and_outputB8ne200100IcNS_11char_traitsIcEEEENS_19ostreambuf_iteratorIT_T0_EES6_PKS4_S8_S8_RNS_8ios_baseES4_
	.weak_def_can_be_hidden	__ZNSt3__116__pad_and_outputB8ne200100IcNS_11char_traitsIcEEEENS_19ostreambuf_iteratorIT_T0_EES6_PKS4_S8_S8_RNS_8ios_baseES4_
	.p2align	2
__ZNSt3__116__pad_and_outputB8ne200100IcNS_11char_traitsIcEEEENS_19ostreambuf_iteratorIT_T0_EES6_PKS4_S8_S8_RNS_8ios_baseES4_: ; @_ZNSt3__116__pad_and_outputB8ne200100IcNS_11char_traitsIcEEEENS_19ostreambuf_iteratorIT_T0_EES6_PKS4_S8_S8_RNS_8ios_baseES4_
Lfunc_begin2:
	.cfi_startproc
	.cfi_personality 155, ___gxx_personality_v0
	.cfi_lsda 16, Lexception2
; %bb.0:
	sub	sp, sp, #112
	stp	x26, x25, [sp, #32]             ; 16-byte Folded Spill
	stp	x24, x23, [sp, #48]             ; 16-byte Folded Spill
	stp	x22, x21, [sp, #64]             ; 16-byte Folded Spill
	stp	x20, x19, [sp, #80]             ; 16-byte Folded Spill
	stp	x29, x30, [sp, #96]             ; 16-byte Folded Spill
	add	x29, sp, #96
	.cfi_def_cfa w29, 16
	.cfi_offset w30, -8
	.cfi_offset w29, -16
	.cfi_offset w19, -24
	.cfi_offset w20, -32
	.cfi_offset w21, -40
	.cfi_offset w22, -48
	.cfi_offset w23, -56
	.cfi_offset w24, -64
	.cfi_offset w25, -72
	.cfi_offset w26, -80
	mov	x19, x0
	cbz	x0, LBB4_16
; %bb.1:
	mov	x24, x5
	mov	x20, x4
	mov	x22, x3
	mov	x21, x2
	ldr	x8, [x4, #24]
	sub	x9, x3, x1
	subs	x8, x8, x9
	csel	x23, x8, xzr, gt
	sub	x25, x2, x1
	cmp	x25, #1
	b.lt	LBB4_3
; %bb.2:
	ldr	x8, [x19]
	ldr	x8, [x8, #96]
	mov	x0, x19
	mov	x2, x25
	blr	x8
	cmp	x0, x25
	b.ne	LBB4_15
LBB4_3:
	cmp	x23, #1
	b.lt	LBB4_12
; %bb.4:
	mov	x8, #9223372036854775800        ; =0x7ffffffffffffff8
	cmp	x23, x8
	b.hs	LBB4_17
; %bb.5:
	cmp	x23, #23
	b.hs	LBB4_7
; %bb.6:
	strb	w23, [sp, #31]
	add	x25, sp, #8
	b	LBB4_8
LBB4_7:
	orr	x8, x23, #0x7
	cmp	x8, #23
	mov	w9, #25                         ; =0x19
	csinc	x26, x9, x8, eq
	mov	x0, x26
	bl	__Znwm
	mov	x25, x0
	orr	x8, x26, #0x8000000000000000
	stp	x23, x8, [sp, #16]
	str	x0, [sp, #8]
LBB4_8:
	mov	x0, x25
	mov	x1, x24
	mov	x2, x23
	bl	_memset
	strb	wzr, [x25, x23]
	ldrsb	w8, [sp, #31]
	ldr	x9, [sp, #8]
	cmp	w8, #0
	add	x8, sp, #8
	csel	x1, x9, x8, lt
	ldr	x8, [x19]
	ldr	x8, [x8, #96]
Ltmp27:
	mov	x0, x19
	mov	x2, x23
	blr	x8
Ltmp28:
; %bb.9:
	ldrsb	w8, [sp, #31]
	tbnz	w8, #31, LBB4_11
; %bb.10:
	cmp	x0, x23
	b.ne	LBB4_15
	b	LBB4_12
LBB4_11:
	ldr	x8, [sp, #8]
	ldr	x9, [sp, #24]
	and	x1, x9, #0x7fffffffffffffff
	mov	x24, x0
	mov	x0, x8
	bl	__ZdlPvm
	cmp	x24, x23
	b.ne	LBB4_15
LBB4_12:
	sub	x22, x22, x21
	cmp	x22, #1
	b.lt	LBB4_14
; %bb.13:
	ldr	x8, [x19]
	ldr	x8, [x8, #96]
	mov	x0, x19
	mov	x1, x21
	mov	x2, x22
	blr	x8
	cmp	x0, x22
	b.ne	LBB4_15
LBB4_14:
	str	xzr, [x20, #24]
	b	LBB4_16
LBB4_15:
	mov	x19, #0                         ; =0x0
LBB4_16:
	mov	x0, x19
	ldp	x29, x30, [sp, #96]             ; 16-byte Folded Reload
	ldp	x20, x19, [sp, #80]             ; 16-byte Folded Reload
	ldp	x22, x21, [sp, #64]             ; 16-byte Folded Reload
	ldp	x24, x23, [sp, #48]             ; 16-byte Folded Reload
	ldp	x26, x25, [sp, #32]             ; 16-byte Folded Reload
	add	sp, sp, #112
	ret
LBB4_17:
	bl	__ZNSt3__112basic_stringIcNS_11char_traitsIcEENS_9allocatorIcEEE20__throw_length_errorB8ne200100Ev
LBB4_18:
Ltmp29:
	mov	x19, x0
	ldrsb	w8, [sp, #31]
	tbz	w8, #31, LBB4_20
; %bb.19:
	ldr	x0, [sp, #8]
	ldr	x8, [sp, #24]
	and	x1, x8, #0x7fffffffffffffff
	bl	__ZdlPvm
LBB4_20:
	mov	x0, x19
	bl	__Unwind_Resume
Lfunc_end2:
	.cfi_endproc
	.section	__TEXT,__gcc_except_tab
	.p2align	2, 0x0
GCC_except_table4:
Lexception2:
	.byte	255                             ; @LPStart Encoding = omit
	.byte	255                             ; @TType Encoding = omit
	.byte	1                               ; Call site Encoding = uleb128
	.uleb128 Lcst_end2-Lcst_begin2
Lcst_begin2:
	.uleb128 Lfunc_begin2-Lfunc_begin2      ; >> Call Site 1 <<
	.uleb128 Ltmp27-Lfunc_begin2            ;   Call between Lfunc_begin2 and Ltmp27
	.byte	0                               ;     has no landing pad
	.byte	0                               ;   On action: cleanup
	.uleb128 Ltmp27-Lfunc_begin2            ; >> Call Site 2 <<
	.uleb128 Ltmp28-Ltmp27                  ;   Call between Ltmp27 and Ltmp28
	.uleb128 Ltmp29-Lfunc_begin2            ;     jumps to Ltmp29
	.byte	0                               ;   On action: cleanup
	.uleb128 Ltmp28-Lfunc_begin2            ; >> Call Site 3 <<
	.uleb128 Lfunc_end2-Ltmp28              ;   Call between Ltmp28 and Lfunc_end2
	.byte	0                               ;     has no landing pad
	.byte	0                               ;   On action: cleanup
Lcst_end2:
	.p2align	2, 0x0
                                        ; -- End function
	.section	__TEXT,__text,regular,pure_instructions
	.private_extern	__ZNSt3__112basic_stringIcNS_11char_traitsIcEENS_9allocatorIcEEE20__throw_length_errorB8ne200100Ev ; -- Begin function _ZNSt3__112basic_stringIcNS_11char_traitsIcEENS_9allocatorIcEEE20__throw_length_errorB8ne200100Ev
	.globl	__ZNSt3__112basic_stringIcNS_11char_traitsIcEENS_9allocatorIcEEE20__throw_length_errorB8ne200100Ev
	.weak_def_can_be_hidden	__ZNSt3__112basic_stringIcNS_11char_traitsIcEENS_9allocatorIcEEE20__throw_length_errorB8ne200100Ev
	.p2align	2
__ZNSt3__112basic_stringIcNS_11char_traitsIcEENS_9allocatorIcEEE20__throw_length_errorB8ne200100Ev: ; @_ZNSt3__112basic_stringIcNS_11char_traitsIcEENS_9allocatorIcEEE20__throw_length_errorB8ne200100Ev
	.cfi_startproc
; %bb.0:
	stp	x29, x30, [sp, #-16]!           ; 16-byte Folded Spill
	mov	x29, sp
	.cfi_def_cfa w29, 16
	.cfi_offset w30, -8
	.cfi_offset w29, -16
Lloh10:
	adrp	x0, l_.str.2@PAGE
Lloh11:
	add	x0, x0, l_.str.2@PAGEOFF
	bl	__ZNSt3__120__throw_length_errorB8ne200100EPKc
	.loh AdrpAdd	Lloh10, Lloh11
	.cfi_endproc
                                        ; -- End function
	.private_extern	__ZNSt3__120__throw_length_errorB8ne200100EPKc ; -- Begin function _ZNSt3__120__throw_length_errorB8ne200100EPKc
	.globl	__ZNSt3__120__throw_length_errorB8ne200100EPKc
	.weak_def_can_be_hidden	__ZNSt3__120__throw_length_errorB8ne200100EPKc
	.p2align	2
__ZNSt3__120__throw_length_errorB8ne200100EPKc: ; @_ZNSt3__120__throw_length_errorB8ne200100EPKc
Lfunc_begin3:
	.cfi_startproc
	.cfi_personality 155, ___gxx_personality_v0
	.cfi_lsda 16, Lexception3
; %bb.0:
	stp	x20, x19, [sp, #-32]!           ; 16-byte Folded Spill
	stp	x29, x30, [sp, #16]             ; 16-byte Folded Spill
	add	x29, sp, #16
	.cfi_def_cfa w29, 16
	.cfi_offset w30, -8
	.cfi_offset w29, -16
	.cfi_offset w19, -24
	.cfi_offset w20, -32
	mov	x20, x0
	mov	w0, #16                         ; =0x10
	bl	___cxa_allocate_exception
	mov	x19, x0
Ltmp30:
	mov	x1, x20
	bl	__ZNSt12length_errorC1B8ne200100EPKc
Ltmp31:
; %bb.1:
Lloh12:
	adrp	x1, __ZTISt12length_error@GOTPAGE
Lloh13:
	ldr	x1, [x1, __ZTISt12length_error@GOTPAGEOFF]
Lloh14:
	adrp	x2, __ZNSt12length_errorD1Ev@GOTPAGE
Lloh15:
	ldr	x2, [x2, __ZNSt12length_errorD1Ev@GOTPAGEOFF]
	mov	x0, x19
	bl	___cxa_throw
LBB6_2:
Ltmp32:
	mov	x20, x0
	mov	x0, x19
	bl	___cxa_free_exception
	mov	x0, x20
	bl	__Unwind_Resume
	.loh AdrpLdrGot	Lloh14, Lloh15
	.loh AdrpLdrGot	Lloh12, Lloh13
Lfunc_end3:
	.cfi_endproc
	.section	__TEXT,__gcc_except_tab
	.p2align	2, 0x0
GCC_except_table6:
Lexception3:
	.byte	255                             ; @LPStart Encoding = omit
	.byte	255                             ; @TType Encoding = omit
	.byte	1                               ; Call site Encoding = uleb128
	.uleb128 Lcst_end3-Lcst_begin3
Lcst_begin3:
	.uleb128 Lfunc_begin3-Lfunc_begin3      ; >> Call Site 1 <<
	.uleb128 Ltmp30-Lfunc_begin3            ;   Call between Lfunc_begin3 and Ltmp30
	.byte	0                               ;     has no landing pad
	.byte	0                               ;   On action: cleanup
	.uleb128 Ltmp30-Lfunc_begin3            ; >> Call Site 2 <<
	.uleb128 Ltmp31-Ltmp30                  ;   Call between Ltmp30 and Ltmp31
	.uleb128 Ltmp32-Lfunc_begin3            ;     jumps to Ltmp32
	.byte	0                               ;   On action: cleanup
	.uleb128 Ltmp31-Lfunc_begin3            ; >> Call Site 3 <<
	.uleb128 Lfunc_end3-Ltmp31              ;   Call between Ltmp31 and Lfunc_end3
	.byte	0                               ;     has no landing pad
	.byte	0                               ;   On action: cleanup
Lcst_end3:
	.p2align	2, 0x0
                                        ; -- End function
	.section	__TEXT,__text,regular,pure_instructions
	.private_extern	__ZNSt12length_errorC1B8ne200100EPKc ; -- Begin function _ZNSt12length_errorC1B8ne200100EPKc
	.globl	__ZNSt12length_errorC1B8ne200100EPKc
	.weak_def_can_be_hidden	__ZNSt12length_errorC1B8ne200100EPKc
	.p2align	2
__ZNSt12length_errorC1B8ne200100EPKc:   ; @_ZNSt12length_errorC1B8ne200100EPKc
	.cfi_startproc
; %bb.0:
	stp	x29, x30, [sp, #-16]!           ; 16-byte Folded Spill
	mov	x29, sp
	.cfi_def_cfa w29, 16
	.cfi_offset w30, -8
	.cfi_offset w29, -16
	bl	__ZNSt11logic_errorC2EPKc
Lloh16:
	adrp	x8, __ZTVSt12length_error@GOTPAGE
Lloh17:
	ldr	x8, [x8, __ZTVSt12length_error@GOTPAGEOFF]
	add	x8, x8, #16
	str	x8, [x0]
	ldp	x29, x30, [sp], #16             ; 16-byte Folded Reload
	ret
	.loh AdrpLdrGot	Lloh16, Lloh17
	.cfi_endproc
                                        ; -- End function
	.section	__TEXT,__cstring,cstring_literals
l_.str:                                 ; @.str
	.asciz	"Counter = "

l_.str.1:                               ; @.str.1
	.asciz	", s = "

l_.str.2:                               ; @.str.2
	.asciz	"basic_string"

.subsections_via_symbols
