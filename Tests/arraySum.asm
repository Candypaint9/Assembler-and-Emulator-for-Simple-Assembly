ldc 0x1000         
a2sp               

adj -3
ldc 0             
stl 1              ;initial sum at SP + 1

ldc array          ; Load base address of the array into A
stl 2              ;store base address of array at SP + 2

ldc N     	
stl 0              ;store counter at SP

loop:
    ldl 0          
    brz end        

    ldl 2          ;load current array address from SP + 2
    ldnl 0         
    
    ldl 1          ; Load current sum
    add            
    stl 1          

    ldc 1         
    ldl 2          ; Load current array address
    add            ; address++
    stl 2         

    ldc -1         
    ldl 0          
    add            ; counter--
    stl 0          

    br loop        

end:
    ldl 1          ;load sum
    ldc result     
    stnl 0         ; storing sum in result
    HALT           

N: data 5                 
array:      data 1
           data 2
           data 3
           data 1
           data 4
                
result:     data 0                
