/* 
   Copyright (C) 2008 Free Software Foundation, Inc.
   Written by Zheng Da.

   This file is part of the GNU Hurd.

   The GNU Hurd is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   The GNU Hurd is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with the GNU Hurd; see the file COPYING.  If not, write to
   the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.  */

/*
 * This file translates a string into a bpf program.
 * The BPF structures are defined in both of bpf.h and pcap-bpf.h
 * Hopefully, there is no conflict between them.
 * This file uses the BPF definition in pcap-bpf.h.
 */

#include <pcap.h>
#include <stdlib.h>
#include <string.h>

#include "util.h"

#define NETF_IN 0x1
#define NETF_OUT 0x2
#define NETF_NBPA 10
#define NETF_BPF (1 << NETF_NBPA)

/* This function translates the bpf program
 * from the string into the instructions. */
struct bpf_insn *trans_filter_program (char *str, int send, int *filter_len)
{
  struct bpf_program program;
  struct bpf_insn *insn;
  pcap_t *pcap;
  int err;

  debug ("Compiling the bpf program: %s.\n", str);
  pcap = pcap_open_dead (DLT_EN10MB, 1500);
  if (pcap == NULL)
    return NULL;
  err = pcap_compile (pcap, &program, str, 1, 0);
  if (err < 0) 
    {
      debug ("pcap_compile: %s\n", pcap_geterr (pcap));
      pcap_close (pcap);
      return NULL;
    }

  debug ("Finish compiling the bpf program, get %d bpf instructions.\n",
	 program.bf_len);
  insn = (struct bpf_insn *) malloc ((program.bf_len + 1) * sizeof (*insn));
  /* Clear the first instruction. */
  memset (insn, 0, sizeof (*insn));
  if (send)
    insn->code = NETF_OUT | NETF_BPF;
  else
    insn->code = NETF_IN | NETF_BPF;
  memcpy (insn + 1, program.bf_insns, program.bf_len * sizeof (*insn));
  *filter_len = ((program.bf_len + 1) * sizeof (*insn)) / sizeof (short);
  debug ("%d bpf instructions, the length of filters is %d words\n", 
	 program.bf_len, *filter_len);
  pcap_freecode (&program);
  pcap_close (pcap);

  return insn;
}
