ldc 0x1000 
a2sp 

adj -7
ldc N
ldnl 0
stl 0	;storing n
ldc 1
stl 1	;storing i
ldc 0
stl 2	;storing j
ldc array
stl 3	;storing base address in 3

outerLoop:
	ldl 0
	ldl 1
	sub
	brz done

	;making j=i
	ldl 1
	stl 2

	br innerLoop


innerLoop:
	ldl 2
	brz incrementI
	
	;storing arr[j] in 3
	ldl 3	;base array
	ldl 2	;j
	add
	ldnl 0	;arr[j]
	stl 4	
	
	ldl 2
	ldc 1
	sub	;j-1
	ldl 3	;base address
	add	;array+j-1
	ldnl 0	;array[j-1]
	stl 5

	ldl 4
	ldl 5
	sub	;array[j]-array[j-1]
	brlz swap

decrementJ:	
	;drecreasing j
	ldl 2
	adc -1
	stl 2

	br innerLoop

swap:	ldl 3	;array
	ldl 2	;j
	add	;array+j
	stl 6
	ldl 5
	ldl 6
	stnl 0	

	adc -1	;array+j-1
	stl 6
	ldl 4
	ldl 6
	stnl 0	

	br decrementJ
	

incrementI:
	ldl 1
	adc 1
	stl 1
	br outerLoop

done:	HALT

N: data 8
array:  data 6
    	data -4
    	data 3
    	data 7
    	data 2
	data 40
	data -20
	data 0