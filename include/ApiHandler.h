#ifndef API_HANDLER_H
#define API_HANDLER_H
#include <Arduino.h>

String getStatsJSON();
void registarAcao(String nomeAcao);
void iniciarMetricas();

#endif