stackSize     equ   8*1024

    dc.l      end+stackSize            ; start up SP
;    dc.l      stack
    dc.l      start           ; start up PC
    
; started off as a dreamcast vector table
;        dc.l    0x0             /* Initial Stack Address */
;        dc.l    START           /* Start of program Code */
    dc.l    BUS_E_INT       /* Bus error */
    dc.l    ADRS_E_INT      /* Address error */
    dc.l    INT             /* Illegal instruction */
    dc.l    INT             /* Division by zero */
    dc.l    INT             /* CHK exception */
    dc.l    INT             /* TRAPV exception */
    dc.l    INT             /* Privilage violation */
    dc.l    INT             /* TRACE exception */
    dc.l    INT             /* Line-A emulator */
    dc.l    INT             /* Line-F emulator */
    dc.l    INT             /* Reserved (NOT USED) */
    dc.l    INT             /* Co-processor protocol violation */
    dc.l    INT             /* Format error */
    dc.l    INT             /* Uninitialized Interrupt */
    dc.l    INT             /* Reserved (NOT USED) */
    dc.l    INT             /* Reserved (NOT USED) */
    dc.l    INT             /* Reserved (NOT USED) */
    dc.l    INT             /* Reserved (NOT USED) */
    dc.l    INT             /* Reserved (NOT USED) */
    dc.l    INT             /* Reserved (NOT USED) */
    dc.l    INT             /* Reserved (NOT USED) */
    dc.l    INT             /* Reserved (NOT USED) */
    dc.l    INT             /* Spurious Interrupt */
    dc.l    INT             /* IRQ Level 1 */
    dc.l    INT             /* IRQ Level 2 (EXT Interrupt) */
    dc.l    INT             /* IRQ Level 3 */
    dc.l    INT             /* IRQ Level 4 (VDP Horizontal Interrupt) */
    dc.l    INT             /* IRQ Level 5 */
    dc.l    INT             /* IRQ Level 6 (VDP Vertical Interrupt) */
    dc.l    INT             /* IRQ Level 7 */
    dc.l    INT             /* TRAP #00 Exception */
    dc.l    INT             /* TRAP #01 Exception */
    dc.l    INT             /* TRAP #02 Exception */
    dc.l    INT             /* TRAP #03 Exception */
    dc.l    INT             /* TRAP #04 Exception */
    dc.l    INT             /* TRAP #05 Exception */
    dc.l    INT             /* TRAP #06 Exception */
    dc.l    INT             /* TRAP #07 Exception */
    dc.l    INT             /* TRAP #08 Exception */
    dc.l    INT             /* TRAP #09 Exception */
    dc.l    INT             /* TRAP #10 Exception */
    dc.l    INT             /* TRAP #11 Exception */
    dc.l    INT             /* TRAP #12 Exception */
    dc.l    INT             /* TRAP #13 Exception */
    dc.l    INT             /* TRAP #14 Exception */
    dc.l    INT             /* TRAP #15 Exception */
    dc.l    INT             /* (FP) Branch or Set on Unordered Condition */
    dc.l    INT             /* (FP) Inexact Result */
    dc.l    INT             /* (FP) Divide by Zero */
    dc.l    INT             /* (FP) Underflow */
    dc.l    INT             /* (FP) Operand Error */
    dc.l    INT             /* (FP) Overflow */
    dc.l    INT             /* (FP) Signaling NAN */
    dc.l    INT             /* (FP) Unimplemented Data Type */
    dc.l    INT             /* MMU Configuration Error */
    dc.l    INT             /* MMU Illegal Operation Error */
    dc.l    INT             /* MMU Access Violation Error */
    dc.l    INT             /* Reserved (NOT USED) */
    dc.l    INT             /* Reserved (NOT USED) */
    dc.l    INT             /* Reserved (NOT USED) */
    dc.l    INT             /* Reserved (NOT USED) */
    dc.l    INT             /* Reserved (NOT USED) */    
    
    
    rorg      $1000
  
INT:
      illegal
      
BUS_E_INT:      ; Bus Error Exception
      illegal

ADRS_E_INT:     ; Address Error Exception
    ; TODO !
    move.w    (A7)+,D0
    move.l    (A7)+,D1    ; missing a word ?
    ; draw bombs here ;)
    move      #end+stackSize,SP
    jmp       start
    

    

varA:
    dc.w	    $0102
varB:
    dc.w      $0304
res1:
    dc.l      0
res2:
    dc.l      0
res3:
    dc.l      0
res4:
    dc.l      0
    
start:

    ;move.l	  #res4,A0
    ;add.l     #1,A0
    ;move.w    (A0),D0      ; do a bus error


    sub.l     #16,A7        ; return space for 4 l words    
    move.w    varB,-(A7)
    move.w    varA,-(A7)    ; reverse order    
    
    jsr       test
    
    move.l    (A7)+,D3
    move.l    (A7)+,D2
    move.l    (A7)+,D1
    move.l    (A7)+,D0
    

done:
    bra       done
    bra       start
        
test:
    link      A6,#-20

    move.l    #$43432121,D0
    move.l    D0,20(A6)

    move.l    #$87876564,D0
    move.l    D0,16(A6)

    move.l    #$87654321,D0
    move.l    D0,12(A6)

    move.l    #$12345678,D0
    move.l    D0,8(A6)
    
    unlk      A6
    add.l     #4,SP
    rts

end:
