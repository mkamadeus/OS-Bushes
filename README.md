# K2-OS-6 - Bushes
> Simple operating system called Bushes.

## ⚠ WARNING: Silakan gunakan repository ini sebagai referensi. Jangan di-clone dan dikumpulin buat tugas ya :D

This is a project that is ~currently being~ worked on by 4 people:
* Matthew Kevin Amadeus - 13518035
* Michelle Theresia - 13518050
* Chokyi Ozer - 13518107
* Lionnarta Savirandy - 13518128

## How to run
1. To generate a new `system.img`, run `generate.sh `
2. To compile the `kernel.c` and `kernel.asm` and link it to `system.img`, run `compile.sh`
3. To run the operating system, run in shell:
```bash
bochs -f if2230.config 
```

## Milestone 1 (14/02/20):
1. `kernel.c` made
2. operating system ran
3. interrupt 0x21 implemented
4. boot logo made
5. understanding how interrupt works in operating system
6. separated simple program and ran it to operating system
7. understanding how kernel.asm code works 

## Milestone 2 (forgot):
1. Spek wajib kelar
2. Program tambahan di milestone 1 kelar

## Milestone 3 (forgot):
1. Spek wajib kelar
2. ls - list directory content
3. cp [src] [dest] - copy file/folder to folder
4. mv [src] [dest] - move file/folder to folder
5. mkdir [name] - make new folder
6. rm [name] - remove file/folder
7. cat [name] - print file contents

## Milestone 4 (25/04/20):
1. defrag - Defragment filesystem (Bonus)
2. fraggr - "Frag Grenade" the whole program (Cuz y not)
