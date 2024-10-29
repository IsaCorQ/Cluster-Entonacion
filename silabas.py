import re


def es_vocal(letra):
    return letra.lower() in 'aeiouáéíóúü'


def es_vocal_cerrada(letra):
    return letra.lower() in 'iuíú'


def es_vocal_abierta(letra):
    return letra.lower() in 'aeoáéó'


def es_consonante_especial(c1, c2):
    grupos = ['bl', 'cl', 'fl', 'gl', 'pl', 'br', 'cr', 'dr', 'fr', 'gr', 'pr', 'tr']
    return (c1 + c2).lower() in grupos


def es_digrafo(c1, c2):
    return (c1 + c2).lower() in ['ch', 'll', 'rr']


def forma_diptongo(v1, v2):
    if es_vocal_cerrada(v1) and es_vocal_cerrada(v2) and v1.lower() != v2.lower():
        return True
    if (es_vocal_cerrada(v1) and es_vocal_abierta(v2)) or (es_vocal_abierta(v1) and es_vocal_cerrada(v2)):
        if 'í' in (v1.lower(), v2.lower()) or 'ú' in (v1.lower(), v2.lower()):
            return False
        return True
    return False


def encontrar_siguiente_silaba(palabra, inicio):
    if inicio >= len(palabra):
        return len(palabra)

    pos = inicio
    # Encontrar la primera vocal
    while pos < len(palabra) and not es_vocal(palabra[pos]):
        pos += 1

    if pos >= len(palabra):
        return len(palabra)

    # Si hay vocales consecutivas, manejar diptongos/hiatos
    siguiente_pos = pos + 1
    if siguiente_pos < len(palabra) and es_vocal(palabra[siguiente_pos]):
        if not forma_diptongo(palabra[pos], palabra[siguiente_pos]):
            return pos + 1
        siguiente_pos += 1

    # Avanzar hasta encontrar la próxima vocal o el final
    while siguiente_pos < len(palabra) and not es_vocal(palabra[siguiente_pos]):
        siguiente_pos += 1

    # Si no hay más vocales, terminar aquí
    if siguiente_pos >= len(palabra):
        return len(palabra)

    # Contar consonantes entre vocales
    num_consonantes = siguiente_pos - (pos + 1)

    if num_consonantes == 0:
        return pos + 1
    elif num_consonantes == 1:
        return pos + 1
    elif num_consonantes == 2:
        if es_consonante_especial(palabra[pos + 1], palabra[pos + 2]) or \
                es_digrafo(palabra[pos + 1], palabra[pos + 2]):
            return pos + 1
        return pos + 2
    else:
        if pos + 3 < len(palabra) and es_consonante_especial(palabra[pos + 2], palabra[pos + 3]):
            return pos + 2
        return pos + 3


def dividir_silabas(palabra):
    silabas = []
    inicio = 0

    while inicio < len(palabra):
        fin = encontrar_siguiente_silaba(palabra, inicio)
        if fin == inicio:
            break
        silabas.append(palabra[inicio:fin])
        inicio = fin

    if inicio < len(palabra):
        silabas.append(palabra[inicio:])

    return silabas


def procesar_texto(texto):
    # Patrón para identificar palabras y mantener la puntuación y espacios
    patron = r'([^\W\d_]+|[.,!?;:()"\'¡¿\s])'
    elementos = re.findall(patron, texto)

    resultado = []
    for elem in elementos:
        if elem.isspace() or not elem.isalpha():
            resultado.append(elem)
        else:
            silabas = dividir_silabas(elem)
            resultado.append('-'.join(silabas))

    return ''.join(resultado)


def procesar_archivo(nombre_archivo_entrada, nombre_archivo_salida):
    try:
        # Leer el archivo de entrada
        with open(nombre_archivo_entrada, 'r', encoding='utf-8') as archivo:
            texto = archivo.read()

        # Procesar el texto
        texto_procesado = procesar_texto(texto)

        # Escribir el resultado en el archivo de salida
        with open(nombre_archivo_salida, 'w', encoding='utf-8') as archivo:
            archivo.write(texto_procesado)

        print(f"Archivo procesado exitosamente. Resultado guardado en {nombre_archivo_salida}")
        return True

    except FileNotFoundError:
        print(f"Error: No se encontró el archivo {nombre_archivo_entrada}")
        return False
    except Exception as e:
        print(f"Error al procesar el archivo: {str(e)}")
        return False


if __name__ == "__main__":
    archivo_entrada = "texto_entrada.txt"
    archivo_salida = "texto_silabas.txt"

    procesar_archivo(archivo_entrada, archivo_salida)