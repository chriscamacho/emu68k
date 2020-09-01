stackSize     equ   512

;    dc.l      end+stackSize            ; start up SP
    dc.l      stack
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
      jmp   INT
      
BUS_E_INT:      ; Bus Error Exception
      illegal

ADRS_E_INT:     ; Address Error Exception
    ; TODO !
    move.w    (A7)+,D0
    move.l    (A7)+,D1    ; missing a word ?
    ; draw bombs here ;)
    move      #end+stackSize,SP
    jmp       start
    
    dc.l      $a5a5a5a5,$a5a5a5a5,$a5a5a5a5,$a5a5a5a5,$a5a5a5a5,$a5a5a5a5,$a5a5a5a5,$a5a5a5a5
    dc.l      $a5a5a5a5,$a5a5a5a5,$a5a5a5a5,$a5a5a5a5,$a5a5a5a5,$a5a5a5a5,$a5a5a5a5,$a5a5a5a5
    dc.l      $a5a5a5a5,$a5a5a5a5,$a5a5a5a5,$a5a5a5a5,$a5a5a5a5,$a5a5a5a5,$a5a5a5a5,$a5a5a5a5
    dc.l      $a5a5a5a5,$a5a5a5a5,$a5a5a5a5,$a5a5a5a5,$a5a5a5a5,$a5a5a5a5,$a5a5a5a5,$a5a5a5a5
    dc.l      $a5a5a5a5,$a5a5a5a5,$a5a5a5a5,$a5a5a5a5,$a5a5a5a5,$a5a5a5a5,$a5a5a5a5,$a5a5a5a5
    dc.l      $a5a5a5a5,$a5a5a5a5,$a5a5a5a5,$a5a5a5a5,$a5a5a5a5,$a5a5a5a5,$a5a5a5a5,$a5a5a5a5
    dc.l      $a5a5a5a5,$a5a5a5a5,$a5a5a5a5,$a5a5a5a5,$a5a5a5a5,$a5a5a5a5,$a5a5a5a5,$a5a5a5a5
    dc.l      $a5a5a5a5,$a5a5a5a5,$a5a5a5a5,$a5a5a5a5,$a5a5a5a5,$a5a5a5a5,$a5a5a5a5,$a5a5a5a5
    dc.l      $a5a5a5a5,$a5a5a5a5,$a5a5a5a5,$a5a5a5a5,$a5a5a5a5,$a5a5a5a5,$a5a5a5a5,$a5a5a5a5
    dc.l      $a5a5a5a5,$a5a5a5a5,$a5a5a5a5,$a5a5a5a5,$a5a5a5a5,$a5a5a5a5,$a5a5a5a5,$a5a5a5a5
    dc.l      $a5a5a5a5,$a5a5a5a5,$a5a5a5a5,$a5a5a5a5,$a5a5a5a5,$a5a5a5a5,$a5a5a5a5,$a5a5a5a5
    dc.l      $a5a5a5a5,$a5a5a5a5,$a5a5a5a5,$a5a5a5a5,$a5a5a5a5,$a5a5a5a5,$a5a5a5a5,$a5a5a5a5
    dc.l      $a5a5a5a5,$a5a5a5a5,$a5a5a5a5,$a5a5a5a5,$a5a5a5a5,$a5a5a5a5,$a5a5a5a5,$a5a5a5a5
    dc.l      $a5a5a5a5,$a5a5a5a5,$a5a5a5a5,$a5a5a5a5,$a5a5a5a5,$a5a5a5a5,$a5a5a5a5,$a5a5a5a5
    dc.l      $a5a5a5a5,$a5a5a5a5,$a5a5a5a5,$a5a5a5a5,$a5a5a5a5,$a5a5a5a5,$a5a5a5a5,$a5a5a5a5
    dc.l      $a5a5a5a5,$a5a5a5a5,$a5a5a5a5,$a5a5a5a5,$a5a5a5a5,$a5a5a5a5,$a5a5a5a5,$a5a5a5a5
    dc.l      $a5a5a5a5,$a5a5a5a5,$a5a5a5a5,$a5a5a5a5,$a5a5a5a5,$a5a5a5a5,$a5a5a5a5,$a5a5a5a5
stack:    
    dc.l      $FF00FF00,$FF00FF00,$FF00FF00,$FF00FF00,$FF00FF00,$FF00FF00,$FF00FF00,$FF00FF00
varA:
    dc.w	    $0102
varB:
    dc.w      $0304


oops: 
    dc.l      $fffffff0
    
start:

    ;move.l	  #res4,A0
    ;add.l     #1,A0
    ;move.w    (A0),D0      ; do a bus error

    move.w    (varA),-(A7)
    move.w    (varB),-(A7)
    
    move.l    #$7ac71e55,d0   ; to show unharmed after jsr
    move.l    #$beefdead,d1
    
;    movea.l   oops,A7
    jsr       test
    

    

done:
    bra       done
    bra       start
        
test:

    movem.l   d0-d1,-(A7)
    move.w    14(A7),d0       ; +8 saved regs +4 ret address +2 2nd param
    move.w    12(A7),d1       ; +8 saved regs +4 ret address
    
    ; use a stack frame here if extra local vars needed...
    
    movem.l   (A7)+,d0-d1     ; restore regs
    add.l     #4,A7             ; +8 is correct but there is return address
    move.l    -4(A7),(A7)      ; correct for params put return address
    rts
end:
