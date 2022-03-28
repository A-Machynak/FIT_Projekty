; Vernamova sifra na architekture DLX
; Augustin Machynak xmachy02
; registry: r4-r9-r10-r20-r30-r0

        .data 0x04		; zacatek data segmentu v pameti
login:  .asciiz "xmachy02"  	; <-- nahradte vasim loginem
cipher: .space 9 		; sem ukladejte sifrovane znaky (za posledni nezapomente dat 0)

        .align 2		; dale zarovnavej na ctverice (2^2) bajtu
laddr:  .word login		; 4B adresa vstupniho textu (pro vypis)
caddr:  .word cipher		; 4B adresa sifrovaneho retezce (pro vypis)

        .text 0x40		; adresa zacatku programu v pameti
        .global main		; 
	
main:
loop:	nop
	lb r4, login(r9) 	; nacti znak
	slti r10, r4, 97 	; r4 < 97 ? (r10 = 1) : (r10 = 0)
	bnez r10, end		; r10 != 0 ? j end : nop
	nop
	addi r4, r4, 13		; znak + 'm'
	slti r10, r4, 123	; r4 < 122 ? ...
	bnez r10, skipOF1	; r10 != 0 ? ...
	nop
	subi r4, r4, 26

skipOF1:nop
	sb cipher(r9), r4	; store byte
	addi r9, r9, 1 		; posun o 1 znak
	lb r4, login(r9) 	; nacti dalsi znak
	slti r10, r4, 97 	; r4 < 97 ? (r10 = 1) : (r10 = 0)
	bnez r10, end		; r10 != 0 ? j end : nop
	nop
	subi r4, r4, 1		; znak - 'a'
	slti r10, r4, 97	; r4 < 97 ? (r10 = 1) : (r10 = 0)
	beqz r10, skipOF2
	nop
	addi r4, r4, 22

skipOF2:nop
	sb cipher(r9), r4	; store byte
	addi r9, r9, 1 		; posun o 1 znak
	j loop

end:    nop
	addi r9, r9, 1
	sb cipher(r9), r0
	addi r14, r0, caddr 	; <-- pro vypis sifry nahradte laddr adresou caddr
        trap 5  		; vypis textoveho retezce (jeho adresa se ocekava v r14)
        trap 0 			; ukonceni simulace
