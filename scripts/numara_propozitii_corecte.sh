#!/bin/bash

# Verifica daca scriptul a primit exact un argument
if [ "$#" -ne 1 ]; then
    echo "Usage: $0 <character>"
    exit 1
fi

# Salveaza primul argument intr-o variabila
CHARACTER=$1

# Verifica daca argumentul este un caracter alfanumeric
if [[ ! "$CHARACTER" =~ ^([A-Za-z]|[0-9])$ ]]; then
    echo "Eroare: Argumentul trebuie să fie un caracter alfanumeric."
    exit 1
fi

# Initializeaza contorul
COUNT=0

# Creez fisierul "out.txt"
echo "" > out.txt

# Functie care verifica daca o propozitie este corecta
function check() {
    if [[ "$1" =~ ^[A-Z][A-Za-z0-9,\ ]*[\.\?!]$ && ! "$1" =~ ,\ si ]]; then
        if echo "$1" | grep -q "$CHARACTER"; then
            echo "Yes" >> out.txt           # in caz afirmativ scriu un "Yes" in fisierul "out.txt"
        fi
    fi
}

# Cat timp citim linii de la intrarea standard
while IFS= read -r LINE; do
    # Sparg linia in propozitii (delimitate de .|!|?)
    echo "$LINE" | grep -o -E '[^.!?]+[.!?]' | while IFS= read -r SENTENCE; do
        # Elimin spatiile de la inceputul propozitiei (daca exista),
        # deoarece pe aceasi linie putem avea mai multe propozitii despartite prin spatiu
        TRIMMED_SENTENCE=$(echo "$SENTENCE" | sed 's/^\ *//')
        # Verific daca propozitia este corecta
        check "$TRIMMED_SENTENCE"
    done
done
 
# Dupa ce am terminat de citit linii de la tastatura, verific de cate ori am tiparit "Yes" in fisierul "out.txt"
while IFS= read -r LINE; do
    if echo $LINE | grep -q "Yes"; then
        ((COUNT++))
    fi
done < out.txt

# Elimin fisierul
rm out.txt

# Afisez numarul de propozitii corecte
echo "$COUNT"