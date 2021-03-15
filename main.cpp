#include "hospital.h"
#include "db_manager.h"

#include <cstdlib>

uint16_t Patient::s_basPatientID = 1000; //will be loaded from the patient.csv file as the largest patientID, or will be set as the default value with 1000 if there is no entry in the db
Doctor doctor[NUM_OF_DOCTORS];
std::vector<Patient> patients;
int numberOfBedsOccupied = 0;	//Will be loaded on startup, default value zero.



/*
Room number allocation:-
Per patient one room, and total of 100 rooms (can be changed by modifying the MAX_BEDS macro's value
The room number is given by i +1 (for ith element in the array), 
and corresponding to the room number the status would be known by accessing the value of roomNumber - 1 th element
if false -> room empty
if true -> room occupied
*/
bool Room[MAX_BEDS];
void search_patient(uint16_t);
void Update();

int main()
{
	StartFromFile();
	while (true)
	{
		Update();
	}
}

void Update()
{
	uint16_t choice;
	std::cout << "\n\nChoose an option from below:-\n\n"
			  << "1) Seek out for a doctor\n"
			  << "2) Request a discharge\n"
			  << "3) Display details of a patient\n"
			  << "4) Show Rooms' status\n"
			  << "5) Show admitted patient's data\n"
			  << "6) Quit\n\n";

	std::cout << "Enter your choice>> ";
	std::cin >> choice;
	switch (choice)
	{
	case 1:
	{
		uint16_t docChoice;
		bool unableToSelectFreeDoctor = false;
		do
		{
			std::cout << "Choose a doctor for your treatment:-\n";
			for (int i = 0; i < NUM_OF_DOCTORS; i++)
			{
				std::cout << i + 1 << ") " << doctor[i].m_name << " - " << doctor[i].m_specialization << std::endl;
			}
			std::cout<< "11) To cancel\n";

			std::cout << (unableToSelectFreeDoctor ?
				"This doctor is currently not available, please choose another doctor>> "
				: "Please choose a doctor: ");
			std::cin >> docChoice;
		} while (!(unableToSelectFreeDoctor = true) || !doctor[docChoice - 1].IsAvailable());

		if (docChoice == 11)
		{
			std::cout << "Cancelled\n";
			break;
		}
		std::cout << "Alright, the doctor has been chosen for you" << std::endl;

		//The doctor has been chosen, now take the patient's details
		std::string name, address, contactNumber;
		uint16_t age;
		char gender;
		std::cout << "Now, please fill in the details:- " << std::endl;
		std::cin.ignore();
		std::cout << "Name: ";
		getline(std::cin, name);
		std::cout << "Age: ";
		std::cin >> age;
		std::cout << "Gender<M/F>: ";
		std::cin >> gender;
		std::cout << "Contact Number: ";
		std::cin >> contactNumber;
		std::cin.ignore();
		std::cout << "Address: ";
		getline(std::cin, address);

		patients.push_back({ name, doctor[docChoice - 1].m_doctorID, age, address, contactNumber, gender });
		doctor[docChoice - 1].AssignPatient(patients[patients.size() - 1].GetPatientID()); //assign the patient currently registered

		SaveData();
		break;
	}

	case 2:
	{
		uint16_t patientID;
		std::cout << "\n\nEnter your patient id: ";
		std::cin >> patientID;
		int i = 0;
		for (; i < MAX_BEDS; i++)
		{
			if (patients[i].GetPatientID() == patientID)
				break;
		}
		patients[i].CreateBill();

		SaveData();
		break;
	}

	case 3:
	{
		uint16_t patientID;
		std::cout << "\nEnter your patient id: ";
		std::cin >> patientID;
		search_patient(patientID);

		break;
	}

	case 4:
	{
		ShowRoomData();
		break;
	}

	case 5:
	{
		ShowPatientAdmittedData();
		break;
	}

	case 6:
	{
		std::cout << "\nSaving Data, please don't turn off your system";
		SaveData();
		std::cout << "\nSaved!" << std::endl;
		std::cout << "\nPress enter to quit" << '\n';
		std::cin.ignore();
		std::cin.get();
		exit(0);
	}

	default:
		std::cout << "\nInvalid option selected!!\n\n";
		break;
	}
}

