	.text
	.file	"forwarding_micro.c"
	.file	0 "/work/muke/PND-Loads/benchmarks" "forwarding_micro.c" md5 0xac592b9a1f78a2e962d53a52c82fafbc
	.file	1 "/usr/bin/../lib/gcc-cross/aarch64-linux-gnu/12/../../../../aarch64-linux-gnu/include" "stdint.h" md5 0xe1a448af0fce22a39bc183d2915834c7
	.section	.rodata.cst8,"aM",@progbits,8
	.p2align	3                               // -- Begin function main
.LCPI0_0:
	.xword	0x4009327bb2fec56d              // double 3.1496499999999998
.LCPI0_1:
	.xword	0x4005dbd512ec6bcf              // double 2.7323400000000002
	.text
	.globl	main
	.p2align	2
	.type	main,@function
main:                                   // @main
.Lfunc_begin0:
	.loc	0 4 0                           // forwarding_micro.c:4:0
	.cfi_startproc
// %bb.0:
	//DEBUG_VALUE: main:argc <- $w0
	//DEBUG_VALUE: main:argv <- $x1
	sub	sp, sp, #48
	.cfi_def_cfa_offset 48
	stp	x29, x30, [sp, #16]             // 16-byte Folded Spill
	stp	x20, x19, [sp, #32]             // 16-byte Folded Spill
	add	x29, sp, #16
	.cfi_def_cfa w29, 32
	.cfi_offset w19, -8
	.cfi_offset w20, -16
	.cfi_offset w30, -24
	.cfi_offset w29, -32
.Ltmp0:
	.loc	0 6 18 prologue_end             // forwarding_micro.c:6:18
	ldr	x0, [x1, #8]
.Ltmp1:
	//DEBUG_VALUE: main:argc <- [DW_OP_LLVM_entry_value 1] $w0
	.loc	0 6 13 is_stmt 0                // forwarding_micro.c:6:13
	bl	atoi
.Ltmp2:
	//DEBUG_VALUE: main:argv <- [DW_OP_LLVM_entry_value 1] $x1
	mov	w19, w0
.Ltmp3:
	//DEBUG_VALUE: main:n <- $w19
	.loc	0 7 26 is_stmt 1                // forwarding_micro.c:7:26
	mov	w0, #4
	bl	malloc
	.loc	0 8 14                          // forwarding_micro.c:8:14
	lsl	w8, w19, #1
	.loc	0 7 26                          // forwarding_micro.c:7:26
	mov	x20, x0
.Ltmp4:
	//DEBUG_VALUE: main:load <- $x20
	.loc	0 8 14                          // forwarding_micro.c:8:14
	add	w9, w8, w19
	.loc	0 9 28                          // forwarding_micro.c:9:28
	sbfiz	x0, x8, #2, #32
	.loc	0 8 11                          // forwarding_micro.c:8:11
	str	w9, [x20]
	.loc	0 9 18                          // forwarding_micro.c:9:18
	bl	malloc
	.loc	0 14 33                         // forwarding_micro.c:14:33
	sub	w8, w19, #1
	adrp	x9, .LCPI0_0
	adrp	x10, .LCPI0_1
	.loc	0 15 31                         // forwarding_micro.c:15:31
	scvtf	d2, w19
	.loc	0 10 5                          // forwarding_micro.c:10:5
	//APP
	dsb	sy
	isb

	//NO_APP
	.loc	0 14 31                         // forwarding_micro.c:14:31
	scvtf	d0, w8
	ldr	d1, [x9, :lo12:.LCPI0_0]
	ldr	d3, [x10, :lo12:.LCPI0_1]
	.loc	0 14 29 is_stmt 0               // forwarding_micro.c:14:29
	fmul	d0, d0, d1
.Ltmp5:
	//DEBUG_VALUE: main:offset <- $d0
	.loc	0 15 29 is_stmt 1               // forwarding_micro.c:15:29
	fmul	d1, d2, d3
.Ltmp6:
	//DEBUG_VALUE: main:factor <- $d1
	.loc	0 16 40                         // forwarding_micro.c:16:40
	fdiv	d0, d0, d1
.Ltmp7:
	.loc	0 16 47 is_stmt 0               // forwarding_micro.c:16:47
	fdiv	d0, d0, d1
	.loc	0 16 22                         // forwarding_micro.c:16:22
	fcvtzu	x8, d0
.Ltmp8:
	//DEBUG_VALUE: main:store <- $x0
	.loc	0 16 57                         // forwarding_micro.c:16:57
	str	w19, [x0, x8, lsl #2]
	.loc	0 17 22 is_stmt 1               // forwarding_micro.c:17:22
	ldr	w8, [x20]
	.loc	0 17 18 is_stmt 0               // forwarding_micro.c:17:18
	stur	w8, [x29, #-4]
.Ltmp9:
	.loc	0 19 9 is_stmt 1                // forwarding_micro.c:19:9
	ldur	w8, [x29, #-4]
	.loc	0 19 11 is_stmt 0               // forwarding_micro.c:19:11
	sdiv	w9, w8, w19
	msub	w8, w9, w19, w8
	.loc	0 19 15                         // forwarding_micro.c:19:15
	cmp	w8, #0
	cset	w0, eq
.Ltmp10:
	.cfi_def_cfa wsp, 48
	.loc	0 22 1 is_stmt 1                // forwarding_micro.c:22:1
	ldp	x20, x19, [sp, #32]             // 16-byte Folded Reload
.Ltmp11:
	ldp	x29, x30, [sp, #16]             // 16-byte Folded Reload
	add	sp, sp, #48
	.cfi_def_cfa_offset 0
	.cfi_restore w19
	.cfi_restore w20
	.cfi_restore w30
	.cfi_restore w29
	ret
.Ltmp12:
.Lfunc_end0:
	.size	main, .Lfunc_end0-main
	.cfi_endproc
                                        // -- End function
	.section	.debug_loclists,"",@progbits
	.word	.Ldebug_list_header_end0-.Ldebug_list_header_start0 // Length
.Ldebug_list_header_start0:
	.hword	5                               // Version
	.byte	8                               // Address size
	.byte	0                               // Segment selector size
	.word	7                               // Offset entry count
.Lloclists_table_base0:
	.word	.Ldebug_loc0-.Lloclists_table_base0
	.word	.Ldebug_loc1-.Lloclists_table_base0
	.word	.Ldebug_loc2-.Lloclists_table_base0
	.word	.Ldebug_loc3-.Lloclists_table_base0
	.word	.Ldebug_loc4-.Lloclists_table_base0
	.word	.Ldebug_loc5-.Lloclists_table_base0
	.word	.Ldebug_loc6-.Lloclists_table_base0
.Ldebug_loc0:
	.byte	4                               // DW_LLE_offset_pair
	.uleb128 .Lfunc_begin0-.Lfunc_begin0    //   starting offset
	.uleb128 .Ltmp1-.Lfunc_begin0           //   ending offset
	.byte	1                               // Loc expr size
	.byte	80                              // DW_OP_reg0
	.byte	4                               // DW_LLE_offset_pair
	.uleb128 .Ltmp1-.Lfunc_begin0           //   starting offset
	.uleb128 .Lfunc_end0-.Lfunc_begin0      //   ending offset
	.byte	4                               // Loc expr size
	.byte	163                             // DW_OP_entry_value
	.byte	1                               // 1
	.byte	80                              // DW_OP_reg0
	.byte	159                             // DW_OP_stack_value
	.byte	0                               // DW_LLE_end_of_list
.Ldebug_loc1:
	.byte	4                               // DW_LLE_offset_pair
	.uleb128 .Lfunc_begin0-.Lfunc_begin0    //   starting offset
	.uleb128 .Ltmp2-.Lfunc_begin0           //   ending offset
	.byte	1                               // Loc expr size
	.byte	81                              // DW_OP_reg1
	.byte	4                               // DW_LLE_offset_pair
	.uleb128 .Ltmp2-.Lfunc_begin0           //   starting offset
	.uleb128 .Lfunc_end0-.Lfunc_begin0      //   ending offset
	.byte	4                               // Loc expr size
	.byte	163                             // DW_OP_entry_value
	.byte	1                               // 1
	.byte	81                              // DW_OP_reg1
	.byte	159                             // DW_OP_stack_value
	.byte	0                               // DW_LLE_end_of_list
.Ldebug_loc2:
	.byte	4                               // DW_LLE_offset_pair
	.uleb128 .Ltmp3-.Lfunc_begin0           //   starting offset
	.uleb128 .Ltmp11-.Lfunc_begin0          //   ending offset
	.byte	1                               // Loc expr size
	.byte	99                              // DW_OP_reg19
	.byte	0                               // DW_LLE_end_of_list
.Ldebug_loc3:
	.byte	4                               // DW_LLE_offset_pair
	.uleb128 .Ltmp4-.Lfunc_begin0           //   starting offset
	.uleb128 .Ltmp11-.Lfunc_begin0          //   ending offset
	.byte	1                               // Loc expr size
	.byte	100                             // DW_OP_reg20
	.byte	0                               // DW_LLE_end_of_list
.Ldebug_loc4:
	.byte	4                               // DW_LLE_offset_pair
	.uleb128 .Ltmp5-.Lfunc_begin0           //   starting offset
	.uleb128 .Ltmp7-.Lfunc_begin0           //   ending offset
	.byte	2                               // Loc expr size
	.byte	144                             // DW_OP_regx
	.byte	64                              // 64
	.byte	0                               // DW_LLE_end_of_list
.Ldebug_loc5:
	.byte	4                               // DW_LLE_offset_pair
	.uleb128 .Ltmp6-.Lfunc_begin0           //   starting offset
	.uleb128 .Lfunc_end0-.Lfunc_begin0      //   ending offset
	.byte	2                               // Loc expr size
	.byte	144                             // DW_OP_regx
	.byte	65                              // 65
	.byte	0                               // DW_LLE_end_of_list
.Ldebug_loc6:
	.byte	4                               // DW_LLE_offset_pair
	.uleb128 .Ltmp8-.Lfunc_begin0           //   starting offset
	.uleb128 .Ltmp10-.Lfunc_begin0          //   ending offset
	.byte	1                               // Loc expr size
	.byte	80                              // DW_OP_reg0
	.byte	0                               // DW_LLE_end_of_list
.Ldebug_list_header_end0:
	.section	.debug_abbrev,"",@progbits
	.byte	1                               // Abbreviation Code
	.byte	17                              // DW_TAG_compile_unit
	.byte	1                               // DW_CHILDREN_yes
	.byte	37                              // DW_AT_producer
	.byte	37                              // DW_FORM_strx1
	.byte	19                              // DW_AT_language
	.byte	5                               // DW_FORM_data2
	.byte	3                               // DW_AT_name
	.byte	37                              // DW_FORM_strx1
	.byte	114                             // DW_AT_str_offsets_base
	.byte	23                              // DW_FORM_sec_offset
	.byte	16                              // DW_AT_stmt_list
	.byte	23                              // DW_FORM_sec_offset
	.byte	27                              // DW_AT_comp_dir
	.byte	37                              // DW_FORM_strx1
	.byte	17                              // DW_AT_low_pc
	.byte	27                              // DW_FORM_addrx
	.byte	18                              // DW_AT_high_pc
	.byte	6                               // DW_FORM_data4
	.byte	115                             // DW_AT_addr_base
	.byte	23                              // DW_FORM_sec_offset
	.ascii	"\214\001"                      // DW_AT_loclists_base
	.byte	23                              // DW_FORM_sec_offset
	.byte	0                               // EOM(1)
	.byte	0                               // EOM(2)
	.byte	2                               // Abbreviation Code
	.byte	15                              // DW_TAG_pointer_type
	.byte	0                               // DW_CHILDREN_no
	.byte	73                              // DW_AT_type
	.byte	19                              // DW_FORM_ref4
	.byte	0                               // EOM(1)
	.byte	0                               // EOM(2)
	.byte	3                               // Abbreviation Code
	.byte	36                              // DW_TAG_base_type
	.byte	0                               // DW_CHILDREN_no
	.byte	3                               // DW_AT_name
	.byte	37                              // DW_FORM_strx1
	.byte	62                              // DW_AT_encoding
	.byte	11                              // DW_FORM_data1
	.byte	11                              // DW_AT_byte_size
	.byte	11                              // DW_FORM_data1
	.byte	0                               // EOM(1)
	.byte	0                               // EOM(2)
	.byte	4                               // Abbreviation Code
	.byte	22                              // DW_TAG_typedef
	.byte	0                               // DW_CHILDREN_no
	.byte	73                              // DW_AT_type
	.byte	19                              // DW_FORM_ref4
	.byte	3                               // DW_AT_name
	.byte	37                              // DW_FORM_strx1
	.byte	58                              // DW_AT_decl_file
	.byte	11                              // DW_FORM_data1
	.byte	59                              // DW_AT_decl_line
	.byte	11                              // DW_FORM_data1
	.byte	0                               // EOM(1)
	.byte	0                               // EOM(2)
	.byte	5                               // Abbreviation Code
	.byte	46                              // DW_TAG_subprogram
	.byte	1                               // DW_CHILDREN_yes
	.byte	17                              // DW_AT_low_pc
	.byte	27                              // DW_FORM_addrx
	.byte	18                              // DW_AT_high_pc
	.byte	6                               // DW_FORM_data4
	.byte	64                              // DW_AT_frame_base
	.byte	24                              // DW_FORM_exprloc
	.byte	3                               // DW_AT_name
	.byte	37                              // DW_FORM_strx1
	.byte	58                              // DW_AT_decl_file
	.byte	11                              // DW_FORM_data1
	.byte	59                              // DW_AT_decl_line
	.byte	11                              // DW_FORM_data1
	.byte	39                              // DW_AT_prototyped
	.byte	25                              // DW_FORM_flag_present
	.byte	73                              // DW_AT_type
	.byte	19                              // DW_FORM_ref4
	.byte	63                              // DW_AT_external
	.byte	25                              // DW_FORM_flag_present
	.byte	0                               // EOM(1)
	.byte	0                               // EOM(2)
	.byte	6                               // Abbreviation Code
	.byte	5                               // DW_TAG_formal_parameter
	.byte	0                               // DW_CHILDREN_no
	.byte	2                               // DW_AT_location
	.byte	34                              // DW_FORM_loclistx
	.byte	3                               // DW_AT_name
	.byte	37                              // DW_FORM_strx1
	.byte	58                              // DW_AT_decl_file
	.byte	11                              // DW_FORM_data1
	.byte	59                              // DW_AT_decl_line
	.byte	11                              // DW_FORM_data1
	.byte	73                              // DW_AT_type
	.byte	19                              // DW_FORM_ref4
	.byte	0                               // EOM(1)
	.byte	0                               // EOM(2)
	.byte	7                               // Abbreviation Code
	.byte	52                              // DW_TAG_variable
	.byte	0                               // DW_CHILDREN_no
	.byte	2                               // DW_AT_location
	.byte	24                              // DW_FORM_exprloc
	.byte	3                               // DW_AT_name
	.byte	37                              // DW_FORM_strx1
	.byte	58                              // DW_AT_decl_file
	.byte	11                              // DW_FORM_data1
	.byte	59                              // DW_AT_decl_line
	.byte	11                              // DW_FORM_data1
	.byte	73                              // DW_AT_type
	.byte	19                              // DW_FORM_ref4
	.byte	0                               // EOM(1)
	.byte	0                               // EOM(2)
	.byte	8                               // Abbreviation Code
	.byte	52                              // DW_TAG_variable
	.byte	0                               // DW_CHILDREN_no
	.byte	2                               // DW_AT_location
	.byte	34                              // DW_FORM_loclistx
	.byte	3                               // DW_AT_name
	.byte	37                              // DW_FORM_strx1
	.byte	58                              // DW_AT_decl_file
	.byte	11                              // DW_FORM_data1
	.byte	59                              // DW_AT_decl_line
	.byte	11                              // DW_FORM_data1
	.byte	73                              // DW_AT_type
	.byte	19                              // DW_FORM_ref4
	.byte	0                               // EOM(1)
	.byte	0                               // EOM(2)
	.byte	9                               // Abbreviation Code
	.byte	53                              // DW_TAG_volatile_type
	.byte	0                               // DW_CHILDREN_no
	.byte	73                              // DW_AT_type
	.byte	19                              // DW_FORM_ref4
	.byte	0                               // EOM(1)
	.byte	0                               // EOM(2)
	.byte	0                               // EOM(3)
	.section	.debug_info,"",@progbits
.Lcu_begin0:
	.word	.Ldebug_info_end0-.Ldebug_info_start0 // Length of Unit
.Ldebug_info_start0:
	.hword	5                               // DWARF version number
	.byte	1                               // DWARF Unit Type
	.byte	8                               // Address Size (in bytes)
	.word	.debug_abbrev                   // Offset Into Abbrev. Section
	.byte	1                               // Abbrev [1] 0xc:0xa7 DW_TAG_compile_unit
	.byte	0                               // DW_AT_producer
	.hword	12                              // DW_AT_language
	.byte	1                               // DW_AT_name
	.word	.Lstr_offsets_base0             // DW_AT_str_offsets_base
	.word	.Lline_table_start0             // DW_AT_stmt_list
	.byte	2                               // DW_AT_comp_dir
	.byte	0                               // DW_AT_low_pc
	.word	.Lfunc_end0-.Lfunc_begin0       // DW_AT_high_pc
	.word	.Laddr_table_base0              // DW_AT_addr_base
	.word	.Lloclists_table_base0          // DW_AT_loclists_base
	.byte	2                               // Abbrev [2] 0x27:0x5 DW_TAG_pointer_type
	.word	44                              // DW_AT_type
	.byte	3                               // Abbrev [3] 0x2c:0x4 DW_TAG_base_type
	.byte	3                               // DW_AT_name
	.byte	5                               // DW_AT_encoding
	.byte	4                               // DW_AT_byte_size
	.byte	4                               // Abbrev [4] 0x30:0x8 DW_TAG_typedef
	.word	56                              // DW_AT_type
	.byte	5                               // DW_AT_name
	.byte	1                               // DW_AT_decl_file
	.byte	90                              // DW_AT_decl_line
	.byte	3                               // Abbrev [3] 0x38:0x4 DW_TAG_base_type
	.byte	4                               // DW_AT_name
	.byte	7                               // DW_AT_encoding
	.byte	8                               // DW_AT_byte_size
	.byte	5                               // Abbrev [5] 0x3c:0x5a DW_TAG_subprogram
	.byte	0                               // DW_AT_low_pc
	.word	.Lfunc_end0-.Lfunc_begin0       // DW_AT_high_pc
	.byte	1                               // DW_AT_frame_base
	.byte	109
	.byte	6                               // DW_AT_name
	.byte	0                               // DW_AT_decl_file
	.byte	4                               // DW_AT_decl_line
                                        // DW_AT_prototyped
	.word	44                              // DW_AT_type
                                        // DW_AT_external
	.byte	6                               // Abbrev [6] 0x4b:0x9 DW_TAG_formal_parameter
	.byte	0                               // DW_AT_location
	.byte	8                               // DW_AT_name
	.byte	0                               // DW_AT_decl_file
	.byte	4                               // DW_AT_decl_line
	.word	44                              // DW_AT_type
	.byte	6                               // Abbrev [6] 0x54:0x9 DW_TAG_formal_parameter
	.byte	1                               // DW_AT_location
	.byte	9                               // DW_AT_name
	.byte	0                               // DW_AT_decl_file
	.byte	4                               // DW_AT_decl_line
	.word	155                             // DW_AT_type
	.byte	7                               // Abbrev [7] 0x5d:0xb DW_TAG_variable
	.byte	2                               // DW_AT_location
	.byte	145
	.byte	124
	.byte	7                               // DW_AT_name
	.byte	0                               // DW_AT_decl_file
	.byte	17                              // DW_AT_decl_line
	.word	150                             // DW_AT_type
	.byte	8                               // Abbrev [8] 0x68:0x9 DW_TAG_variable
	.byte	2                               // DW_AT_location
	.byte	11                              // DW_AT_name
	.byte	0                               // DW_AT_decl_file
	.byte	6                               // DW_AT_decl_line
	.word	44                              // DW_AT_type
	.byte	8                               // Abbrev [8] 0x71:0x9 DW_TAG_variable
	.byte	3                               // DW_AT_location
	.byte	12                              // DW_AT_name
	.byte	0                               // DW_AT_decl_file
	.byte	7                               // DW_AT_decl_line
	.word	169                             // DW_AT_type
	.byte	8                               // Abbrev [8] 0x7a:0x9 DW_TAG_variable
	.byte	4                               // DW_AT_location
	.byte	13                              // DW_AT_name
	.byte	0                               // DW_AT_decl_file
	.byte	14                              // DW_AT_decl_line
	.word	174                             // DW_AT_type
	.byte	8                               // Abbrev [8] 0x83:0x9 DW_TAG_variable
	.byte	5                               // DW_AT_location
	.byte	15                              // DW_AT_name
	.byte	0                               // DW_AT_decl_file
	.byte	15                              // DW_AT_decl_line
	.word	174                             // DW_AT_type
	.byte	8                               // Abbrev [8] 0x8c:0x9 DW_TAG_variable
	.byte	6                               // DW_AT_location
	.byte	16                              // DW_AT_name
	.byte	0                               // DW_AT_decl_file
	.byte	9                               // DW_AT_decl_line
	.word	39                              // DW_AT_type
	.byte	0                               // End Of Children Mark
	.byte	9                               // Abbrev [9] 0x96:0x5 DW_TAG_volatile_type
	.word	44                              // DW_AT_type
	.byte	2                               // Abbrev [2] 0x9b:0x5 DW_TAG_pointer_type
	.word	160                             // DW_AT_type
	.byte	2                               // Abbrev [2] 0xa0:0x5 DW_TAG_pointer_type
	.word	165                             // DW_AT_type
	.byte	3                               // Abbrev [3] 0xa5:0x4 DW_TAG_base_type
	.byte	10                              // DW_AT_name
	.byte	8                               // DW_AT_encoding
	.byte	1                               // DW_AT_byte_size
	.byte	2                               // Abbrev [2] 0xa9:0x5 DW_TAG_pointer_type
	.word	150                             // DW_AT_type
	.byte	3                               // Abbrev [3] 0xae:0x4 DW_TAG_base_type
	.byte	14                              // DW_AT_name
	.byte	4                               // DW_AT_encoding
	.byte	8                               // DW_AT_byte_size
	.byte	0                               // End Of Children Mark
.Ldebug_info_end0:
	.section	.debug_str_offsets,"",@progbits
	.word	72                              // Length of String Offsets Set
	.hword	5
	.hword	0
.Lstr_offsets_base0:
	.section	.debug_str,"MS",@progbits,1
.Linfo_string0:
	.asciz	"Ubuntu clang version 15.0.7"   // string offset=0
.Linfo_string1:
	.asciz	"forwarding_micro.c"            // string offset=28
.Linfo_string2:
	.asciz	"/work/muke/PND-Loads/benchmarks" // string offset=47
.Linfo_string3:
	.asciz	"int"                           // string offset=79
.Linfo_string4:
	.asciz	"unsigned long"                 // string offset=83
.Linfo_string5:
	.asciz	"uintptr_t"                     // string offset=97
.Linfo_string6:
	.asciz	"main"                          // string offset=107
.Linfo_string7:
	.asciz	"y"                             // string offset=112
.Linfo_string8:
	.asciz	"argc"                          // string offset=114
.Linfo_string9:
	.asciz	"argv"                          // string offset=119
.Linfo_string10:
	.asciz	"char"                          // string offset=124
.Linfo_string11:
	.asciz	"n"                             // string offset=129
.Linfo_string12:
	.asciz	"load"                          // string offset=131
.Linfo_string13:
	.asciz	"offset"                        // string offset=136
.Linfo_string14:
	.asciz	"double"                        // string offset=143
.Linfo_string15:
	.asciz	"factor"                        // string offset=150
.Linfo_string16:
	.asciz	"store"                         // string offset=157
	.section	.debug_str_offsets,"",@progbits
	.word	.Linfo_string0
	.word	.Linfo_string1
	.word	.Linfo_string2
	.word	.Linfo_string3
	.word	.Linfo_string4
	.word	.Linfo_string5
	.word	.Linfo_string6
	.word	.Linfo_string7
	.word	.Linfo_string8
	.word	.Linfo_string9
	.word	.Linfo_string10
	.word	.Linfo_string11
	.word	.Linfo_string12
	.word	.Linfo_string13
	.word	.Linfo_string14
	.word	.Linfo_string15
	.word	.Linfo_string16
	.section	.debug_addr,"",@progbits
	.word	.Ldebug_addr_end0-.Ldebug_addr_start0 // Length of contribution
.Ldebug_addr_start0:
	.hword	5                               // DWARF version number
	.byte	8                               // Address size
	.byte	0                               // Segment selector size
.Laddr_table_base0:
	.xword	.Lfunc_begin0
.Ldebug_addr_end0:
	.ident	"Ubuntu clang version 15.0.7"
	.section	".note.GNU-stack","",@progbits
	.section	.debug_line,"",@progbits
.Lline_table_start0:
