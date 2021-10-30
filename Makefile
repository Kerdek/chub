bin/chub: bin/stage3/chub
	cp $^ $@

bin/stage1/chub: bin/stage1/chub.o
	ld -o $@ -m elf_x86_64 /usr/lib/x86_64-linux-gnu/crt1.o /usr/lib/x86_64-linux-gnu/crti.o /usr/lib/gcc/x86_64-linux-gnu/10/crtbegin.o -dynamic-linker /usr/lib/x86_64-linux-gnu/ld-linux-x86-64.so.2 $^ /usr/lib/x86_64-linux-gnu/libc.so /usr/lib/gcc/x86_64-linux-gnu/10/libgcc.a /usr/lib/gcc/x86_64-linux-gnu/10/crtend.o /usr/lib/x86_64-linux-gnu/crtn.o
bin/stage2/chub: bin/stage2/chub.o
	ld -o $@ -m elf_x86_64 /usr/lib/x86_64-linux-gnu/crt1.o /usr/lib/x86_64-linux-gnu/crti.o /usr/lib/gcc/x86_64-linux-gnu/10/crtbegin.o -dynamic-linker /usr/lib/x86_64-linux-gnu/ld-linux-x86-64.so.2 $^ /usr/lib/x86_64-linux-gnu/libc.so /usr/lib/gcc/x86_64-linux-gnu/10/libgcc.a /usr/lib/gcc/x86_64-linux-gnu/10/crtend.o /usr/lib/x86_64-linux-gnu/crtn.o
bin/stage3/chub: bin/stage3/chub.o
	ld -o $@ -m elf_x86_64 /usr/lib/x86_64-linux-gnu/crt1.o /usr/lib/x86_64-linux-gnu/crti.o /usr/lib/gcc/x86_64-linux-gnu/10/crtbegin.o -dynamic-linker /usr/lib/x86_64-linux-gnu/ld-linux-x86-64.so.2 $^ /usr/lib/x86_64-linux-gnu/libc.so /usr/lib/gcc/x86_64-linux-gnu/10/libgcc.a /usr/lib/gcc/x86_64-linux-gnu/10/crtend.o /usr/lib/x86_64-linux-gnu/crtn.o

%.o: %.a
	as -o $@ -c $^

bin/stage1/%.a: %.cb
	$(CC) $^ $@
bin/stage2/%.a: %.cb | bin/stage1/chub
	bin/stage1/chub $^ $@
	#diff bin/stage2/$*.a bin/stage1/$*.a
bin/stage3/%.a: %.cb | bin/stage2/chub
	bin/stage2/chub $^ $@

clean:
	rm -f bin/stage*/*