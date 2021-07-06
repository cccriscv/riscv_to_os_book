  lui x1, 0x7FFFF000
  addi x2, x0, helloworld
.puts:
  lb  x3, 0(x2)
  sb  x3, 1(x1)
  addi x2, x2, 1
  bne x3, x0, .puts
  beq x0, x0, .exit

helloworld: .ascii "Hello World!\n"
.exit:
