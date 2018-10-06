cmd_/root/alcb/alcb.ko := ld -r -m elf_x86_64 -T ./scripts/module-common.lds --build-id  -o /root/alcb/alcb.ko /root/alcb/alcb.o /root/alcb/alcb.mod.o
