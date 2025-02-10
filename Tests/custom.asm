ldc     0xa5    ;checking for lower case hex
ldc     0xA5 	;checking for upper case hex
adc     -5 	;negative value in adc
ldc   +0 	
ldc  -0 	
adj    -071 	;-071 is treated as Octal
adj +022   	;+022 is treated as Octal

Start:     
    ldc -0x10    ;negative hex value
    a2sp          ;a2sp instruction
loop:	add   		;only ad instruction
loop1: 	SET  45 	;set loop1 to the value of 45

ldc loop1 	;ldc value of loop1

var1: data -1000000000 	;setting var1 equal to -1000000000
