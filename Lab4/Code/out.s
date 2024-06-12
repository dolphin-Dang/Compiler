.data
_prompt: .asciiz "Enter an integer:"
_ret: .asciiz "\n"
.globl main
.text
read:
	li $v0, 4
	la $a0, _prompt
	syscall
	li $v0, 5
	syscall
	jr $ra

write:
	li $v0, 1
	syscall
	li $v0, 4
	la $a0, _ret
	syscall
	move $v0, $0
	jr $ra

main:
	addi $sp, $sp, -8
	sw $fp, 0($sp)
	sw $ra, 4($sp)
	move $fp, $sp
	addi $sp, $sp, -36
	lw $t0, -4($fp)
	li $t0, 0
	sw $t0, -4($fp)
	lw $t0, -8($fp)
	li $t0, 1
	sw $t0, -8($fp)
	lw $t0, -12($fp)
	li $t0, 0
	sw $t0, -12($fp)
	addi $sp, $sp, -4
	sw $ra, 0($sp)
	jal read
	lw $ra, 0($sp)
	addi $sp, $sp, 4
	lw $t0, -16($fp)
	move $t0, $v0
	lw $t1, -20($fp)
	lw $t2, -16($fp)
	move $t1, $t2
	sw $t1, -20($fp)
label1:
	lw $t1, -12($fp)
	lw $t3, -20($fp)
	bne $t1, $t3, label3
	lw $t4, -24($fp)
	lw $t5, -4($fp)
	lw $t6, -8($fp)
	add $t4, $t5, $t6
	sw $t4, -24($fp)
	lw $t4, -28($fp)
	lw $t7, -24($fp)
	move $t4, $t7
	sw $t4, -28($fp)
	lw $t4, -8($fp)
	move $a0, $t4
	addi $sp, $sp, -4
	sw $ra, 0($sp)
	jal write
	lw $ra, 0($sp)
	addi $sp, $sp, 4
	lw $s0, -4($fp)
	lw $s1, -8($fp)
	move $s0, $s1
	sw $s0, -4($fp)
	lw $s0, -8($fp)
	lw $s2, -28($fp)
	move $s0, $s2
	sw $s0, -8($fp)
	lw $s0, -32($fp)
	lw $s3, -12($fp)
	addi $s0, $s3, 1
	sw $s0, -32($fp)
	lw $s0, -12($fp)
	lw $s4, -32($fp)
	move $s0, $s4
	sw $s0, -12($fp)
	j label1
label2:
	lw $s0, -36($fp)
	li $s0, 0
	sw $s0, -36($fp)
	lw $ra, 4($fp)
	addi $sp, $fp, 8
	lw $s0, -36($fp)
	lw $fp, 0($fp)
	move $v0, $s0
	jr $ra
