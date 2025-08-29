// Número de mediciones que quieres promediar
const int NUMBER_OF_DISTANCES = 5;

// Array para guardar las últimas mediciones
float distancias[NUMBER_OF_DISTANCES];

// Índice actual en el array
int indice = 0;

// Contador de cuántas mediciones reales llevamos
int contadorMediciones = 0;

void newRange(float nuevaMedicion) {
    // Guardar la nueva medición en la posición actual
    distancias[indice] = nuevaMedicion;

    // Avanzar el índice de forma circular
    indice = (indice + 1) % NUMBER_OF_DISTANCES;

    // Aumentar el contador hasta el máximo
    if (contadorMediciones < NUMBER_OF_DISTANCES) {
        contadorMediciones++;
    }

    // Calcular la media de las mediciones disponibles
    float suma = 0;
    for (int i = 0; i < contadorMediciones; i++) {
        suma += distancias[i];
    }
    float media = suma / contadorMediciones;

    // Mostrar resultados
    Serial.print("Medición actual: ");
    Serial.println(nuevaMedicion);
    Serial.print("Media: ");
    Serial.println(media);
}

void setup() {
    Serial.begin(9600);
}

void loop() {
    // Ejemplo: simulamos una nueva medición
    float medicion = random(50, 150) / 10.0; // valor aleatorio entre 5.0 y 15.0
    newRange(medicion);

    delay(1000); // esperar