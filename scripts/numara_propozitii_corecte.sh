#!/bin/bash

# verifică dacă scriptul a primit exact un argument
if [ "$#" -ne 1 ]; then
    echo "Usage: $0 <character>"
    exit 1
fi

# salvează primul argument într-o variabilă
character=$1

# verifică dacă argumentul este un caracter alfanumeric
if [[ ! "$character" =~ ^([A-Za-z]|[0-9])$ ]]; then
    echo "Eroare: Argumentul trebuie să fie un caracter alfanumeric."
    exit 1
fi

# initializează contorul
count=0

# cat timp citim linii de la intrarea standard
while IFS= read -r line; do
    # daca am citit o propozitie valida
    if [[ "$line" =~ ^[A-Z][A-Za-z0-9,\ ]*[\.\?!]$ && ! "$line" =~ ,\ si ]]; then
        # daca propozitia contine caracterul trimis ca argument, incrementam contorul
        if echo "$line" | grep -q "$character"; then
            ((count++))
        fi
    fi
done

# Afișează rcontorul
echo "$count"