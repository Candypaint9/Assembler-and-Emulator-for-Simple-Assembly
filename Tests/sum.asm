ldc 0x1000         ;Load constant 0x1000 into A register
a2sp 

ldc 0
ldc N
ldnl 0
stl 0 

loop: 	ldl 0
	brz done
	ldl 1
 	add
	stl 1
	adc -1
	stl 0
	br loop 

done: 	ldl 1
	ldc sum
	stnl 0
	HALT

N: 	data 0108
sum: 	data 0