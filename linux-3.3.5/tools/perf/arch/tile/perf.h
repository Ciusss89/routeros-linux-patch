/*
 * Copyright 2012 Tilera Corporation. All Rights Reserved.
 *
 *   This program is free software; you can redistribute it and/or
 *   modify it under the terms of the GNU General Public License
 *   as published by the Free Software Foundation, version 2.
 *
 *   This program is distributed in the hope that it will be useful, but
 *   WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE, GOOD TITLE or
 *   NON INFRINGEMENT.  See the GNU General Public License for
 *   more details.
 *
 * Tilera TILEPro and TILEGx specific performance events.
 *
 * We have a sort-order requirement if some event names share the same
 * beginning. Take TLB_EXCEPTION and TLB for example, we should put
 * TLB_EXCEPTION before TLB, otherwise perf tool will take TLB as a correct
 * event name, and then continue to parse the rest string "_EXCEPTION",
 * then failed.
 */

#define CRW(x) .type = PERF_TYPE_RAW, .config = PERF_COUNT_RAW_##x

#if defined(__tilepro__)
#define PERF_COUNT_RAW_ZERO                             0x000
#define PERF_COUNT_RAW_ONE                              0x001
#define PERF_COUNT_RAW_PASS_WRITTEN                     0x002
#define PERF_COUNT_RAW_FAIL_WRITTEN                     0x003
#define PERF_COUNT_RAW_DONE_WRITTEN                     0x004

#define PERF_COUNT_RAW_MP_BUNDLE_RETIRED                0x006
#define PERF_COUNT_RAW_MP_BUNDLE_1_INSTR_RETIRED        0x007
#define PERF_COUNT_RAW_MP_BUNDLE_2_INSTR_RETIRED        0x008
#define PERF_COUNT_RAW_MP_BUNDLE_3_INSTR_RETIRED        0x009
#define PERF_COUNT_RAW_MP_UDN_READ_STALL                0x00a

#define PERF_COUNT_RAW_MP_IDN_READ_STALL                0x00b
#define PERF_COUNT_RAW_MP_SN_READ_STALL                 0x00c
#define PERF_COUNT_RAW_MP_UDN_WRITE_STALL               0x00d
#define PERF_COUNT_RAW_MP_IDN_WRITE_STALL               0x00e
#define PERF_COUNT_RAW_MP_SN_WRITE_STALL                0x00f

#define PERF_COUNT_RAW_MP_DATA_CACHE_STALL              0x010
#define PERF_COUNT_RAW_MP_INSTRUCTION_CACHE_STALL       0x011
#define PERF_COUNT_RAW_MP_ICACHE_HIT_ISSUED             0x012
#define PERF_COUNT_RAW_MP_ITLB_HIT_ISSUED               0x013
#define PERF_COUNT_RAW_MP_CONDITIONAL_BRANCH_MISSPREDICT 0x014

#define PERF_COUNT_RAW_MP_INDIRECT_BRANCH_MISSPREDICT   0x015
#define PERF_COUNT_RAW_MP_CONDITIONAL_BRANCH_ISSUED     0x016
#define PERF_COUNT_RAW_MP_INDIRECT_BRANCH_ISSUED        0x017
#define PERF_COUNT_RAW_MP_CACHE_BUSY_STALL              0x018
#define PERF_COUNT_RAW_MP_NAP_STALL                     0x019

#define PERF_COUNT_RAW_MP_SPR_STALL             0x01a
#define PERF_COUNT_RAW_MP_ALU_STALL             0x01b
#define PERF_COUNT_RAW_MP_LOAD_MISS_REPLAY_TRAP 0x01c
#define PERF_COUNT_RAW_TLB_CNT                  0x01d
#define PERF_COUNT_RAW_RD_CNT                   0x01e

#define PERF_COUNT_RAW_WR_CNT                   0x01f
#define PERF_COUNT_RAW_TLB_EXCEPTION            0x020
#define PERF_COUNT_RAW_RD_MISS                  0x021
#define PERF_COUNT_RAW_WR_MISS                  0x022
#define PERF_COUNT_RAW_SNP_INC_RD_CNT           0x023

#define PERF_COUNT_RAW_SNP_NINC_RD_CNT          0x024
#define PERF_COUNT_RAW_SNP_WR_CNT               0x025
#define PERF_COUNT_RAW_SNP_IO_RD_CNT            0x026
#define PERF_COUNT_RAW_SNP_IO_WR_CNT            0x027
#define PERF_COUNT_RAW_SNP_LOCAL_DRD_CNT        0x028

#define PERF_COUNT_RAW_LOCAL_WR_CNT             0x029
#define PERF_COUNT_RAW_LOCAL_IRD_CNT            0x02a
#define PERF_COUNT_RAW_REMOTE_DRD_CNT           0x02b
#define PERF_COUNT_RAW_REMOTE_WR_CNT            0x02c
#define PERF_COUNT_RAW_REMOTE_IRD_CNT           0x02d

#define PERF_COUNT_RAW_COH_INV_CNT              0x02e
#define PERF_COUNT_RAW_SNP_INC_RD_MISS          0x02f
#define PERF_COUNT_RAW_SNP_NINC_RD_MISS         0x030
#define PERF_COUNT_RAW_SNP_WR_MISS              0x031
#define PERF_COUNT_RAW_SNP_IO_RD_MISS           0x032

#define PERF_COUNT_RAW_SNP_IO_WR_MISS           0x033
#define PERF_COUNT_RAW_LOCAL_DRD_MISS           0x034
#define PERF_COUNT_RAW_LOCAL_WR_MISS            0x035
#define PERF_COUNT_RAW_LOCAL_IRD_MISS           0x036
#define PERF_COUNT_RAW_REMOTE_DRD_MISS          0x037

#define PERF_COUNT_RAW_REMOTE_WR_MISS           0x038
#define PERF_COUNT_RAW_REMOTE_IRD_MISS          0x039
#define PERF_COUNT_RAW_COH_INV_HIT              0x03a
#define PERF_COUNT_RAW_VIC_WB_CNT               0x03b
#define PERF_COUNT_RAW_TDN_STARVED              0x03c

#define PERF_COUNT_RAW_DMA_STARVED              0x03d
#define PERF_COUNT_RAW_MDN_STARVED              0x03e
#define PERF_COUNT_RAW_RTF_STARVED              0x03f
#define PERF_COUNT_RAW_IREQ_STARVED             0x040
#define PERF_COUNT_RAW_RRTF_STARVED             0x041

#define PERF_COUNT_RAW_UDN_PKT_SNT              0x054
#define PERF_COUNT_RAW_UDN_SNT                  0x055
#define PERF_COUNT_RAW_UDN_BUBBLE               0x056
#define PERF_COUNT_RAW_UDN_CONGESTION           0x057
#define PERF_COUNT_RAW_IDN_PKT_SNT              0x058

#define PERF_COUNT_RAW_IDN_SNT                  0x059
#define PERF_COUNT_RAW_IDN_BUBBLE               0x05a
#define PERF_COUNT_RAW_IDN_CONGESTION           0x05b
#define PERF_COUNT_RAW_MDN_PKT_SNT              0x05c
#define PERF_COUNT_RAW_MDN_SNT                  0x05d

#define PERF_COUNT_RAW_MDN_BUBBLE               0x05e
#define PERF_COUNT_RAW_MDN_CONGESTION           0x05f
#define PERF_COUNT_RAW_TDN_PKT_SNT              0x060
#define PERF_COUNT_RAW_TDN_SNT                  0x061
#define PERF_COUNT_RAW_TDN_BUBBLE               0x062

#define PERF_COUNT_RAW_TDN_CONGESTION           0x063
#define PERF_COUNT_RAW_VDN_PKT_SNT              0x064
#define PERF_COUNT_RAW_VDN_SNT                  0x065
#define PERF_COUNT_RAW_VDN_BUBBLE               0x066
#define PERF_COUNT_RAW_VDN_CONGESTION           0x067

#define PERF_COUNT_RAW_UDN_DMUX_STALL           0x068
#define PERF_COUNT_RAW_IDN_DMUX_STALL           0x069

  { CRW(ONE),				"ONE",				""},
  { CRW(MP_BUNDLE_RETIRED),		"MP_BUNDLE_RETIRED",		""},
  { CRW(MP_BUNDLE_1_INSTR_RETIRED),	"MP_BUNDLE_1_INSTR_RETIRED",	""},
  { CRW(MP_BUNDLE_2_INSTR_RETIRED),	"MP_BUNDLE_2_INSTR_RETIRED",	""},
  { CRW(MP_BUNDLE_3_INSTR_RETIRED),	"MP_BUNDLE_3_INSTR_RETIRED",	""},
  { CRW(MP_INSTRUCTION_CACHE_STALL),	"MP_INSTRUCTION_CACHE_STALL",	""},
  { CRW(MP_DATA_CACHE_STALL),		"MP_DATA_CACHE_STALL",		""},
  { CRW(TLB_EXCEPTION),			"TLB_EXCEPTION",		""},
  { CRW(TLB_CNT),			"TLB_CNT",			""},
  { CRW(ZERO),				"ZERO",				""},

  { CRW(PASS_WRITTEN),			"PASS_WRITTEN",			""},
  { CRW(DONE_WRITTEN),			"DONE_WRITTEN",			""},
  { CRW(MP_UDN_READ_STALL),		"MP_UDN_READ_STALL",		""},
  { CRW(MP_IDN_READ_STALL),		"MP_IDN_READ_STALL",		""},
  { CRW(MP_SN_READ_STALL),		"MP_SN_READ_STALL",		""},
  { CRW(MP_UDN_WRITE_STALL),		"MP_UDN_WRITE_STALL",		""},
  { CRW(MP_IDN_READ_STALL),		"MP_IDN_READ_STALL",		""},
  { CRW(MP_SN_WRITE_STALL),		"MP_SN_WRITE_STALL",		""},
  { CRW(MP_DATA_CACHE_STALL),		"MP_DATA_CACHE_STALL",		""},
  { CRW(MP_INSTRUCTION_CACHE_STALL),	"MP_INSTRUCTION_CACHE_STALL",	""},

  { CRW(MP_ICACHE_HIT_ISSUED),		"MP_ICACHE_HIT_ISSUED",		""},
  { CRW(MP_ITLB_HIT_ISSUED),		"MP_ITLB_HIT_ISSUED",		""},
  { CRW(MP_CONDITIONAL_BRANCH_MISSPREDICT),	
				"MP_CONDITIONAL_BRANCH_MISSPREDICT",	""},
  { CRW(MP_INDIRECT_BRANCH_MISSPREDICT),
				"MP_INDIRECT_BRANCH_MISSPREDICT",	""},
  { CRW(MP_CONDITIONAL_BRANCH_ISSUED),	"MP_CONDITIONAL_BRANCH_ISSUED",	""},
  { CRW(MP_INDIRECT_BRANCH_ISSUED),	"MP_INDIRECT_BRANCH_ISSUED",	""},
  { CRW(MP_CACHE_BUSY_STALL),		"MP_CACHE_BUSY_STALL",		""},
  { CRW(MP_NAP_STALL),			"MP_NAP_STALL",			""},
  { CRW(MP_SPR_STALL),			"MP_SPR_STALL",			""},
  { CRW(MP_ALU_STALL),			"MP_ALU_STALL",			""},

  { CRW(MP_LOAD_MISS_REPLAY_TRAP),	"MP_LOAD_MISS_REPLAY_TRAP",	""},
  { CRW(RD_CNT),			"RD_CNT",			""},
  { CRW(WR_CNT),			"WR_CNT",			""},
  { CRW(RD_MISS),			"RD_MISS",			""},
  { CRW(WR_MISS),			"WR_MISS",			""},
  { CRW(SNP_INC_RD_CNT),		"SNP_INC_RD_CNT",		""},
  { CRW(SNP_NINC_RD_CNT),		"SNP_NINC_RD_CNT",		""},
  { CRW(SNP_WR_CNT),			"SNP_WR_CNT",			""},
  { CRW(SNP_IO_RD_CNT),			"SNP_IO_RD_CNT",		""},
  { CRW(SNP_IO_WR_CNT),			"SNP_IO_WR_CNT",		""},
  
  { CRW(SNP_LOCAL_DRD_CNT),		"SNP_LOCAL_DRD_CNT",		""},
  { CRW(LOCAL_WR_CNT),			"LOCAL_WR_CNT",			""},
  { CRW(LOCAL_IRD_CNT),			"LOCAL_IRD_CNT",		""},
  { CRW(REMOTE_DRD_CNT),		"REMOTE_DRD_CNT",		""},
  { CRW(REMOTE_WR_CNT),			"REMOTE_WR_CNT",		""},
  { CRW(REMOTE_IRD_CNT),		"REMOTE_IRD_CNT",		""},
  { CRW(COH_INV_CNT),			"COH_INV_CNT",			""},
  { CRW(SNP_INC_RD_MISS),		"SNP_INC_RD_MISS",		""},
  { CRW(SNP_NINC_RD_MISS),		"SNP_NINC_RD_MISS",		""},
  { CRW(SNP_WR_MISS),			"SNP_WR_MISS",			""},

  { CRW(SNP_IO_RD_MISS),		"SNP_IO_RD_MISS",		""},
  { CRW(SNP_IO_WR_MISS),		"SNP_IO_WR_MISS",		""},
  { CRW(LOCAL_DRD_MISS),		"LOCAL_DRD_MISS",		""},
  { CRW(LOCAL_WR_MISS),			"LOCAL_WR_MISS",		""},
  { CRW(LOCAL_IRD_MISS),		"LOCAL_IRD_MISS",		""},
  { CRW(REMOTE_DRD_MISS),		"REMOTE_DRD_MISS",		""},
  { CRW(REMOTE_WR_MISS),		"REMOTE_WR_MISS",		""},
  { CRW(REMOTE_IRD_MISS),		"REMOTE_IRD_MISS",		""},
  { CRW(COH_INV_HIT),			"COH_INV_HIT",			""},
  { CRW(VIC_WB_CNT),			"VIC_WB_CNT",			""},

  { CRW(TDN_STARVED),			"TDN_STARVED",			""},
  { CRW(DMA_STARVED),			"DMA_STARVED",			""},
  { CRW(MDN_STARVED),			"MDN_STARVED",			""},
  { CRW(RTF_STARVED),			"RTF_STARVED",			""},
  { CRW(IREQ_STARVED),			"IREQ_STARVED",			""},
  { CRW(RRTF_STARVED),			"RRTF_STARVED",			""},
  { CRW(UDN_PKT_SNT),			"UDN_PKT_SNT",			""},
  { CRW(UDN_SNT),			"UDN_SNT",			""},
  { CRW(UDN_BUBBLE),			"UDN_BUBBLE",			""},
  { CRW(UDN_CONGESTION),		"UDN_CONGESTION",		""},

  { CRW(IDN_PKT_SNT),			"IDN_PKT_SNT",			""},
  { CRW(IDN_SNT),			"IDN_SNT",			""},
  { CRW(IDN_BUBBLE),			"IDN_BUBBLE",			""},
  { CRW(IDN_CONGESTION),		"IDN_CONGESTION",		""},
  { CRW(MDN_PKT_SNT),			"MDN_PKT_SNT",			""},
  { CRW(MDN_SNT),			"MDN_SNT",			""},
  { CRW(MDN_BUBBLE),			"MDN_CONGESTION",		""},
  { CRW(TDN_PKT_SNT),			"TDN_PKT_SNT",			""},
  { CRW(TDN_SNT),			"TDN_SNT",			""},
  { CRW(TDN_BUBBLE),			"TDN_BUBBLE",			""},

  { CRW(TDN_CONGESTION),		"TDN_CONGESTION",		""},
  { CRW(VDN_PKT_SNT),			"VDN_PKT_SNT",			""},
  { CRW(VDN_SNT),			"VDN_SNT",			""},
  { CRW(VDN_BUBBLE),			"VDN_BUBBLE",			""},
  { CRW(VDN_CONGESTION),		"VDN_CONGESTION",		""},
  { CRW(UDN_DMUX_STALL),		"UDN_DMUX_STALL",		""},
  { CRW(IDN_DMUX_STALL),		"IDN_DMUX_STALL",		""},
#endif /* __tilepro__*/

#ifdef __tilegx__
#define PERF_COUNT_RAW_ZERO			0x180
#define PERF_COUNT_RAW_ONE			0x181
#define PERF_COUNT_RAW_PASS_WRITTEN		0x182
#define PERF_COUNT_RAW_FAIL_WRITTEN		0x183
#define PERF_COUNT_RAW_DONE_WRITTEN		0x184
#define PERF_COUNT_RAW_CBOX_QUIESCED		0x186

#define PERF_COUNT_RAW_L1D_FILL_STALL           0x0c4
#define PERF_COUNT_RAW_CBOX_FULL_STALL		0x0c5
#define PERF_COUNT_RAW_LOAD_HIT_STALL   	0x0c6
#define PERF_COUNT_RAW_LOAD_STALL		0x0c7
#define PERF_COUNT_RAW_ALU_SRC_STALL        	0x0c8
#define PERF_COUNT_RAW_IDN_SRC_STALL            0x0c9

#define PERF_COUNT_RAW_UDN_SRC_STALL		0x0ca
#define PERF_COUNT_RAW_MF_STALL                 0x0cb
#define PERF_COUNT_RAW_SLOW_SPR_STALL           0x0cc
#define PERF_COUNT_RAW_NETWORK_DEST_STALL       0x0cd
#define PERF_COUNT_RAW_INSTRUCTION_STALL        0x0ce
#define PERF_COUNT_RAW_PFB_HIT_IN_PFB		0x0cf

#define PERF_COUNT_RAW_PFB_HIT			0x0d0
#define PERF_COUNT_RAW_CBOX_RESP		0x0d1
#define PERF_COUNT_RAW_CBOX_REQ			0x0d3
#define PERF_COUNT_RAW_ITLB_MISS_INTERRUPT	0x0d4
#define PERF_COUNT_RAW_INTERRUPT		0x0d5
#define PERF_COUNT_RAW_ICACHE_FILL_PEND		0x0d6
#define PERF_COUNT_RAW_ICACHE_FILL		0x0d7

#define PERF_COUNT_RAW_WAY_MISPREDICT		0x0d8
#define PERF_COUNT_RAW_COND_BRANCH_PRED_CORRECT		0x0d9
#define PERF_COUNT_RAW_COND_BRANCH_PRED_INCORRECT	0x0da
#define PERF_COUNT_RAW_INSTRUCTION_BUNDLE	0x0db
#define PERF_COUNT_RAW_USED_RAS			0x0dd
#define PERF_COUNT_RAW_RAS_CORRECT		0x0de
#define PERF_COUNT_RAW_COND_BRANCH_NO_PREDICT	0x0df
#define PERF_COUNT_RAW_TLB			0x040
#define PERF_COUNT_RAW_READ			0x041

#define PERF_COUNT_RAW_WRITE			0x042
#define PERF_COUNT_RAW_TLB_EXCEPTION		0x043
#define PERF_COUNT_RAW_READ_MISS		0x044
#define PERF_COUNT_RAW_WRITE_MISS		0x045
#define PERF_COUNT_RAW_L1_OPCODE_VALID		0x046

#define PERF_COUNT_RAW_L1_OPCODE_LD_VALID	0x047
#define PERF_COUNT_RAW_L1_OPCODE_ST_VALID	0x048
#define PERF_COUNT_RAW_L1_OPCODE_ATOMIC_VALID	0x049
#define PERF_COUNT_RAW_L1_OPCODE_FLUSH_VALID	0x04a
#define PERF_COUNT_RAW_L1_OPCODE_INV_VALID	0x04b

#define PERF_COUNT_RAW_L1_OPCODE_FINV_VALID	0x04c
#define PERF_COUNT_RAW_L1_OPCODE_MF_VALID	0x04d
#define PERF_COUNT_RAW_L1_OPCODE_PFETCH_VALID	0x04e
#define PERF_COUNT_RAW_L1_OPCODE_WH64_VALID	0x04f
#define PERF_COUNT_RAW_L1_OPCODE_DTLBPR_VALID	0x050

#define PERF_COUNT_RAW_L1_OPCODE_FWB_VALID	0x051
#define PERF_COUNT_RAW_L1_OPCODE_LD_NON_TEMPORAL_VALID	0x052
#define PERF_COUNT_RAW_L1_OPCODE_ST_NON_TEMPORAL_VALID	0x053
#define PERF_COUNT_RAW_SNOOP_INCREMENT_READ	0x080

#define PERF_COUNT_RAW_SNOOP_NON_INCREMENT_READ	0x081
#define PERF_COUNT_RAW_SNOOP_WRITE		0x082
#define PERF_COUNT_RAW_SNOOP_IO_READ		0x083
#define PERF_COUNT_RAW_SNOOP_IO_WRITE		0x084
#define PERF_COUNT_RAW_LOCAL_DATA_READ		0x085

#define PERF_COUNT_RAW_LOCAL_WRITE		0x086
#define PERF_COUNT_RAW_LOCAL_INSTRUCTION_READ	0x087
#define PERF_COUNT_RAW_REMOTE_DATA_READ		0x088
#define PERF_COUNT_RAW_REMOTE_WRITE		0x089
#define PERF_COUNT_RAW_REMOTE_INSTRUCTION_READ	0x08a

#define PERF_COUNT_RAW_COHERENCE_INVALIDATION	0x08b
#define PERF_COUNT_RAW_SNOOP_INCREMENT_READ_MISS	0x08c
#define PERF_COUNT_RAW_SNOOP_NON_INCREMENT_READ_MISS	0x08d
#define PERF_COUNT_RAW_SNOOP_WRITE_MISS		0x08e
#define PERF_COUNT_RAW_SNOOP_IO_READ_MISS	0x08f

#define PERF_COUNT_RAW_SNOOP_IO_WRITE_MISS	0x090
#define PERF_COUNT_RAW_LOCAL_DATA_READ_MISS	0x091
#define PERF_COUNT_RAW_LOCAL_WRITE_MISS		0x092
#define PERF_COUNT_RAW_LOCAL_INSTRUCTION_READ_MISS	0x093
#define PERF_COUNT_RAW_REMOTE_DATA_READ_MISS	0x094

#define PERF_COUNT_RAW_REMOTE_WRITE_MISS	0x095
#define PERF_COUNT_RAW_REMOTE_INSTRUCTION_READ_MISS	0x096
#define PERF_COUNT_RAW_COHERENCE_INVALIDATION_HIT	0x097
#define PERF_COUNT_RAW_CACHE_WRITEBACK		0x098
#define PERF_COUNT_RAW_SDN_STARVED		0x099

#define PERF_COUNT_RAW_RDN_STARVED		0x09a
#define PERF_COUNT_RAW_QDN_STARVED		0x09b
#define PERF_COUNT_RAW_SKF_STARVED		0x09c
#define PERF_COUNT_RAW_RTF_STARVED		0x09d
#define PERF_COUNT_RAW_IREQ_STARVED		0x09e

#define PERF_COUNT_RAW_LOCAL_WRITE_BUFFER_ALLOC	0x0a1
#define PERF_COUNT_RAW_REMOTE_WRITE_BUFFER_ALLOC	0x0a2
#define PERF_COUNT_RAW_ARB_VALID		0x0a3
#define PERF_COUNT_RAW_MDF_WRITE		0x0a4
#define PERF_COUNT_RAW_LDB_READ			0x0a5

#define PERF_COUNT_RAW_L2_OPCODE_LD_VALID	0x0a6
#define PERF_COUNT_RAW_L2_OPCODE_ST_VALID	0x0a7
#define PERF_COUNT_RAW_L2_OPCODE_ATOMIC_VALID	0x0a8
#define PERF_COUNT_RAW_L2_OPCODE_FLUSH_VALID	0x0a9
#define PERF_COUNT_RAW_L2_OPCODE_INV_VALID	0x0aa

#define PERF_COUNT_RAW_L2_OPCODE_FINV_VALID	0x0ab
#define PERF_COUNT_RAW_L2_OPCODE_MF_VALID	0x0ac
#define PERF_COUNT_RAW_L2_OPCODE_PFETCH_VALID	0x0ad
#define PERF_COUNT_RAW_L2_OPCODE_WH64_VALID	0x0ae
#define PERF_COUNT_RAW_L2_OPCODE_FWB_VALID	0x0af

#define PERF_COUNT_RAW_L2_OPCODE_LD_NON_TEMPORAL_VALID	0x0b0
#define PERF_COUNT_RAW_L2_OPCODE_ST_NON_TEMPORAL_VALID	0x0b1
#define PERF_COUNT_RAW_L2_OPCODE_LD_NOFIL_VALID		0x0b2
#define PERF_COUNT_RAW_L2_OPCODE_LD_NOFIL_NON_TEMPORAL_VALID	0x0b3
#define PERF_COUNT_RAW_L2_OPCODE_RDN_VALID			0x0b4

#define PERF_COUNT_RAW_L2_OPCODE_QDN_VALID	0x0b5
#define PERF_COUNT_RAW_L2_OPCODE_IO_READ_VALID	0x0b6
#define PERF_COUNT_RAW_L2_OPCODE_IO_WRITE_VALID	0x0b7
#define PERF_COUNT_RAW_L2_OPCODE_I_STREAM_VALID	0x0b8
#define PERF_COUNT_RAW_L2_OPCODE_MDF_VALID	0x0b9

#define PERF_COUNT_RAW_L2_OPCODE_IREQ_IV_VALID	0x0ba
#define PERF_COUNT_RAW_UDN_PACKET_SENT		0x000
#define PERF_COUNT_RAW_UDN_WORD_SENT		0x001
#define PERF_COUNT_RAW_UDN_BUBBLE		0x002

#define PERF_COUNT_RAW_UDN_CONGESTION		0x003
#define PERF_COUNT_RAW_IDN_PACKET_SENT		0x004
#define PERF_COUNT_RAW_IDN_WORD_SENT		0x005
#define PERF_COUNT_RAW_IDN_BUBBLE		0x006
#define PERF_COUNT_RAW_IDN_CONGESTION		0x007

#define PERF_COUNT_RAW_RDN_PACKET_SENT		0x008
#define PERF_COUNT_RAW_RDN_WORD_SENT		0x009
#define PERF_COUNT_RAW_RDN_BUBBLE		0x00a
#define PERF_COUNT_RAW_RDN_CONGESTION		0x00b
#define PERF_COUNT_RAW_SDN_PACKET_SENT		0x00c

#define PERF_COUNT_RAW_SDN_WORD_SENT		0x00d
#define PERF_COUNT_RAW_SDN_BUBBLE		0x00e
#define PERF_COUNT_RAW_SDN_CONGESTION		0x00f
#define PERF_COUNT_RAW_QDN_PACKET_SENT		0x010
#define PERF_COUNT_RAW_QDN_WORD_SENT		0x011

#define PERF_COUNT_RAW_QDN_BUBBLE		0x012
#define PERF_COUNT_RAW_QDN_CONGESTION		0x013
#define PERF_COUNT_RAW_UDN_DEMUX_STALL		0x014
#define PERF_COUNT_RAW_IDN_DEMUX_STALL		0x015

  { CRW(ZERO),				"ZERO",				""},
  { CRW(ONE),				"ONE",				""},
  { CRW(PASS_WRITTEN),			"PASS_WRITTEN",			""},
  { CRW(FAIL_WRITTEN),			"FAIL_WRITTEN",			""},
  { CRW(DONE_WRITTEN),			"DONE_WRITTEN",			""},
  { CRW(L1D_FILL_STALL),		"L1D_FILL_STALL",		""},
  { CRW(LOAD_HIT_STALL),		"LOAD_HIT_STALL",		""},
  { CRW(LOAD_STALL),			"LOAD_STALL",			""},
  { CRW(ALU_SRC_STALL),			"ALU_SRC_STALL",		""},
  { CRW(IDN_SRC_STALL),			"IDN_SRC_STALL",		""},

  { CRW(UDN_SRC_STALL),			"UDN_SRC_STALL",		""},
  { CRW(MF_STALL),			"MF_STALL",			""},
  { CRW(SLOW_SPR_STALL),		"SLOW_SPR_STALL",		""},
  { CRW(NETWORK_DEST_STALL),		"NETWORK_DEST_STALL",		""},
  { CRW(INSTRUCTION_STALL),		"INSTRUCTION_STALL",		""},
  { CRW(ITLB_MISS_INTERRUPT),		"ITLB_MISS_INTERRUPT",		""},
  { CRW(INTERRUPT),			"INTERRUPT",			""},
  { CRW(INSTRUCTION_BUNDLE),		"INSTRUCTION_BUNDLE",		""},
  { CRW(TLB_EXCEPTION),			"TLB_EXCEPTION",		""},
  { CRW(READ_MISS),			"READ_MISS",			""},

  { CRW(WRITE_MISS),			"WRITE_MISS",			""},
  { CRW(TLB),				"TLB",				""},
  { CRW(READ),				"READ",				""},
  { CRW(WRITE),				"WRITE",			""},
  { CRW(SNOOP_INCREMENT_READ_MISS),	"SNOOP_INCREMENT_READ_MISS", 	""},
  { CRW(SNOOP_NON_INCREMENT_READ_MISS),	"SNOOP_NON_INCREMENT_READ_MISS",""},
  { CRW(SNOOP_WRITE_MISS),		"SNOOP_WRITE_MISS",		""},
  { CRW(SNOOP_IO_READ_MISS),		"SNOOP_IO_READ_MISS",		""},
  { CRW(SNOOP_IO_WRITE_MISS),		"SNOOP_IO_WRITE_MISS",		""},
  { CRW(LOCAL_DATA_READ_MISS),		"LOCAL_DATA_READ_MISS",		""},
  
  { CRW(LOCAL_WRITE_MISS),		"LOCAL_WRITE_MISS",		""},
  { CRW(LOCAL_INSTRUCTION_READ_MISS),	"LOCAL_INSTRUCTION_READ_MISS",	""},
  { CRW(REMOTE_DATA_READ_MISS),		"REMOTE_DATA_READ_MISS",	""},
  { CRW(REMOTE_WRITE_MISS),		"REMOTE_WRITE_MISS",		""},
  { CRW(REMOTE_INSTRUCTION_READ_MISS),	"REMOTE_INSTRUCTION_READ_MISS",	""},
  { CRW(COHERENCE_INVALIDATION_HIT),	"COHERENCE_INVALIDATION_HIT",	""},
  { CRW(SNOOP_INCREMENT_READ),		"SNOOP_INCREMENT_READ",		""},
  { CRW(SNOOP_NON_INCREMENT_READ),	"SNOOP_NON_INCREMENT_READ",	""},
  { CRW(SNOOP_WRITE),			"SNOOP_WRITE",			""},
  { CRW(SNOOP_IO_READ),			"SNOOP_IO_READ",		""},
  
  { CRW(SNOOP_IO_WRITE),		"SNOOP_IO_WRITE",		""},
  { CRW(LOCAL_DATA_READ),		"LOCAL_DATA_READ",		""},
  { CRW(LOCAL_WRITE),			"LOCAL_WRITE",			""},
  { CRW(LOCAL_INSTRUCTION_READ),	"LOCAL_INSTRUCTION_READ", 	""},
  { CRW(REMOTE_DATA_READ),		"REMOTE_DATA_READ",		""},
  { CRW(REMOTE_WRITE),			"REMOTE_WRITE",			""},
  { CRW(REMOTE_INSTRUCTION_READ),	"REMOTE_INSTRUCTION_READ", 	""},
  { CRW(COHERENCE_INVALIDATION),	"COHERENCE_INVALIDATION", 	""},
  { CRW(CACHE_WRITEBACK),		"CACHE_WRITEBACK",		""},

  { CRW(SDN_STARVED),			"SDN_STARVED",			""},
  { CRW(RDN_STARVED),			"RDN_STARVED",			""},
  { CRW(QDN_STARVED),			"QDN_STARVED",			""},
  { CRW(SKF_STARVED),			"SKF_STARVED",			""},
  { CRW(RTF_STARVED),			"RTF_STARVED",			""},
  { CRW(IREQ_STARVED),			"IREQ_STARVED",			""},
  { CRW(LOCAL_WRITE_BUFFER_ALLOC),	"LOCAL_WRITE_BUFFER_ALLOC",	""},
  { CRW(REMOTE_WRITE_BUFFER_ALLOC),	"REMOTE_WRITE_BUFFER_ALLOC",	""},
  { CRW(UDN_PACKET_SENT),		"UDN_PACKET_SENT",		""},
  { CRW(UDN_WORD_SENT),			"UDN_WORD_SENT",		""},

  { CRW(UDN_BUBBLE),			"UDN_BUBBLE",			""},
  { CRW(UDN_CONGESTION),		"UDN_CONGESTION",		""},
  { CRW(IDN_PACKET_SENT),		"IDN_PACKET_SENT",		""},
  { CRW(IDN_WORD_SENT),			"IDN_WORD_SENT",		""},
  { CRW(IDN_BUBBLE),			"IDN_BUBBLE",			""},
  { CRW(IDN_CONGESTION),		"IDN_CONGESTION",		""},
  { CRW(RDN_PACKET_SENT),		"RDN_PACKET_SENT",		""},
  { CRW(RDN_WORD_SENT),			"RDN_WORD_SENT",		""},
  { CRW(RDN_BUBBLE),			"RDN_BUBBLE",			""},
  { CRW(RDN_CONGESTION),		"RDN_CONGESTION",		""},

  { CRW(SDN_PACKET_SENT),		"SDN_PACKET_SENT",		""},
  { CRW(SDN_WORD_SENT),			"SDN_WORD_SENT",		""},
  { CRW(SDN_BUBBLE),			"SDN_BUBBLE",			""},
  { CRW(SDN_CONGESTION),		"SDN_CONGESTION",		""},
  { CRW(QDN_PACKET_SENT),		"QDN_PACKET_SENT",		""},
  { CRW(QDN_WORD_SENT),			"QDN_WORD_SENT",		""},
  { CRW(QDN_BUBBLE),			"QDN_BUBBLE",			""},
  { CRW(QDN_CONGESTION),		"QDN_CONGESTION",		""},
  { CRW(UDN_DEMUX_STALL),		"UDN_DEMUX_STALL",		""},
  { CRW(IDN_DEMUX_STALL),		"IDN_DEMUX_STALL",		""},

  { CRW(CBOX_QUIESCED),			"CBOX_QUIESCED",		""},
  { CRW(CBOX_FULL_STALL),		"CBOX_FULL_STALL",		""},
  { CRW(PFB_HIT_IN_PFB),		"PFB_HIT_IN_PFB",		""},
  { CRW(PFB_HIT),			"PFB_HIT",			""},
  { CRW(CBOX_RESP),			"CBOX_RESP",			""},
  { CRW(CBOX_REQ),			"CBOX_REQ",			""},
  { CRW(ICACHE_FILL_PEND),		"ICACHE_FILL_PEND",		""},
  { CRW(ICACHE_FILL),			"ICACHE_FILL",			""},
  { CRW(WAY_MISPREDICT),		"WAY_MISPREDICT",		""},
  { CRW(COND_BRANCH_PRED_CORRECT),	"COND_BRANCH_PRED_CORRECT",	""},

  { CRW(COND_BRANCH_PRED_INCORRECT),	"COND_BRANCH_PRED_INCORRECT",	""},
  { CRW(COND_BRANCH_NO_PREDICT),	"COND_BRANCH_NO_PREDICT",	""},
  { CRW(USED_RAS),			"USED_RAS",			""},
  { CRW(RAS_CORRECT),			"RAS_CORRECT",			""},
  { CRW(ARB_VALID),			"ARB_VALID",			""},
  { CRW(MDF_WRITE),			"MDF_WRITE",			""},
  { CRW(LDB_READ),			"LDB_READ",			""},
  { CRW(L1_OPCODE_VALID),		"L1_OPCODE_VALID",		""},
  { CRW(L1_OPCODE_LD_VALID),		"L1_OPCODE_LD_VALID",		""},
  { CRW(L1_OPCODE_ST_VALID),		"L1_OPCODE_ST_VALID",		""},

  { CRW(L1_OPCODE_ATOMIC_VALID),	"L1_OPCODE_ATOMIC_VALID",	""},
  { CRW(L1_OPCODE_FLUSH_VALID),		"L1_OPCODE_FLUSH_VALID",	""},
  { CRW(L1_OPCODE_INV_VALID),		"L1_OPCODE_INV_VALID",		""},
  { CRW(L1_OPCODE_FINV_VALID),		"L1_OPCODE_FINV_VALID",		""},
  { CRW(L1_OPCODE_MF_VALID),		"L1_OPCODE_MF_VALID",		""},
  { CRW(L1_OPCODE_PFETCH_VALID),	"L1_OPCODE_PFETCH_VALID",	""},
  { CRW(L1_OPCODE_WH64_VALID),		"L1_OPCODE_WH64_VALID",		""},
  { CRW(L1_OPCODE_DTLBPR_VALID),	"L1_OPCODE_DTLBPR_VALID",	""},
  { CRW(L1_OPCODE_FWB_VALID),		"L1_OPCODE_FWB_VALID",		""},

  { CRW(L1_OPCODE_LD_NON_TEMPORAL_VALID),	"L1_OPCODE_LD_NON_TEMPORAL_VALID",	""},
  { CRW(L1_OPCODE_ST_NON_TEMPORAL_VALID),	"L1_OPCODE_ST_NON_TEMPORAL_VALID",	""},
  { CRW(L2_OPCODE_LD_VALID),		"L2_OPCODE_LD_VALID",		""},
  { CRW(L2_OPCODE_ST_VALID),		"L2_OPCODE_ST_VALID",		""},
  { CRW(L2_OPCODE_ATOMIC_VALID),	"L2_OPCODE_ATOMIC_VALID",	""},
  { CRW(L2_OPCODE_FLUSH_VALID),		"L2_OPCODE_FLUSH_VALID",	""},
  { CRW(L2_OPCODE_INV_VALID),		"L2_OPCODE_INV_VALID",		""},
  { CRW(L2_OPCODE_FINV_VALID),		"L2_OPCODE_FINV_VALID",		""},
  { CRW(L2_OPCODE_MF_VALID),		"L2_OPCODE_MF_VALID",		""},
  { CRW(L2_OPCODE_PFETCH_VALID),	"L2_OPCODE_PFETCH_VALID",	""},

  { CRW(L2_OPCODE_WH64_VALID),		"L2_OPCODE_WH64_VALID",		""},
  { CRW(L2_OPCODE_FWB_VALID),		"L2_OPCODE_FWB_VALID",		""},
  { CRW(L2_OPCODE_LD_NON_TEMPORAL_VALID),	"L2_OPCODE_LD_NON_TEMPORAL_VALID",	""},
  { CRW(L2_OPCODE_ST_NON_TEMPORAL_VALID),	"L2_OPCODE_ST_NON_TEMPORAL_VALID",	""},
  { CRW(L2_OPCODE_LD_NOFIL_VALID),		"L2_OPCODE_LD_NOFIL_VALID",		""},
  { CRW(L2_OPCODE_LD_NOFIL_NON_TEMPORAL_VALID),	"L2_OPCODE_LD_NOFIL_NON_TEMPORAL_VALID",""},
  { CRW(L2_OPCODE_RDN_VALID),		"L2_OPCODE_RDN_VALID",		""},
  { CRW(L2_OPCODE_QDN_VALID),		"L2_OPCODE_QDN_VALID",		""},
  { CRW(L2_OPCODE_IO_READ_VALID),	"L2_OPCODE_IO_READ_VALID",	""},
  { CRW(L2_OPCODE_IO_WRITE_VALID),	"L2_OPCODE_IO_WRITE_VALID",	""},

  { CRW(L2_OPCODE_I_STREAM_VALID),	"L2_OPCODE_I_STREAM_VALID",	""},
  { CRW(L2_OPCODE_MDF_VALID),		"L2_OPCODE_MDF_VALID",		""},
  { CRW(L2_OPCODE_IREQ_IV_VALID),	"L2_OPCODE_IREQ_IV_VALID",	""},
#endif