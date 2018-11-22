/*suma64uns.s:	
	Suma N enteros sin signo de 32 bits en una plataforma de 32 bits
	sin perder precisión.
*/

# SECCIÓN DE DATOS (.data, variables globales inicializadas)
.section .data
	.macro linea
	# 	.int 1,1,1,1
	# 	.int 2,2,2,2
	#	.int 1,2,3,4
	# 	.int -1,-1,-1,-1
	#	.int 0x7fffffff, 0x7fffffff, 0x7fffffff, 0x7fffffff
	#	.int 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff
	#	.int 0x08000000, 0x08000000, 0x08000000, 0x08000000
		.int 0x10000000, 0x20000000, 0x40000000, 0x80000000
	.endm
lista:		.irpc i,12345678
			linea
		.endr

longlista:	
	.int (.-lista)/4		# .= contador posiciones.
resultado:	
	.quad -1
formato:
	.ascii "suma = 0x%x%08x\n\0"

# SECCIÓN DE CÓDIGO (.text, instrucciones máquina)
.section .text
_start:	.global _start # PROGRAMA PRINCIPAL

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
	push %esi				# Preservar %esi (se usa como índice)
	mov $0, %eax			# Poner a 0 acumulador
	mov $0, %edx			# Poner a 0 acarreo
	mov $0, %esi			# Poner a 0 índice
bucle:
	add (%ebx,%esi,4), %eax	# En cada iteración del bucle, sumar el número de la lista correspondiente a %ebx
							# que se encuentra en la posición %esi-ésima y almacenarlo en %eax
	adc $0, %edx			# Añadir el acarreo si hay
	inc %esi				# Incrementar índice
	cmp %esi,%ecx			# Comparar con longitud
	jne bucle				# Si no son iguales, seguir sumando

	pop %esi				# Recuperar %esi antiguo
	ret						# Retornar de la subrutina
