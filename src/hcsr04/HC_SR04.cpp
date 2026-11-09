#include "HC_SR04.h"

// CONSTRUCTEUR

HC_SR04::HC_SR04(int trig, int echo)
{
    trigPin = trig;
    echoPin = echo;
}

// INITIALISATION

void HC_SR04::init()
{
    pinMode(trigPin, OUTPUT);
    pinMode(echoPin, INPUT);
}


float HC_SR04::getDistance()
{
    // Déclenchement

    digitalWrite(trigPin, LOW);

    delayMicroseconds(2);

    digitalWrite(trigPin, HIGH);

    delayMicroseconds(10);

    digitalWrite(trigPin,LOW);


    // Lecture du temps de retour

    long duree = pulseIn(echoPin, HIGH, 30000);

    // Pas de mesure valide

    if (duree <= 0)
    {
        return -1.0;
    }

    // Calcul de la distance en cm

    float distance = duree * 0.0343 / 2.0;

    return distance;
}
