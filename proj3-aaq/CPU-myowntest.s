starts:
	addiu $s0, $0, 0xffff  #0
	jal apple   #1
	addiu $s2, $0, 32  #2
	jalr $s2  #3
maps: 
	addi $s1,$0, 0x7fff  #4
	mult $s1, $s0   #5
	mfhi $s2   #6
	jr $ra	 #7
apple:
	addi $s3, $0, 6 #8
	addiu $sp, $sp, -4 #9
	sw $ra, 0($sp)#10
	jal maps #11
	lw $ra, 0($sp) #12
	addiu $sp, $sp,4 #13
	jr $ra#14
	