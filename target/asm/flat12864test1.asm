    dc.l    end+1024            ; start up SP
    dc.l    start           ; start up PC

; demonstrates the 128x64 frame buffer display and the VBL plugin
; this got a bit out of hand and some sections of code rely
; on certain registers not getting changed... not terribly good practice!
    
    rorg     $400
frame:
    dc.l      0
ypos:
    dc.w      2                 ; y pos of small text
start:

    move.l    #$A0000,A0        ; address of 128x64x1 output in A0
    move.l    #font8x8,A1        ; font data in A1
    
    moveq.l   #2,D0             ; x-coord D0 (in chars)
    moveq.l   #24,D1            ; y-coord D1 (in lines)
    lsl.w     #4,D1             ; y*16
    add.w     D1,D0             ; screen offset in D0
    
    move.l    #message,A2        ; start of message in A2
    

print8x:
    moveq     #0,D1
    move.b    (A2)+,D1          ; D1 next char to print
    cmp.b     #0,D1             ; is it a zero character
    beq.s     donePrint8x       ; if so message complete!
    sub.b     #32,D1            ; char data starts at ascii 32
    lsl.w     #3,D1             ; 8 lines per char
    
    move.b    0(A1,D1.w),0(A0,D0.w)
    move.b    1(A1,D1.w),16(A0,D0.w)
    move.b    2(A1,D1.w),32(A0,D0.w)
    move.b    3(A1,D1.w),48(A0,D0.w)
    move.b    4(A1,D1.w),64(A0,D0.w)
    move.b    5(A1,D1.w),80(A0,D0.w)
    move.b    6(A1,D1.w),96(A0,D0.w)
    move.b    7(A1,D1.w),112(A0,D0.w) ; neat
    
    add.l     #1,D0
    bra.s     print8x
donePrint8x:

    move.l    #font4x8,A1
    
    
    moveq     #8,D0             ; D0 x-coord in chars
    moveq     #0,d3
    move.b    D0,d3
    andi.b    #1,d3             ; d3 is shift flag
    lsr       #3,D0             ; D0 /8 offset

    
    move.w    (ypos),D1         ; get the ypos
    addi.w    #1,D1             ; add 1
    cmp.w     #56,D1            ; if >56
    ble       yok
    moveq.w   #0,D1             ; reset to 0
yok:
    move.w    D1,ypos           ; save ypos

    lsl.w     #4,D1
    add.w     D1,D0             ; screen offset in D0

    move.l    #message2,A2        ; start of message in A2
    
print4:
    moveq     #0,D1
    move.b    (A2)+,D1          ; D1 next char to print
    cmp.b     #0,D1             ; is it a zero character
    beq.w     donePrint4       ; if so message complete!
    sub.b     #32,D1            ; char data starts at ascii 32
    lsl.w     #3,D1             ; 8 lines per char
    moveq     #0,D2
    move.b    (A2)+,D2          ; 2nd char of pair
    cmp.b      #0,D2
    bne.s     print42ok
    move.b    #32,D2            ; D2 is second char in pain
print42ok:
    sub.b     #32,D2            ; char data starts at ascii 32
    lsl.w     #3,D2             ; 8 lines per char
    
    
    move.b    0(A1,D1.w),D3     ; left of pair
    move.b    0(A1,D2.w),D4     ; right of pair
    lsl.b     #4,D3
    or.b      D3,D4
    move.b    D4,0(A0,D0.w)
    
    move.b    1(A1,D1.w),D3     ; left of pair
    move.b    1(A1,D2.w),D4     ; right of pair
    lsl.b     #4,D3
    or.b      D3,D4
    move.b    D4,16(A0,D0.w)
    
    move.b    2(A1,D1.w),D3     ; left of pair
    move.b    2(A1,D2.w),D4     ; right of pair
    lsl.b     #4,D3
    or.b      D3,D4
    move.b    D4,32(A0,D0.w)
    
    move.b    3(A1,D1.w),D3     ; left of pair
    move.b    3(A1,D2.w),D4     ; right of pair
    lsl.b     #4,D3
    or.b      D3,D4
    move.b    D4,48(A0,D0.w)
    
    move.b    4(A1,D1.w),D3     ; left of pair
    move.b    4(A1,D2.w),D4     ; right of pair
    lsl.b     #4,D3
    or.b      D3,D4
    move.b    D4,64(A0,D0.w)
    
    move.b    5(A1,D1.w),D3     ; left of pair
    move.b    5(A1,D2.w),D4     ; right of pair
    lsl.b     #4,D3
    or.b      D3,D4
    move.b    D4,80(A0,D0.w)
    
    move.b    6(A1,D1.w),D3     ; left of pair
    move.b    6(A1,D2.w),D4     ; right of pair
    lsl.b     #4,D3
    or.b      D3,D4
    move.b    D4,96(A0,D0.w)
    
    move.b    7(A1,D1.w),D3     ; left of pair
    move.b    7(A1,D2.w),D4     ; right of pair
    lsl.b     #4,D3
    or.b      D3,D4
    move.b    D4,112(A0,D0.w)

    
    add.l     #1,D0 ; destination offset

    bra.w     print4
donePrint4:

; wait for the VBL plugin to change
; this is like waiting for a vertical blank...
    move.b    ($B0000),D5
waitforit:
    move.b    ($B0000),D6
    cmp.b     D5,D6
    beq       waitforit

; clear the frame buffer
cls:
    move.w    #$400,D7
clsloop:                        ; TODO !
    move.b    #0,(A0)+          ; reset at top of loop
    dbra      D7,clsloop

; frame++    
    move.l    (frame),D7
    addq.l    #1,D7
    move.l    D7,(frame)

; put the frame value into the ascii message
    moveq.l   #7,D5
    move.l    #insert+8,A3      ; least significant nibble
fl:
    move.l    D7,D6
    and.l     #$f,D6            ; isolate next nibble
    
    cmp     #10,D6              ; 0-9 is okay
    blt     putAok
    add.b   #7+32,D6               ; to get to a-f
putAok:
    add.b   #48,D6              ; make ascii
    move.b  D6,-(A3)            ; insert into message
    
    lsr.l     #4,D7             ; next nibble
    dbra      D5,fl
    
		bra.w		  start			        ; main loop
    

    rts
    
message:
    dc.b    "Hello World!",0
message2:
    dc.b    "Frame Number: "
insert:
    dc.b    "--------",0

    
    EVEN                        ; in case strings are odd length!

; TODO incbin !
; path from the point of view of the makefile above...
    include "./target/asm/fonts.inc"

end:                            ; for stack location calculation
