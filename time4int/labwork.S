  # labwork.S
  # Written 2015-2017 by F Lundevall
  # Skeleton file for IS1200/IS1500 lab 1.
  # The macros PUSH and POP are in the public domain.
  # Please add your own code at the end of the file.

  #
  # Please keep the two macros PUSH and POP unchanged
  #
.macro	PUSH reg
	addi	$sp,$sp,-4
	sw	\reg,0($sp)
.endm

.macro	POP reg
	lw	\reg,0($sp)
	addi	$sp,$sp,4
.endm
  #
  # Please add your own code below this line
  #
   # You can add your own code here
  #
.global enable_interrupts
.global delay
.global hexasc
.global time2string
.global PUSH
.global POP
.data
.text
.set noat



hexasc:
  lui $at, 0x0000       #temporary mask of a0.
  ori $t0, $at, 0x000F
  and $t0,$t0,$a0   #temporary mask of a0.  
  slti $t1, $t0, 0xA   #If masked-input < A = 1, else 0.
  bne  $t1, $0, number   #Branch to number if not 0 (Input < A). 
  nop
  addi $v0,$t0,0x37 #If character, add ASCII-A - 10. 
  jr $ra       #Jump back to $ra.
  nop     #Just in case. Can move addi here?

number: 
  addi $v0, $t0, 0x30 #If number, add ASCII-0.
  jr $ra      #Return to $ra.
  nop       #Just in case. Can move addi here?
  
delay: 
  li $t1,4711   #Constant to loop for delay.

  #$a0 is input from calling function (int ms), which is ms to delay. Called ms-counter below.
while:  
  beq $a0,$0,done # If ms-counter == 0.
  nop
  addi $a0,$a0,-1 # Reduce ms-counter by one. 
  li $t0,0    # i = 0;

for:
  beq $t1,$t0,while #if i == constant.
  nop
  addi $t0,$t0,1 # i = i + 1;
  j for   #for looping. Loop until i-counter is equal with given constant.
  nop 
  
done:
  jr $ra  #Jump back to calling function.
  nop



time2string:
  #Register $a0 contains the adress of an area in memory, suitably large for the output from time2string.
  #The 16 LSB of $a1 contains time-info, organized as four NBCD-coded digits of 4 bits each.
  #All other bits in register $a1 can have any value and must be ignored.
  #Register $a0 can contain the address 0x100100017, and register $a1 can contain the value 0x00001653.
  

  # A colon character (ASCII 0x3A). Between numbers. 
  # Null at end.

  
  #PUSH SAVED TEMPORARY.
  PUSH $ra
  PUSH $s1
  PUSH $s2
  PUSH $s3
  PUSH $s4
  PUSH $s5
  PUSH $s6
  PUSH $s7
  
  # $a0 is saved for output, $a1 contains actual time-info. 
  # Need to swap to fit Hexasc.
  # $a0 = time-info, $a1 = output.
    move $s4,$a0 #$a0 saved to $s4
   move $a0,$a1  
    move $a1,$s4
  
    li $s5, 12 #Ammount to shift first loop, $5 changes each loop.
  li $s6, 8  #Ammount to shift for colon.
  li $s7, 4
  
loop:
  move $s1,$a0   #$a0 saved to $s1. (Time-info to $s1).
    srlv $a0,$a0,$s5 #Shift right to get minutes or seconds.
  
  #If $s5 is == 0, end loop.
    beq $s5,$0,nine
    nop
    
    jal hexasc       #$ra from hexasc refers here.
    nop
    sb $v0,0($a1)       #Store result from Hexasc to $a1 (temp-output).
    addi $a1,$a1,1      #Next address to write to.
  
  #If $5 is == 8, insert colon.
    beq $s5,$s6, colon  #Jump to colon function.
    nop
  

  
  #Jump to next NBCD integer and loop.
    addi $s5,$s5,-4     #Shift to next 4 bits.
  move $a0,$s1      #Restore $a0 to original state.
  
  
    j loop        #Jump back to loop.
    nop

colon: 
    addi $s2,$0,0x3A  #Insert ASCII-colon.
   sb $s2,0($a1)     #Store colon to $a1 (temp-output).
    addi $a1,$a1,1    # Next in line to write to.
   addi $s5,$s5, -4  #Shift byte.
    move $a0,$s1    #Restore $a0
    j loop      #Jump back to loop.
    nop

end_loop:
    addi $s2,$0,0x00 # Null Byte.
  sb $s2,0($a1)    #Store null-byte into $a1 (temp-output).
  move $a0,$s1    #Restore $a0
  
    move $a1,$s1 #Restore $a1 
    move $a0,$s4 #Restore $a0
    
  #POP SAVED TEMPORARY.
  POP $s7
  POP $s6
  POP $s5
  POP $s4
  POP $s3
  POP $s2
  POP $s1  
  POP $ra
  jr  $ra     #Jump back to calling function. 
  nop

  
      
nine:
   move $t8, $a0
   andi $a0,$a0,0x0000000F  
   slti $t6, $a0, 9  #if input < 9 = 1
   bne  $t6, $0, notnine
   nop
   #0x4e = N, 0x49 = I, 0x4e = N, 0x45 = E.
   andi $v0, 0
     addi $v0, $v0, 0x4e #N
     #addi $a1,$a1,
     sb   $v0, 0($a1)
     andi $v0, 0
         addi $v0, $v0, 0x49 #I
         addi $a1,$a1,1
         sb   $v0, ($a1)
         andi $v0, 0 
         addi $v0, $v0, 0x4e #N
         addi $a1,$a1,1
         sb   $v0, 0($a1)
         andi $v0, 0 
         addi $v0, $v0, 0x45 #E
         addi $a1,$a1,1
         sb   $v0, 0($a1)
         addi $a1,$a1,1
         move $a0, $t8
         j end_loop
         nop

notnine:  
  jal hexasc       #$ra from hexasc refers here.
    nop
    sb $v0,0($a1)       #Store result from Hexasc to $a1 (temp-output).
    addi $a1,$a1,1      #Next address to write to.
    
    j end_loop        #Jump back to loop.
    nop
  
enable_interrupts:
  ei 
  jr $ra
  nop  
