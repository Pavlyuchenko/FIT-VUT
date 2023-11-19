; Autor reseni: Michal Pavlíček xpavlim00
; Pocet cyklu k serazeni puvodniho retezce: 3269
; Pocet cyklu razeni sestupne serazeneho retezce: 3608
; Pocet cyklu razeni vzestupne serazeneho retezce: 483
; Pocet cyklu razeni retezce s vasim loginem: 998
; Implementovany radici algoritmus: Bubble sort
; ------------------------------------------------

.data
; login:          .asciiz "vitejte-v-inp-2023"
; login:          .asciiz "vvttpnjiiee3220---"
; login:          .asciiz "---0223eeiijnpttvv"
login:          .asciiz "xpavlim00"

params_sys5:    .space  8   ; misto pro ulozeni adresy pocatku
                            ; retezce pro vypis pomoci syscall 5
                            ; (viz nize - "funkce" print_string)


.text
main:
        daddi   r13, r0, login   ; adresa login: do r13

        jal    word_length     ; zjisteni delky retezce
        jal     bubble_sort     ; razeni pomoci bubble sort

        jal     print_string

        syscall 0   ; halt

word_length:
        daddi    r6, r6, 1      ; word_length++ 

        lb      r5, login(r6)
        bne     r5, r0, word_length

        jr      r31

bubble_sort:
        daddi   r1, r0, 0
        daddi   r2, r0, 1
        daddi   r5, r0, 0
        daddi   r7, r6, -1

        ; r1: index counter
        ; r2: index counter + 1
        ; r3: stores login[i]
        ; r4: stores login[i+1]
        ; r5: stores if login[i+1] < login[i]
        ; r6: length of login that sets the number of required iterations
        ; r7: keeps the length of login throughout each iteration (so that sorted chars are not checked again)
        ; r8: register for checking if login is already sorted (1 = not sorted, 0 = sorted)
loop:
        lb      r3, login(r1)   ; temp_storage = login[i]
        lb      r4, login(r2)   ; temp_storage_2 = login[i+1]

        slt     r5, r4, r3      ; temp_storage_2 < temp_storage

        beq     r5, r0, skip_swap ; jump to skip_swap if r5 == r0
        daddi   r8, r0, 1       ; not sorted yet
        sb      r4, login(r1)   ; login[i] = temp_storage_2
        sb      r3, login(r2)   ; login[i+1] = temp_storage
skip_swap:
        daddi   r1, r1, 1       ; i++
        daddi   r2, r1, 1       ; j++
        daddi   r7, r7, -1      ; word_length--

        bne     r7, r0, loop    ; jump to loop if login(r1+1) != 0

        daddi   r1, r0, 0       ; i = 0
        daddi   r2, r0, 1       ; j = 1
        daddi   r6, r6, -1      ; counter--
        daddi   r7, r6, 0       ; word_length = counter

        beq     r8, r0, finish_sort ; jump to finish_sort if r8 == r0

        daddi   r8, r0, 0       ; sorted = 0
        bne     r6, r0, loop    ; jump to loop if counter != 0

finish_sort:
        jr      r31


print_string:   ; adresa retezce se ocekava v r13
                sw      r13, params_sys5(r0)
                daddi   r14, r0, params_sys5    ; adr pro syscall 5 musi do r14
                syscall 5   ; systemova procedura - vypis retezce na terminal
                jr      r31 ; return - r31 je urcen na return address
