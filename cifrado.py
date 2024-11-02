import string

# Generamos el conjunto completo de caracteres imprimibles
CARACTERES = string.printable  # Incluye letras, números, signos de puntuación y otros caracteres imprimibles
LONGITUD = len(CARACTERES)

# Función para cifrar con Vigenère
def cifrar_vigenere(mensaje, clave):
    texto_cifrado = []
    clave_expandida = (clave * (len(mensaje) // len(clave) + 1))[:len(mensaje)]
    
    for m, k in zip(mensaje, clave_expandida):
        if m in CARACTERES:
            # Encontramos el índice de cada carácter en el conjunto completo
            indice_m = CARACTERES.index(m)
            indice_k = CARACTERES.index(k)
            # Aplicamos el desplazamiento y tomamos el módulo de la longitud total
            texto_cifrado.append(CARACTERES[(indice_m + indice_k) % LONGITUD])
        else:
            # Si el carácter no está en el conjunto (por ejemplo, un carácter no imprimible), se deja igual
            texto_cifrado.append(m)
    
    return ''.join(texto_cifrado)

# Función para descifrar con Vigenère
def descifrar_vigenere(texto_cifrado, clave):
    mensaje_descifrado = []
    clave_expandida = (clave * (len(texto_cifrado) // len(clave) + 1))[:len(texto_cifrado)]
    
    for c, k in zip(texto_cifrado, clave_expandida):
        if c in CARACTERES:
            # Encontramos el índice de cada carácter en el conjunto completo
            indice_c = CARACTERES.index(c)
            indice_k = CARACTERES.index(k)
            # Aplicamos el desplazamiento inverso y tomamos el módulo
            mensaje_descifrado.append(CARACTERES[(indice_c - indice_k) % LONGITUD])
        else:
            # Si el carácter no está en el conjunto, se deja igual
            mensaje_descifrado.append(c)
    
    return ''.join(mensaje_descifrado)

# Ejemplo de uso
mensaje = "¡Hola, mundo! ¿Cómo estás? Este es un mensaje de prueba para verificar si el cifrado y descifrado funcionan correctamente, incluyendo letras acentuadas como á, é, í, ó, ú, y ñ."
clave = "SEGURIDAD"
print("Mensaje original:", mensaje)

# Cifrado
texto_cifrado = cifrar_vigenere(mensaje, clave)
print("Texto cifrado:", texto_cifrado)

# Descifrado
mensaje_descifrado = descifrar_vigenere(texto_cifrado, clave)
print("Texto descifrado:", mensaje_descifrado)

