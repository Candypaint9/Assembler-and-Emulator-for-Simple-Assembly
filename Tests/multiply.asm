; code to multiply two numbers

ldc 0x1000 
a2sp 
adj -2
ldc 0	;storing the sum
stl 0	
ldc 0
stl 1	;storing counter

loop:	ldc num1	
	ldnl 0
	ldl 1
	sub
	brz done 

	ldc num2
	ldnl 0
	ldl 0
	add
	stl 0	

	ldl 1
	ldc 1
	add
	stl 1

	br loop

done:	ldl 0
	ldc sum
	stnl 0
	

num1: 	data 4
num2: 	data 3
sum: 	data 0