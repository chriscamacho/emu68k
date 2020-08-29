    dc.l    $00100000   * PC and SP
    dc.l    $00000400
    
    rorg     $400

start:

		addi.b		#1,d1			
		subi.b		#1,d0
		
		move		d0,d2
		lsl.b		#4,d2			# high nibble--

		move.b		d1,d3
		andi.b		#$0f,d3			# low nibble++

		or.b		d2,d3			# d3 : high nibble++ low nibble--
		
		move.b		d3,($A0000)		# output to latch

		move.w		#10000,d3		# delay loop for count (or too fast to see!)
dly:

		move.b		($80002),d4		# input from switches W A2-A0 
		btst.b		#$07,d4
		beq.s		noW
		
		andi.w		#7,d4
		lsl.w		#1,d4			# mask address and *2
		move.l		#$B0000,a0
		move.b		($80000),d5
		move.b		d5,0(a0,d4.w)			# TODO syntax for (0,a0,d4) ???
		
noW:
		
		dbra		d3,dly

		bra.s		start			# main loop

