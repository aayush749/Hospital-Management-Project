#include "hospital.h"

#include "vendor/csv.hpp"

#include <string>


extern Doctor doctor[NUM_OF_DOCTORS];
extern std::vector<Patient> patients;
extern int numberOfBedsOccupied;
extern bool Room[MAX_BEDS];

//Called when admitting a patient
Patient::Patient(const std::string& name, int docID, int age, const std::string& address, const std::string& contactNumber, char gender)
{
	auto roomStatus = isAnyRoomEmpty();
	if (roomStatus.index() == 0)
	{
		std::cout << "\nSorry, no beds available, unable to admit patient :-(\n";
		return;
	}

	m_name = name;
	m_doctorID = docID;
	m_age = age;
	m_address = address;
	m_contactNumber = contactNumber;
	m_gender = gender;
	m_patientID = ++s_basPatientID;
	m_isPresent = true;
	numberOfBedsOccupied += 1;
	
	//Rooms allocation code
	auto RoomNum = *(std::get_if<int>(&roomStatus));
	Room[RoomNum - 1] = true;	//change the occupancy status of the room to true
	m_roomNumber = RoomNum;   //assign the room number of the patient
	std::cout << "The room number allocated to you is: " << m_roomNumber << std::endl;   //display the room number alloted
}

//default constructor for Patient
Patient::Patient()
{
	auto roomStatus = isAnyRoomEmpty();
	if (roomStatus.index() == 0)
	{
		std::cout << "\nSorry, no beds available, unable to admit patient :-(\n";
		return;
	}

	m_name = "Unknown";
	m_doctorID = NULL;
	m_age = NULL;
	m_address = "Unknown";
	m_contactNumber = "Unknown";
	m_gender = '?';
	m_patientID = ++s_basPatientID;
	m_isPresent = true;
	numberOfBedsOccupied += 1;

	//Rooms allocation code
	Room[*(std::get_if<int>(&roomStatus)) - 1] = true;	//change the occupancy status of the room to true
	m_roomNumber = *(std::get_if<int>(&roomStatus));   //assign the room number of the patient
	std::cout << "The room number allocated to you is: " << m_roomNumber << std::endl;   //display the room number alloted
}

Patient::Patient(const Patient& other)
{
	m_patientID = other.m_patientID;
	m_doctorID = other.m_doctorID, m_age = other.m_age;
	m_gender = other.m_gender;
	m_roomNumber = other.m_roomNumber;
	m_name = other.m_name, m_address = other.m_address, m_contactNumber = other.m_contactNumber;
	bool m_isPresent = other.m_isPresent;

}

//Called when initializing object from data inside file
Patient::Patient(uint16_t p_ID, const std::string& name, int docID, int age, const std::string& address, const std::string& contactNumber, char gender, uint16_t rNo, bool status)
{
	m_patientID = p_ID;
	m_name = name;
	m_doctorID = docID;
	m_age = age;
	m_address = address;
	m_contactNumber = contactNumber;
	m_gender = gender;
	m_roomNumber = rNo;
	m_isPresent = status;
}

void Patient::CreateBill()
{
	if (m_isPresent == false)
	{
		std::cout << "\nThis patient took a discharge already!\n"<<std::endl;
		return;
	}
	float charge = 100.0f;  //by default charge (in case of any mishap)
	for (int i = 0; i < NUM_OF_DOCTORS; i++)
	{
		if (doctor[i].m_doctorID == m_doctorID)
			charge = doctor[i].m_fee;
	}

	m_isPresent = false;
	numberOfBedsOccupied -= 1;
	Room[m_roomNumber - 1] = false; //change the room's occupancy status 

	std::cout << "\n\n";
	std::cout << "Name: " << m_name<<"\n"
			  <<"Age: "<<m_age<<"\n"
		      <<"Gender: "<<m_gender<<"\n"
		      <<"Patient ID: "<<m_patientID<<"\n"
		      <<"Address: "<<m_address<<"\n"
			  <<"Contact Number: "<<m_contactNumber<<"\n"
			  <<"Room Number: "<<m_roomNumber<<"\n"
			  <<"Total Amount to be paid: Rs."<<charge<<"\n\n";
	std::cout << "Please pay the amount at our reception desk, wish you a good health forever!!" << std::endl;
}

bool Doctor::IsAvailable()
{
	return (m_patientunder.size() == MAX_PATIENTS_PER_DOCTOR ? false : true);
}


Doctor::Doctor(const std::string& name, const std::string& specialization, float fee, uint16_t docID)
{
	m_name = name;
	m_specialization = specialization;
	m_fee = fee;
	m_doctorID = docID;
}

void search_patient(uint16_t id)
{
	size_t i;
	for (i = 0; i < patients.size(); i++)
	{
		if (patients[i].m_patientID == id)
		{
			patients[i].show();
			break;
		}
	}
	if (i == patients.size())
		std::cout << "No such patient found\n";
}

std::variant<bool, int> isAnyRoomEmpty()	
{
	for (short i = 0; i < MAX_BEDS; i++)
		if (Room[i] == false)
			return (i + 1);	

	return false;
}

void ShowRoomData()
{
	size_t vacant = 0;
	size_t filled = 0;
	for (short i = 0; i < MAX_BEDS; i++)
	{
		if (Room[i] == false)
			vacant++;
		else
			filled++;
	}

	std::cout << "Total number of rooms: " << MAX_BEDS << std::endl;
	std::cout << "Total number of empty rooms: " << vacant << std::endl;
	std::cout << "Total number of occupied rooms: " << filled << std::endl;
}

void ShowPatientAdmittedData()
{
	if (numberOfBedsOccupied == 0)
	{
		std::cout << "\nNo patients are admitted currently!" << std::endl;
		return;
	}


	for (int i = 0; i < MAX_BEDS; i++)
	{
		std::ifstream stream("db/patient.csv", std::ios::in);
		csv::CSVFormat format;
		format.delimiter(',');
		csv::CSVReader reader(stream, format);
		if (Room[i] == true)
		{
			for (csv::CSVRow& row : reader)
			{
				if (row["RoomNum"].get<uint16_t>() == i + 1 && row["Status"].get<std::string_view>().data()[0] == 'P')
				{
					patients[row["P_ID"].get<uint16_t>() - 1001].show();
					break;
				}
			}
		}
		stream.close();
	}

}


