* = $20
adc #$25
LDY $C040

label:
    pha
    pla
    jmp label
