
%macro round_key 1  ; param: rcon
    ; inspired from https://github.com/kmcallister/aesni-examples/blob/master/common.s
    
    ; Let key rounds Kn = X0 X1 X2 X3 and Kn+1 = W0 W1 W2 W3
    ; We want to compute Kn+1 as;
    ;   W0 = SubWord(RotWord(X3)) ^ Rcon ^ X0 = V ^ X0
    ;   W1 = X1 ^ W0 = V ^ X0 ^ X1
    ;   W2 = X2 ^ W1 = V ^ X0 ^ X1 ^ X2
    ;   W3 = X3 ^ W2 = V ^ X0 ^ X1 ^ X2
    ; with V = SubWord(RotWord(X3)) ^ Rcon

    ; xmm1 = Kn = X0 X1 X2 x3

    aeskeygenassist xmm2, xmm1, %1
    ; it computes some values, only the LSB is interesting:
    ; xmm2 = ? ? ? V

    pshufd xmm2, xmm2, 0b11111111
    ; replicate V everywhere
    ; xmm2 = V V V V

    pxor xmm3, xmm3
    ; xmm3 = 0 0 0 0

    shufps xmm3, xmm1, 0b00010000
    ; xmm3 = 0 0 X1 X0

    pxor xmm1, xmm3
    ; xmm1 = X0 X1 X2^X1 X3^X0

    shufps xmm3, xmm1, 0b10001100
    ; xmm3 = 0 X0 X0 X2^X1

    pxor xmm1, xmm3
    ; xmm1 = X0 X1^X0 X2^X1^X0 X3^X2^X1^X0

    pxor xmm1, xmm2
    ; xmm1 = W0 W1 W2 W3 = Kn+1

%endmacro

section .text
    GLOBAL aes128_cipher

aes128_cipher:
    ; plaintext addr is in RDI
    ; key addr is in RSI
    ; cipher addr is in RDX
    ; return value is in RAX

    ; load plaintext and key into xmm registers
    movups xmm0, [rdi]  ; xmm0 => plaintext
    movups xmm1, [rsi]  ; xmm1 => key

    ; add round key
    pxor xmm0, xmm1

    ; round 1
    round_key 0x1
    aesenc xmm0, xmm1

    ; round 2-9
    round_key 0x2
    aesenc xmm0, xmm1
    round_key 0x4
    aesenc xmm0, xmm1
    round_key 0x8
    aesenc xmm0, xmm1
    round_key 0x10
    aesenc xmm0, xmm1
    round_key 0x20
    aesenc xmm0, xmm1
    round_key 0x40
    aesenc xmm0, xmm1
    round_key 0x80
    aesenc xmm0, xmm1
    round_key 0x1B
    aesenc xmm0, xmm1

    ; round 10
    round_key 0x36
    aesenclast xmm0, xmm1

    ; copy result into cipher buffer
    movups [rdx], xmm0

    ret




