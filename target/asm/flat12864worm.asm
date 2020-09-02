    dc.l    end+1024            ; start up SP
    dc.l    start           ; start up PC

; worm!

FBUF    EQU   $A0000    ; memory mapped hw devices
VBL     EQU   $B0000
JOY     EQU   $C0000

    
    rorg     $400
    
    ; variables
frame:
    dc.l      0
score:
    dc.b      0,0
scoreInc:
    dc.b      0,1

    even
start:

    addq.l    #1,(frame)        ; increment the frame counter

    move.l    #insert+8,A0      ; least significant nibble
    move.l    (frame),D0
    moveq.l   #7,D1             ; nibbles-1
    jsr       putA


    move.l    #frameMess,A0      
    moveq     #7,D0
    moveq     #56,D1


    jsr       print4            ; print the modified message
    
 

       
    
    move.l    #score+2,A0
    move.l    #scoreInc+2,A1
    ABCD      -(A1),-(A0)
    ABCD      -(A1),-(A0)       ; BCD addition
    
    move.l    #scoreInsert+4,A0
    moveq     #0,D0
    move.w    (score),D0
    moveq.l   #3,D1
    jsr       putA
    
    move.l    #scoreMess,A0
    moveq     #1,D0
    moveq     #56,D1
    jsr       print4
    


; bottom of main loop wait for "vbl", clear screen

; wait for the VBL plugin to change
; this is like waiting for a vertical blank...
    move.b    ($B0000),D5
waitforit:
    move.b    ($B0000),D6
    cmp.b     D5,D6
    beq       waitforit


; clear the frame buffer
    move.l    #FBUF,A0
cls:
    move.w    #$400,D0
clsloop:                        
    move.b    #0,(A0)+          
    dbra      D0,clsloop

		bra.w		  start			        ; main loop



;
;
;   put a value into a string 
;
;
;   A0    address of least significant nibble in message
;   D0    the value
;   D1    the number of nibbles required - 1

putA:

fl:
    move.l    D0,D2
    and.l     #$f,D2            ; isolate next nibble
    
    cmp.b     #10,D2              ; 0-9 is okay
    blt       putAok
    add.b     #7+32,D2            ; to get to a-f
putAok:
    add.b     #48,D2              ; make ascii
    move.b    D2,-(A0)            ; insert into message
    
    lsr.l     #4,D0             ; next nibble
    dbra      D1,fl

    rts

;
;
;   plot a point in the framebuffer
;


; d0 x      x coord in chars
; d1 y      y coord in lines
; A0        address of string    
print4:
    movem.l   d3-d6/a1-a2,-(sp)
    
    move.l    #FBUF,A2
    move.l    #font4x8,A1
    
    lsl.w     #4,d1
    add.w     d1,d0

    moveq.l   #0,D5
    moveq.l   #0,D6

print4loop:
    moveq     #0,D3
    move.b    (A0)+,D3          ; D3 next char to print
    cmp.b     #0,D3             ; is it a zero character
    beq       donePrint4       ; if so message complete!
    sub.b     #32,D3            ; char data starts at ascii 32
    lsl.w     #3,D3             ; 8 lines per char

    moveq     #0,D4
    move.b    (A0)+,D4          ; 2nd char of pair
    cmp.b     #0,D4
    bne.s     print42ok
    move.w    #32,D4            ; D4 is second char in pair
print42ok:
    sub.b     #32,D4            ; char data starts at ascii 32
    lsl.w     #3,D4             ; 8 lines per char
 

    
    
    
    move.b    0(A1,D3.w),D5     ; left of pair
    move.b    0(A1,D4.w),D6     ; right of pair
    lsl.b     #4,D5
    or.b      D5,D6
    move.b    D6,0(A2,D0.w)
    
    move.b    1(A1,D3.w),D5     ; left of pair
    move.b    1(A1,D4.w),D6     ; right of pair
    lsl.b     #4,D5
    or.b      D5,D6
    move.b    D6,16(A2,D0.w)
    
    move.b    2(A1,D3.w),D5     ; left of pair
    move.b    2(A1,D4.w),D6     ; right of pair
    lsl.b     #4,D5
    or.b      D5,D6
    move.b    D6,32(A2,D0.w)
    
    move.b    3(A1,D3.w),D5     ; left of pair
    move.b    3(A1,D4.w),D6     ; right of pair
    lsl.b     #4,D5
    or.b      D5,D6
    move.b    D6,48(A2,D0.w)
    
    move.b    4(A1,D3.w),D5     ; left of pair
    move.b    4(A1,D4.w),D6     ; right of pair
    lsl.b     #4,D5
    or.b      D5,D6
    move.b    D6,64(A2,D0.w)
    
    move.b    5(A1,D3.w),D5     ; left of pair
    move.b    5(A1,D4.w),D6     ; right of pair
    lsl.b     #4,D5
    or.b      D5,D6
    move.b    D6,80(A2,D0.w)
    
    move.b    6(A1,D3.w),D5     ; left of pair
    move.b    6(A1,D4.w),D6     ; right of pair
    lsl.b     #4,D5
    or.b      D5,D6
    move.b    D6,96(A2,D0.w)
    
    move.b    7(A1,D3.w),D5     ; left of pair
    move.b    7(A1,D4.w),D6     ; right of pair
    lsl.b     #4,D5
    or.b      D5,D6
    move.b    D6,112(A2,D0.w)

    ;cmp       #0,d4
    ;beq       donePrint4
    
    add.l     #1,D0 ; destination offset

    bra.w     print4loop
donePrint4:
    movem.l   (sp)+,d3-d6/a1-a2 
    rts
    
;print4:
;    movem.l   d2-d4/a1-a2,-(A7)
;    
;    move.l    A0,A2
;    move.l    #FBUF,A0
;    move.l    #font4x8,A1
;    lsl.w     #1,D0
;    lsl.w     #4,D1
;    add.w     D1,D0             ; screen offset in D0
;    
;print4loop:
;    moveq     #0,D1
 ;   move.b    (A2)+,D1          ; D1 next char to print
;    cmp.b     #0,D1             ; is it a zero character
;    beq.w     donePrint4       ; if so message complete!
;    sub.b     #32,D1            ; char data starts at ascii 32
;    lsl.w     #3,D1             ; 8 lines per char
;    moveq     #0,D2
;    move.b    (A2)+,D2          ; 2nd char of pair
;    cmp.b      #0,D2
;    bne.s     print42ok
;    move.b    #32,D2            ; D2 is second char in pain
;print42ok:
;    sub.b     #32,D2            ; char data starts at ascii 32
;    lsl.w     #3,D2             ; 8 lines per char
    
    
;    move.b    0(A1,D1.w),D3     ; left of pair
;    move.b    0(A1,D2.w),D4     ; right of pair
;    lsl.b     #4,D3
;    or.b      D3,D4
;    or.b    D4,0(A0,D0.w)
;    
;    move.b    1(A1,D1.w),D3     ; left of pair
;    move.b    1(A1,D2.w),D4     ; right of pair
;    lsl.b     #4,D3
;    or.b      D3,D4
;    or.b    D4,16(A0,D0.w)
;    
;    move.b    2(A1,D1.w),D3     ; left of pair
;    move.b    2(A1,D2.w),D4     ; right of pair
;    lsl.b     #4,D3
;    or.b      D3,D4
;    or.b    D4,32(A0,D0.w)
;    
;    move.b    3(A1,D1.w),D3     ; left of pair
;    move.b    3(A1,D2.w),D4     ; right of pair
;    lsl.b     #4,D3
;    or.b      D3,D4
;    or.b    D4,48(A0,D0.w)
;    
;    move.b    4(A1,D1.w),D3     ; left of pair
;    move.b    4(A1,D2.w),D4     ; right of pair
;    lsl.b     #4,D3
;    or.b      D3,D4
;    or.b    D4,64(A0,D0.w)
;    
;    move.b    5(A1,D1.w),D3     ; left of pair
;    move.b    5(A1,D2.w),D4     ; right of pair
;    lsl.b     #4,D3
;    or.b      D3,D4
;    or.b    D4,80(A0,D0.w)
    
;    move.b    6(A1,D1.w),D3     ; left of pair
;    move.b    6(A1,D2.w),D4     ; right of pair
;    lsl.b     #4,D3
;    or.b      D3,D4
;    or.b    D4,96(A0,D0.w)
;    
;    move.b    7(A1,D1.w),D3     ; left of pair
;    move.b    7(A1,D2.w),D4     ; right of pair
;    lsl.b     #4,D3             
;    or.b      D3,D4
;    or.b    D4,112(A0,D0.w)

    
;    add.l     #1,D0 ; destination offset

;    bra.w     print4loop
;donePrint4:

;    movem.l   (A7)+,d2-d4/a1-a2
    
;    rts
    
scoreMess:
    dc.b    "Score:"
scoreInsert:
    dc.b    "----",0,0
    even
frameMess:
    dc.b    "Frame:"
insert:
    dc.b    "--------",0,0
    even
here:
    dc.l    $deadbeef
    
    EVEN                        ; in case strings are odd length!

; TODO incbin !
; path from the point of view of the makefile above...
    include "./target/asm/fonts.inc"

end:                            ; for stack location calculation
