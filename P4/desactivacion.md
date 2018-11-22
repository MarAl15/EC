# Desactivación de mi bomba digital

Ejecutamos el programa _bombaMMAC_ con gdb (`gdb bombaMMAC`) en dos terminales, una para poder ver el programa desemsamblado y otra para poder descifrar la clave.

Por una parte, ejecutamos `disas main` y obtenemos el código en ensamblador.

Buscamos `strncmp@plt` y ponemos un _breakpoint_ en esa línea (`break *0x08048a8d`).

```console
   ...
   0x08048a86 <+219>:	lea    0x2d(%esp),%eax
   0x08048a8a <+223>:	mov    %eax,(%esp)
   0x08048a8d <+226>:	call   0x8048610 <strncmp@plt>
   0x08048a92 <+231>:	test   %eax,%eax
   0x08048a94 <+233>:	je     0x8048a9b <main+240>
   ...
```

Si ejecutamos el programa, nos pedirá una contraseña, le introducimos una cualquiera. Como era de esperarse, se nos para en la instrucción donde hemos puesto el _break_.

Si imprimimos `EAX(print (char *) \$eax)`, se nos mostrará nuestra contraseña, pero modificadas las primeras letras de esta. Si observamos el código, vemos que antes de modificar `EAX`, ha guardado su valor anterior en `ESP+4`. Entonces miramos donde se encuentra `ESP` en pila (en este caso, `0xffffcf80`) y vemos si la contraseña se encuentra en la dirección `(ESP+4)(print (char *) *(0xffffcf80+4))` que nos muestra dependiendo de la contraseña `DivieRtete;)` o `Desactivala:O`. Si saliese desactívala al introducir la contraseña, veriamos que en `(ESP+4)` nos aparecería `DivieRtete;)`.

Una vez conseguida la contraseña, la introducimos, pero nos explota la bomba. Por lo tanto, tenemos que volver a poner el punto de interrupción en el mismo lugar. Si mostramos el valor de `EAX` veremos que la contraseña introducida tiene modificada las primeras letras. Por lo tanto, debemos descubrir cómo lo hace. Si nos fijamos en el código vemos que resta 10, suma 8, resta 1 y suma 4.

```console
   0x08048a43 <+152>:	sub    $0xa,%eax
   0x08048a46 <+155>:	mov    %al,0x2d(%esp)
   0x08048a4a <+159>:	movzbl 0x2f(%esp),%eax
   0x08048a4f <+164>:	add    $0x8,%eax
   0x08048a52 <+167>:	mov    %al,0x2f(%esp)
   0x08048a56 <+171>:	movzbl 0x30(%esp),%eax
   0x08048a5b <+176>:	sub    $0x1,%eax
   0x08048a5e <+179>:	mov    %al,0x30(%esp)
   0x08048a62 <+183>:	movzbl 0x31(%esp),%eax
   0x08048a67 <+188>:	add    $0x4,%eax
```

Aplicando esos cambios a la contraseña obtenemos: `NinjaRtete;)`. Quitamos todos los punto de interrupción con `delete` y comprobamos si esta es correcta. Al ejecutar el programa de nuevo vemos que ya no estalla, sino que nos pide una contraseña. Introducimos cualquiera y volvemos a poner puntos de interrupción.

En `(ESP+18)` se encuentra `253`, y en `EAX` debería encontrarse la introducida por nosotros pero si nos fijamos esta también ha sido modificada, es la mitad de número introducido. Concluyendo que debemos introducir `506` en vez de `253`.

Si comprobamos la contraseña `NinjaRtete;)` y la contraseña `506` veremos que hemos desactivado la bomba como queriamos. 

---

Código en ensamblador:

```console
(gdb) disas main
Dump of assembler code for function main:
   0x080489ab <+0>:	push   %ebp
   0x080489ac <+1>:	mov    %esp,%ebp
   0x080489ae <+3>:	and    $0xfffffff0,%esp
   0x080489b1 <+6>:	sub    $0x40,%esp
   0x080489b4 <+9>:	mov    %gs:0x14,%eax
   0x080489ba <+15>:	mov    %eax,0x3c(%esp)
   0x080489be <+19>:	xor    %eax,%eax
   0x080489c0 <+21>:	movl   $0x0,0x4(%esp)
   0x080489c8 <+29>:	lea    0x1c(%esp),%eax
   0x080489cc <+33>:	mov    %eax,(%esp)
   0x080489cf <+36>:	call   0x8048550 <gettimeofday@plt>
   0x080489d4 <+41>:	call   0x804881d <trensl>
   0x080489d9 <+46>:	movl   $0x80491a8,(%esp)
   0x080489e0 <+53>:	call   0x8048530 <printf@plt>
   0x080489e5 <+58>:	movl   $0x8049204,(%esp)
   0x080489ec <+65>:	call   0x8048530 <printf@plt>
   0x080489f1 <+70>:	movl   $0x804927b,(%esp)
   0x080489f8 <+77>:	call   0x8048530 <printf@plt>
   0x080489fd <+82>:	mov    0x804b058,%eax
   0x08048a02 <+87>:	mov    %eax,0x8(%esp)
   0x08048a06 <+91>:	movl   $0xf,0x4(%esp)
   0x08048a0e <+99>:	lea    0x2d(%esp),%eax
   0x08048a12 <+103>:	mov    %eax,(%esp)
   0x08048a15 <+106>:	call   0x8048540 <fgets@plt>
   0x08048a1a <+111>:	movzbl 0x2d(%esp),%eax
   0x08048a1f <+116>:	cmp    $0x43,%al
   0x08048a21 <+118>:	jg     0x8048a2c <main+129>
   0x08048a23 <+120>:	movzbl 0x2d(%esp),%eax
   0x08048a28 <+125>:	cmp    $0x4e,%al
   0x08048a2a <+127>:	jg     0x8048a36 <main+139>
   0x08048a2c <+129>:	movl   $0x8049296,0x14(%esp)
   0x08048a34 <+137>:	jmp    0x8048a3e <main+147>
   0x08048a36 <+139>:	movl   $0x80492a4,0x14(%esp)
   0x08048a3e <+147>:	movzbl 0x2d(%esp),%eax
   0x08048a43 <+152>:	sub    $0xa,%eax
   0x08048a46 <+155>:	mov    %al,0x2d(%esp)
   0x08048a4a <+159>:	movzbl 0x2f(%esp),%eax
   0x08048a4f <+164>:	add    $0x8,%eax
   0x08048a52 <+167>:	mov    %al,0x2f(%esp)
   0x08048a56 <+171>:	movzbl 0x30(%esp),%eax
   0x08048a5b <+176>:	sub    $0x1,%eax
   0x08048a5e <+179>:	mov    %al,0x30(%esp)
   0x08048a62 <+183>:	movzbl 0x31(%esp),%eax
   0x08048a67 <+188>:	add    $0x4,%eax
   0x08048a6a <+191>:	mov    %al,0x31(%esp)
   0x08048a6e <+195>:	mov    0x14(%esp),%eax
   0x08048a72 <+199>:	mov    %eax,(%esp)
   0x08048a75 <+202>:	call   0x80485c0 <strlen@plt>
   0x08048a7a <+207>:	mov    %eax,0x8(%esp)
   0x08048a7e <+211>:	mov    0x14(%esp),%eax
   0x08048a82 <+215>:	mov    %eax,0x4(%esp)
   0x08048a86 <+219>:	lea    0x2d(%esp),%eax
   0x08048a8a <+223>:	mov    %eax,(%esp)
   0x08048a8d <+226>:	call   0x8048610 <strncmp@plt>
   0x08048a92 <+231>:	test   %eax,%eax
   0x08048a94 <+233>:	je     0x8048a9b <main+240>
   0x08048a96 <+235>:	call   0x804891d <boom>
   0x08048a9b <+240>:	movl   $0x0,0x4(%esp)
   0x08048aa3 <+248>:	lea    0x24(%esp),%eax
   0x08048aa7 <+252>:	mov    %eax,(%esp)
   0x08048aaa <+255>:	call   0x8048550 <gettimeofday@plt>
   0x08048aaf <+260>:	mov    0x24(%esp),%edx
   0x08048ab3 <+264>:	mov    0x1c(%esp),%eax
   0x08048ab7 <+268>:	sub    %eax,%edx
   0x08048ab9 <+270>:	mov    %edx,%eax
   0x08048abb <+272>:	cmp    $0x3c,%eax
   0x08048abe <+275>:	jle    0x8048ac5 <main+282>
   0x08048ac0 <+277>:	call   0x804891d <boom>
   0x08048ac5 <+282>:	movl   $0xfd,0x18(%esp)
   0x08048acd <+290>:	movl   $0x80492b3,(%esp)
   0x08048ad4 <+297>:	call   0x8048530 <printf@plt>
   0x08048ad9 <+302>:	lea    0x10(%esp),%eax
   0x08048add <+306>:	mov    %eax,0x4(%esp)
   0x08048ae1 <+310>:	movl   $0x80492ca,(%esp)
   0x08048ae8 <+317>:	call   0x8048600 <__isoc99_scanf@plt>
   0x08048aed <+322>:	mov    0x10(%esp),%eax
   0x08048af1 <+326>:	mov    %eax,%edx
   0x08048af3 <+328>:	shr    $0x1f,%edx
   0x08048af6 <+331>:	add    %edx,%eax
   0x08048af8 <+333>:	sar    %eax
   0x08048afa <+335>:	mov    %eax,0x10(%esp)
   0x08048afe <+339>:	mov    0x10(%esp),%eax
   0x08048b02 <+343>:	cmp    0x18(%esp),%eax
   0x08048b06 <+347>:	je     0x8048b0d <main+354>
   0x08048b08 <+349>:	call   0x804891d <boom>
   0x08048b0d <+354>:	movl   $0x0,0x4(%esp)
   0x08048b15 <+362>:	lea    0x1c(%esp),%eax
   0x08048b19 <+366>:	mov    %eax,(%esp)
   0x08048b1c <+369>:	call   0x8048550 <gettimeofday@plt>
   0x08048b21 <+374>:	mov    0x1c(%esp),%edx
   0x08048b25 <+378>:	mov    0x24(%esp),%eax
   0x08048b29 <+382>:	sub    %eax,%edx
   0x08048b2b <+384>:	mov    %edx,%eax
   0x08048b2d <+386>:	cmp    $0x3c,%eax
   0x08048b30 <+389>:	jle    0x8048b37 <main+396>
   0x08048b32 <+391>:	call   0x804891d <boom>
   0x08048b37 <+396>:	call   0x8048964 <defused>
   0x08048b3c <+401>:	mov    0x3c(%esp),%ecx
   0x08048b40 <+405>:	xor    %gs:0x14,%ecx
   0x08048b47 <+412>:	je     0x8048b4e <main+419>
   0x08048b49 <+414>:	call   0x8048560 <__stack_chk_fail@plt>
   0x08048b4e <+419>:	leave  
   0x08048b4f <+420>:	ret    
End of assembler dump.
```
