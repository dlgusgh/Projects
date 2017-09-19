starts:
	addiu $s0, $0, 4
	jal apple
	la $t0 maps
	jalr $t0
maps: 
	addi $s1,$0, 4
	mult $s1, $s0
	mflo $s2
	jr $ra	
apple:
	addi $s3, $0, 6
	jr $ra
	
