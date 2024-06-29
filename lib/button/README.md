# Button Library for ESP32

Autore: **Daniele Rossi**

Versione: **1.0**

Ultima revisione: **29 Ottobre 2021**

---

## Costruttore di Classe

> `Button(uint8_t buttonPin, unsigned long debounce)`

Viene creato un nuovo oggetto con i seguenti parametri:

* `uint8_t buttonPin`: pin associato al tasto
* `unsigned long debounce`: durata del debounce (in ms)

### Esempio

Creo l'oggetto `Button1` per il tasto associato al pin 14 con debounce di 50 ms:

> `Button Button1(14, 50);`

---

## Funzioni

### void onPress(void (*func)(void))

Associa alla pressione del tasto la funzione `func`

### void onRelease(void (*func)(void))

Associa al rilascio del tasto la funzione `func`

### void read(void)

Legge lo stato del tasto ed esegue le funzioni associate alla pressione e al rilascio

N.B.: Questa funzione va inserita nel `void loop()` del file `main.cpp`.
