/*suma64sgn.s:	
	Suma N enteros con signo de 32 bits en una plataforma de 32 bits.
*/

# SECCIÓN DE DATOS (.data, variables globales inicializadas)
.section .data
	.macro linea
	#	.int -1,-1,-1,-1
	# 	.int 1,-2,1,-2
	#	.int 1,2,-3,-4
	#	.int 0x7fffffff, 0x7fffffff, 0x7fffffff, 0x7fffffff
		.int 0x80000000, 0x80000000, 0x80000000, 0x80000000
	#	.int 0x04000000, 0x04000000, 0x04000000, 0x04000000
	#	.int 0x08000000, 0x08000000, 0x08000000, 0x08000000
	# 	.int 0xfc000000, 0xfc000000, 0xfc000000, 0xfc000000
	#	.int 0xf8000000, 0xf8000000, 0xf8000000, 0xf8000000
	#	.int 0xf0000000, 0xe0000000, 0xe0000000, 0xd0000000
	.endm

lista:	.irpc i,12345678
			linea
		.endr

longlista:	
	.int (.-lista)/4
resultado:	
	.quad -1
formato:
	.ascii "suma = 0x%08x%08x \n\0"

# SECCIÓN DE CÓDIGO (.text, instrucciones máquina)
.section .text
_start: .global _start		# PROGRAMA PRINCIPAL

	mov    $lista, %ebx		# Dirección del array lista
	mov longlista, %ecx		# Número de elementos a sumar
	call suma				# Llamar a la subrutina suma
	mov %eax, resultado		# Guardar el resultado usando 2 registros
	mov %edx, resultado+4

	push resultado
	push resultado+4
	push $formato
	call printf				# == printf(formato, resultado, resultado)
	add $12,%esp
	

	mov $1, %eax			# exit: servicio 1 kernel
	mov $0, %ebx			# status: código a retornar(0=OK)
	int $0x80				# Llama _exit(0) para que finalice el programa

suma:
	push %esi				# Preservar %esi
	push %edi				# Preservar %edi
	mov $0, %eax			# Poner a 0 acumulador
	mov $0, %edx			# Poner a 0 acarreo
	mov $0, %esi			# Poner a 0 los registros temporales
	mov $0, %edi
	dec %ecx

bucle:
	movl (%ebx,%ecx,4), %eax 	# En cada iteración del bucle, sumar el número de la lista correspondiente a %ebx
								# que se encuentra en la posición %esi-ésima y almacenarlo en %eax
	cdq							# Introducir la extendión de signo de %eax en EDX:EAX
	add %eax, %esi				# Sumar a %esi el contenido de %eax en cada iteracion
	adc %edx, %edi				# Acarreo
	dec %ecx					# Decrementar en uno el registro %ecx en cada iteracion
	jns bucle					# Mientras el valor de %ecx no sea negativo, saltar al comienzo del bucle
	movl %esi, %eax				# Copiar el contenido de %esi en %eax
	movl %edi, %edx				# Copiar el contenido de %edi en %edx

	pop %edi					# Recuperar %edi antiguo
	pop %esi					# Recuperar %esi
	ret							# Retornar de la subrutina