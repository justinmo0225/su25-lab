.globl f # this allows other files to find the function f

# f takes in two arguments:
# a0 is the value we want to evaluate f at
# a1 is the address of the "output" array (read the lab spec for more information).
# The return value should be stored in a0
.data
values: .word 6, 61, 17, -38, 19, 42, 5     # array

.text
f:
    # Your code here

    addi t0, a0, 3      # -3 to 3 becomes 0 to 6
    slli t1, t0, 2      # makes each value 4 bytes
    la t2, values
    add t3, t1, t2      # temporary to hold t1 + t2
    lw t4, 0(t3)        # t4 = output
    add t5, a1, t1      # placeholder for a1 + t1
    sw t6, 0(t5)

    # This is how you return from a function. You'll learn more about this later.
    # This should be the last line in your program.
    jr ra
