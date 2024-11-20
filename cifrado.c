#include <stdio.h>
#include <string.h>
#include <ctype.h>

// Definimos el conjunto completo de caracteres imprimibles
#define CARACTERES "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ!\"#$%&'()*+,-./:;<=>?@[\\]^_`{|}~ \t\n\r\x0b\x0c"
#define LONGITUD strlen(CARACTERES)

// Función para obtener el índice de un carácter en el conjunto de caracteres imprimibles
int indice_caracter(char c) {
    for (int i = 0; i < LONGITUD; i++) {
        if (CARACTERES[i] == c) {
            return i;
        }
    }
    return -1; // Retornamos -1 si el carácter no está en el conjunto
}

// Función para cifrar con Vigenère
void cifrar_vigenere(const char *mensaje, const char *clave, char *texto_cifrado) {
    int mensaje_len = strlen(mensaje);
    int clave_len = strlen(clave);

    for (int i = 0; i < mensaje_len; i++) {
        char m = mensaje[i];
        char k = clave[i % clave_len];
        int indice_m = indice_caracter(m);
        int indice_k = indice_caracter(k);

        if (indice_m != -1) {
            // Aplicamos el desplazamiento y tomamos el módulo de la longitud total
            texto_cifrado[i] = CARACTERES[(indice_m + indice_k) % LONGITUD];
        } else {
            // Si el carácter no está en el conjunto, lo dejamos igual
            texto_cifrado[i] = m;
        }
    }
    texto_cifrado[mensaje_len] = '\0'; // Aseguramos el fin de cadena
}

// Función para descifrar con Vigenère
void descifrar_vigenere(const char *texto_cifrado, const char *clave, char *mensaje_descifrado) {
    int texto_len = strlen(texto_cifrado);
    int clave_len = strlen(clave);

    for (int i = 0; i < texto_len; i++) {
        char c = texto_cifrado[i];
        char k = clave[i % clave_len];
        int indice_c = indice_caracter(c);
        int indice_k = indice_caracter(k);

        if (indice_c != -1) {
            // Aplicamos el desplazamiento inverso y tomamos el módulo
            mensaje_descifrado[i] = CARACTERES[(indice_c - indice_k + LONGITUD) % LONGITUD];
        } else {
            // Si el carácter no está en el conjunto, lo dejamos igual
            mensaje_descifrado[i] = c;
        }
    }
    mensaje_descifrado[texto_len] = '\0'; // Aseguramos el fin de cadena
}

int main() {
    const char *mensaje = "¡Hola, mundo! ¿Cómo estás? Este es un mensaje de prueba para verificar si el cifrado y descifrado funcionan correctamente.";
    const char *clave = "SEGURIDAD";
    char texto_cifrado[1024];
    char mensaje_descifrado[1024];

    printf("Mensaje original: %s\n", mensaje);

    // Cifrado
    cifrar_vigenere(mensaje, clave, texto_cifrado);
    printf("Texto cifrado: %s\n", texto_cifrado);

    // Descifrado
    descifrar_vigenere(texto_cifrado, clave, mensaje_descifrado);
    printf("Texto descifrado: %s\n", mensaje_descifrado);

    return 0;
}

