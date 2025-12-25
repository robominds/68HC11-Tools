*************************************************************
*							    *
* Before receiving the s records, a code byte is received   *
* from the host. i.e.:                                      *
*		      ascii 'P' for Program		    *
*		      ascii 'V' for verify		    *
*                                                           *
* Data transfer is through the sci, configured for 8 data   *
* bits, 9600 baud.                                          *
*							    *
*  Changes:						    *
*  02-Apr-1995 Added pshb before verify 		    *
*							    *
*************************************************************
*
* registers
*
#include regf1cln.inc(#$00,#$0f)
*
* constants
*
tdre	 equ	$80
rdrf	 equ	$20
mda	 equ	$20
smod	 equ	$40
ms10	 equ	10000/3	      10ms delay with 8mhz xtal.
us500	 equ	500/3	      500us delay.
*
vcsctl	 equ	$07
vcsgsiz  equ	$00
vcsgadr  equ	$00
*
* variables. note: they overwrite initialisation code!!!!
*
	 org	$0000
opt	 rmb	1
record	 rmb	1
type	 rmb	1
byte	 rmb	1
*
* program
	 org	$0000
	 lds	#$00ff
	 ldx	#$1000	      offset for control registers.
	 clr	_sccr1,x	      initialise sci for 8 data bits, 9600 baud
	 ldd	#$300c
	 staa	_baud,x
	 stab	_sccr2,x
	 ldaa	#$00
	 staa	_bprot,x
	 ldaa	#$20
	 staa	_ddrg,x
	 ldaa	#vcsgadr
         staa   _csgadr,x
         ldaa   #vcsgsiz
         staa   _csgsiz,x
         ldaa   #vcsctl
         staa   _csctl,x
	 bclr	_portg,x #$20
	 bset	_hprio,x #mda  force special test mode first,
	 bclr	_hprio,x #smod and then expanded mode. (from bootstrap mode)
*
readopt  bsr	rwritec	      then check control byte for external or internal
	 subb	#'P'
	 stab	opt
*
load	 bsr	rwritec
	 stab	record
	 cmpb	#'S'
	 bne	load
	 bsr	rwritec
	 stab	type
	 pshb
	 bsr	rwritec
	 tba
	 psha		      save byte counter
	 bsr	rwritec	      read ms byte of address
	 tba		      and put it in ms byte of accd
	 bsr	rwritec	      now read ls byte of address into ls byte of accd
	 xgdy		      put load address in y
	 pula		       restore byte counter
	 suba	#2
	 pulb
	 cmpb	#'9'
	 bne	load1
load9	 bsr	rwritec	      now discard remaining bytes,
	 deca		      including checksum.
	 bne	load9
	 cpy	#0	      if execution address =0 then
	 beq	*	      hang up else
	 jmp	0,y	      jump to it!
*
load1	 bsr	readc
	 deca
	 beq	loadend
	 pshb
	 ldab	opt
	 bne	verify
	 pulb
	 stab	byte
	 bsr	prgbyte
	 pshb
verify	 pulb
	 ldab	0,y
	 bsr	writec
	 iny
	 bra	load1
loadend  bsr	writec
	 bra	load
*
readc	 brclr	_scsr,x #rdrf *
	 ldab	_scdr,x	      read next char
	 rts
rwritec	 brclr	_scsr,x #rdrf *
	 ldab	_scdr,x	      read next char
writec	 brclr	_scsr,x #tdre *
	 stab	_scdr,x	      and echo it back to host.
	 rts		      return with char in accb.
*
prgbyte  cmpy	#.config
	 beq	prog
	 cmpy	#$fe00
	 bpl	prog
*
	 bset	_portg,x #$20
	 stab	0,y	      else just store byte at address.
	 bclr	_portg,x #$20
	 ldab	#us500
wait1	 decb		      then wait 500us max.
	 bne	wait1
datapoll ldab	0,y	      now either wait for completion of programming
	 eorb	byte	      cycle by testing ms bit of last data written to
	 bne	datapoll
	 rts
*
prog	 psha		      save acca.
	 ldaa	#$16	      default to byte erase mode
	 cpy	#.config      if byte's address is config then use
	 bne	proga
	 pshb
	 ldab	.config
	 stab	byte
	 pulb
	 ldaa	#$06	      bulk erase, to allow for a1 & a8 as well as a2.
proga	 bsr	program	      now erase byte, or entire memory + config.
	 ldaa	#2
	 bsr	program	      now program byte.
progx	 pula		      restore acca
	 rts
*
program  staa	_pprog,x      enable internal addr/data latches.
	 stab	0,y	      write to required address
	 inc	_pprog,x      enable internal programming voltage
         pshx
	 ldx	#ms10	      and wait 10ms
wait2    dex
	 bne	wait2
         pulx
	 dec	_pprog,x      disable internal programming voltage
	 clr	_pprog,x      release internal addr/data latches
	 rts		      and return
*
         end
