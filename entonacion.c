#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <termios.h>
#include <errno.h>
#include <fcntl.h> 
#include <unistd.h> 

// Utility functions for Vigenère Cipher
#define CARACTERES "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ!\"#$%&'()*+,-./:;<=>?@[\\]^_`{|}~ \t\n\r\x0b\x0c"
#define LONGITUD strlen(CARACTERES)
char result[1];

int indice_caracter(char c) {
    for (int i = 0; i < LONGITUD; i++) {
        if (CARACTERES[i] == c) {
            return i;
        }
    }
    return -1; // Retornamos -1 si el carácter no está en el conjunto
}

void cifrar_vigenere(const char *mensaje, const char *clave, char *texto_cifrado) {
    int mensaje_len = strlen(mensaje);
    int clave_len = strlen(clave);

    for (int i = 0; i < mensaje_len; i++) {
        char m = mensaje[i];
        char k = clave[i % clave_len];
        int indice_m = indice_caracter(m);
        int indice_k = indice_caracter(k);

        if (indice_m != -1) {
            texto_cifrado[i] = CARACTERES[(indice_m + indice_k) % LONGITUD];
        } else {
            texto_cifrado[i] = m;
        }
    }
    texto_cifrado[mensaje_len] = '\0'; // Aseguramos el fin de cadena
}

void descifrar_vigenere(const char *texto_cifrado, const char *clave, char *mensaje_descifrado) {
    int texto_len = strlen(texto_cifrado);
    int clave_len = strlen(clave);

    for (int i = 0; i < texto_len; i++) {
        char c = texto_cifrado[i];
        char k = clave[i % clave_len];
        int indice_c = indice_caracter(c);
        int indice_k = indice_caracter(k);

        if (indice_c != -1) {
            mensaje_descifrado[i] = CARACTERES[(indice_c - indice_k + LONGITUD) % LONGITUD];
        } else {
            mensaje_descifrado[i] = c;
        }
    }
    mensaje_descifrado[texto_len] = '\0'; // Aseguramos el fin de cadena
}

// Utility functions for syllable classification
int is_vowel(char c) {
    char vowels[] = "aeiouáéíóúü";
    c = tolower(c);
    for (int i = 0; vowels[i] != '\0'; i++) {
        if (c == vowels[i]) return 1;
    }
    return 0;
}

int is_accented(char c) {
    char accents[] = "áéíóú";
    c = tolower(c);
    for (int i = 0; accents[i] != '\0'; i++) {
        if (c == accents[i]) return 1;
    }
    return 0;
}

// Function to divide a word into syllables (simplified logic)
void divide_syllables(char *word, char *result) {
    int len = strlen(word);
    int idx = 0;
    for (int i = 0; i < len; i++) {
        result[idx++] = word[i];
        if (is_vowel(word[i]) && i < len - 1 && !is_vowel(word[i + 1])) {
            result[idx++] = '-';
        }
    }
    result[idx] = '\0';
}

// Function to classify a word based on syllable stress
const char *classify_word(const char *syllabified) {
    int len = strlen(syllabified);
    int stress_pos = -1;

    // Identify stressed syllable based on accented vowels
    for (int i = 0; i < len; i++) {
        if (is_accented(syllabified[i])) {
            stress_pos = i;
            break;
        }
    }

    // Default classification
    if (stress_pos == -1) return "grave"; // Assume grave if no accent

    // Count syllables to determine classification
    int syllable_count = 0;
    for (int i = 0; i < len; i++) {
        if (syllabified[i] == '-') syllable_count++;
    }
    syllable_count++; // Total syllables

    if (syllable_count == 1) return "monosyllable";
    if (stress_pos < len - 4) return "sobreesdrújula";
    if (stress_pos < len - 3) return "esdrújula";
    if (stress_pos < len - 2) return "grave";
    return "aguda";
}

// Process a chunk of text
void process_chunk(const char *chunk, int chunk_size, int *results) {
    char word[100];
    char syllabified[200];
    int count = 0;

    for (int i = 0; i < chunk_size; i++) {
        if (isspace(chunk[i]) || ispunct(chunk[i])) {
            word[count] = '\0';
            if (count > 0) {
                divide_syllables(word, syllabified);
                const char *category = classify_word(syllabified);

                if (strcmp(category, "monosyllable") == 0) results[0]++;
                else if (strcmp(category, "aguda") == 0) results[1]++;
                else if (strcmp(category, "grave") == 0) results[2]++;
                else if (strcmp(category, "esdrújula") == 0) results[3]++;
                else if (strcmp(category, "sobreesdrújula") == 0) results[4]++;
            }
            count = 0;
        } else {
            word[count++] = chunk[i];
        }
    }
}

char classify_result_to_char(int classification) {
    switch (classification) {
        case 0: return 'm'; // Monosyllable
        case 1: return 'a'; // Aguda
        case 2: return 'g'; // Grave
        case 3: return 'e'; // Esdrújula
        case 4: return 's'; // Sobreesdrújula
        default: return 'u'; // Unknown
    }
}

// Main function
int main(int argc, char *argv[]) {

    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    const char *input_file = "texto_entrada.txt";
    const char *output_file = "histograma.txt";

    char *text = NULL;
    int text_size = 0;

    int serial_port = open("/dev/ttyACM0", O_RDWR); // Open the serial port (replace with your actual port)


    if (rank == 0) {


        if (serial_port < 0) {
            printf("Error opening serial port\n");
            return 1;
        }

        struct termios tty;
        if (tcgetattr(serial_port, &tty) != 0) {
            printf("Error getting serial port attributes\n");
            return 1;
        }

        cfsetospeed(&tty, B9600); // Set baud rate to 9600
        cfsetispeed(&tty, B9600);

        tty.c_cflag &= ~PARENB; // No parity bit
        tty.c_cflag &= ~CSTOPB; // 1 stop bit
        tty.c_cflag &= ~CSIZE;
        tty.c_cflag |= CS8;     // 8 data bits
        tty.c_cflag |= CREAD | CLOCAL; // Enable receiver, local connection

        tcsetattr(serial_port, TCSANOW, &tty);
        // Read the text file
        FILE *file = fopen(input_file, "r");
        fseek(file, 0, SEEK_END);
        text_size = ftell(file);
        rewind(file);

        text = malloc(text_size + 1);
        fread(text, 1, text_size, file);
        text[text_size] = '\0';
        fclose(file);

        // Cipher the message
        const char *clave = "SEGURIDAD";
        char texto_cifrado[1024];
        cifrar_vigenere(text, clave, texto_cifrado);
        strcpy(text, texto_cifrado); // Replace original text with ciphered text
    }

    // Broadcast text size to all processes
    MPI_Bcast(&text_size, 1, MPI_INT, 0, MPI_COMM_WORLD);

    // Divide text into chunks
    int chunk_size = text_size / size;
    char *chunk = malloc(chunk_size + 1);

    MPI_Scatter(text, chunk_size, MPI_CHAR, chunk, chunk_size, MPI_CHAR, 0, MPI_COMM_WORLD);
    chunk[chunk_size] = '\0';

    int local_results[5] = {0, 0, 0, 0, 0}; // [monosyllable, aguda, grave, esdrújula, sobreesdrújula]
    process_chunk(chunk, chunk_size, local_results);

    int global_results[5] = {0, 0, 0, 0, 0};
    MPI_Reduce(local_results, global_results, 5, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        // Find the most frequent category
        const char *categories[] = {"monosyllable", "aguda", "grave", "esdrújula", "sobreesdrújula"};
        int max_index = 0;
        for (int i = 1; i < 5; i++) {
            if (global_results[i] > global_results[max_index]) {
                max_index = i;
            }
        }

        // Write the entire string, including the null terminator
        char input_char;
        char classification_char = classify_result_to_char(max_index);
        //printf("Enter a letter to send to Arduino: ");
        //usleep(1000);
        scanf("%c", &input_char);  // Read the letter from user input

        // Send the character to the Arduino
        //write(serial_port, &input_char, 1);
        //printf("Sent: %c\n", input_char);

        // Send the classification character to the Arduino
        write(serial_port, &classification_char, 1);
        printf("Sent classification: %c \n", classification_char);

        // Print the most repeated category
        printf("The most repeated category is: %s\n", categories[max_index]);

        // Save detailed histogram to file
        FILE *output = fopen(output_file, "w");
        fprintf(output, "Monosyllable: %d\n", global_results[0]);
        fprintf(output, "Aguda: %d\n", global_results[1]);
        fprintf(output, "Grave: %d\n", global_results[2]);
        fprintf(output, "Esdrújula: %d\n", global_results[3]);
        fprintf(output, "Sobreesdrújula: %d\n", global_results[4]);
        fclose(output);

        printf("Results saved to %s\n", output_file);
    }

    if (text) free(text);
        free(chunk);

        MPI_Finalize();
        return 0;
    close(serial_port); // Close the serial port
}