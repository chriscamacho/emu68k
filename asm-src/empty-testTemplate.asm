    dc.l    $00100000   * SP and PC
    dc.l    $00000400
    
    rorg     $400

start:
	
		subi.b		#1,d0
		move.b		d0,($A0000)		# output to latch
		move.b    ($A0000),d0   # check it reads back.

		bra.s		start			# main loop

