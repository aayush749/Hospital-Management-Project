#pragma once
#include "hospital.h"

extern Doctor doctor[NUM_OF_DOCTORS];
extern bool Room[MAX_BEDS];
extern std::vector<Patient> patients;
extern int numberOfBedsOccupied;

void StartFromFile();

void LoadDocsData();
void LoadPatientData();
void LoadBedData();

//Init means to create the database and load the res
void InitAndCreateDB(); //Performs initializtions and creates the doctor.csv file
void InitDocs();
void InitPatient();
void InitBeds();

//Save Data
void SaveData();
static void SavePatientData();
static void SaveRoomData();