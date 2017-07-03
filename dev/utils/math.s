; Math functions
; Light Games 2017

.globl _div8
; @input:
;   E - Numerator
;   D - Denumarator
; @output:
;   D - Result
;   E - Mod
_div8:
    lda hl, 2(sp)
    ld  a,  (hli)
    ld  e,  a
    ld  a,  (hli)
    ld  d,  a
    ld  l,  #8
    xor a
_div8_0:
    sla e
    rla
    cp  d
    jr  c,  _div8_1
    inc e
    sub d
_div8_1: 
    dec l
    jr  nz, _div8_0
    ld  d,  a
    ret
    
.globl  _mul8
; @input:
;   E - Factor 1
;   D - Factor 2
; @output:
;   E - Result
_mul8:
    lda hl, 2(sp)
    ld  a,  (hli)
    ld  e,  a
    ld  a,  (hli)
    ld  d,  a
    ld  l,  #8
    xor a
_mul8_0:
    rlca
    rlc e
    jr  nc, _mul8_1
    add a,  d
_mul8_1:
    dec l
    jr  nz, _mul8_0
    ld  e,  a
    ret