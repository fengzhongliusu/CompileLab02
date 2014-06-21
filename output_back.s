.data
.globl main
.text
main:
 addi $sp, $sp, -20
 sw $ra, 0($sp)
 li $t0, 10
 sw $t0, 4($sp)
 lw $t3, 4($sp)
 move $t0, $t3
 sw $t0, 8($sp)
 lw $t1, 4($sp)
 addi $t0, $t1, 5
 sw $t0, 4($sp)
 lw $t1, 4($sp)
 li $t4, 10
 mul $t0, $t1, $t4
 sw $t0, 8($sp)
 lw $t1, 4($sp)
 lw $t2, 8($sp)
 sub $t0, $t1, $t2
 sw $t0, 12($sp)
 lw $t1, 12($sp)
 lw $t2, 4($sp)
 div $t1, $t2
 mflo $t0
 sw $t0, 16($sp)
 lw $t3, 0($sp)
 move $v0, $t3
 jr $ra
