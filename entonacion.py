import re
from collections import Counter

def classify_word(word):
    # Divide las palabras por sílabas
    syllables = word.split('-')
    num_syllables = len(syllables)

    # Revisar si hay tildes
    has_tilde = any(re.search(r"[áéíóú]", syllable) for syllable in syllables)

    if num_syllables == 1:
        return 'monosyllable'
    
    if has_tilde:
        # Classify based on the position of the accent
        if re.search(r"[áéíóú]", syllables[-4]) if num_syllables > 3 else False:
            return 'sobreesdrújula'  # Stressed in the fourth-to-last or earlier
        elif re.search(r"[áéíóú]", syllables[-3]):
            return 'esdrújula'  # Stressed on the antepenultimate syllable
        elif re.search(r"[áéíóú]", syllables[-2]):
            return 'grave'  # Stressed on the penultimate syllable
        elif re.search(r"[áéíóú]", syllables[-1]):
            return 'aguda'  # Stressed on the last syllable
    else:
        # No accent marks: classify based on the last letter
        if word[-1] in "aeiouns":  # Ends with a vowel, n, or s
            return 'grave'
        else:  # Ends with a consonant other than n, s, or a vowel
            return 'aguda'

def process_text_from_file(file_path):
    # Leer txt entrada
    with open(file_path, 'r', encoding='utf-8') as f:
        text = f.read()
    
    # Conseguir palabras
    words = re.findall(r'\b\w+(?:-\w+)*\b', text)
    
    # Clasificar y contar
    categories = []
    for word in words:
        category = classify_word(word)
        print(f"{word}: {category}")  # Print each word and its classification
        categories.append(category)
        
    return Counter(categories)

def save_histogram_to_file(counter, output_path):
    # Sort the counter items by count in descending order
    sorted_counter = counter.most_common()
    
    with open(output_path, 'w', encoding='utf-8') as f:
        f.write("Histogram of word types:\n")
        for category, count in sorted_counter:
            f.write(f"{category.capitalize()}: {count}\n")
    print(f"Histogram saved to {output_path}")

input_file_path = 'texto_silabas.txt'
output_file_path = 'histograma.txt'

category_counts = process_text_from_file(input_file_path)
save_histogram_to_file(category_counts, output_file_path)
