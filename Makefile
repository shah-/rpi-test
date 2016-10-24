deb: 00-deb 01-deb 02-deb 03-deb

rel: 00-rel 01-rel 02-rel 03-rel

00-deb:
	gcc -Wall -g -o 00-mouse 00-mouse.c
00-rel:
	gcc -Wall -O2 -s -o 00-mouse 00-mouse.c


01-deb:
	gcc -Wall -g -o 01-keyboard 01-keyboard.c
01-rel:
	gcc -Wall -O2 -s -o 01-keyboard 01-keyboard.c


02-deb:
	gcc -Wall -g -mcpu=cortex-a53 -mfpu=neon -o 02-neon 02-neon.c
02-rel:
	gcc -Wall -O3 -s -mcpu=cortex-a53 -mfpu=neon -o 02-neon 02-neon.c


03-deb:
	gcc -Wall -g -mcpu=cortex-a53 -mfpu=neon -o 03-camera 03-camera.c
03-rel:
	gcc -Wall -O3 -s -mcpu=cortex-a53 -mfpu=neon -o 03-camera 03-camera.c


clean:
	rm -rf 00-mouse 01-keyboard 02-neon 03-camera

asm:
	gcc -c -g -mcpu=cortex-a53 -mfpu=neon 02-neon.c -Wa,-a,-ad -O3 > 02-neon.asm
