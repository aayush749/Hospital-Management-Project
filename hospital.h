#pragma once
/*
The project Hospital Management system includes registration of patients, storing their details
into the system, and also computerized billing in the pharmacy, and labs.The software has the
facility to give a unique id for every patientand stores the details of every patient and the staff
automatically.It includes a search facility to know the current status of each room.User can
search availability of a doctor and the details of a patient using the id.
*/

#include <iostream>
#include <string>
#include <vector>
#include <iomanip>
#include <variant>

#define MAX_BEDS 100
#define MAX_PATIENTS_PER_DOCTOR 5
#define NUM_OF_DOCTORS 10

extern int numberOfBedsOccupied;

class Patient;
class Doctor;

class Patient
{
public:
	void CreateBill(); //this function displays the charges of the patient and also vacates the patient's occupancy by changing the isPresent variable's value to 'false'

	uint16_t GetPatientID()
	{
		return m_patientID;
	}

	Patient();

	Patient(const Patient& other);

	Patient(uint16_t p_ID, const std::string& name, int docID, 
			int age, const std::string& address,
		    const std::string& contactNumber, char gender, 
			uint16_t rNo, bool status);

	Patient(const std::string& name, int docID, int age,
		    const std::string& address, const std::string& contactNumber, 
		    char gender);

	friend void search_patient(uint16_t);

	void show()
	{
		std::cout << std::endl << std::setw(16) << std::left << "Name:" << m_name;
		std::cout << std::endl << std::setw(16) << std::left << "Age:" << m_age;
		if (m_gender == 'M' || m_gender == 'm')
			std::cout << std::endl << std::setw(16) << std::left << "Gender:" << "Male";
		else
			std::cout << std::endl << std::setw(16) << std::left << "Gender:" << "Female";
		std::cout << std::endl << std::setw(16) << std::left << "Address:" << m_address;
		std::cout << std::endl << std::setw(16) << std::left << "Contact Number:" << m_contactNumber << std::endl;
		std::cout << std::endl << std::setw(16) << std::left << "Room Number:" << m_roomNumber << std::endl;
		
	}

	inline static void SetBaseID(uint16_t b_id)
	{
		Patient::s_basPatientID = b_id;
	}

	const uint16_t& GetDoctorId() const
	{
		return m_doctorID;
	}

	const char& GetGender() const
	{
		return m_gender;
	}
	
	const uint16_t& GetAge() const
	{
		return m_age;
	}

	const uint16_t& GetRoomNum() const
	{
		return m_roomNumber;
	}

	const std::string& GetName() const
	{
		return m_name;
	}

	const std::string& GetAddrs() const
	{
		return m_address;
	}

	const std::string& GetContactNum() const
	{
		return m_contactNumber;
	}

	const bool& GetStatus() const
	{
		return m_isPresent;
	}
private:
	static uint16_t s_basPatientID;
	uint16_t m_patientID;
	uint16_t m_doctorID, m_age;
	char m_gender;
	uint16_t m_roomNumber;
	std::string m_name, m_address, m_contactNumber;
	bool m_isPresent;

};


class Doctor
{
	friend class Patient; //to make private members accessible in Patient class
private:
	std::vector<int> m_patientunder; //stores the patient ID's of the patient currently under the doctor
public:
	uint16_t m_doctorID;
	std::string m_name, m_specialization;
	float m_fee;
	bool IsAvailable();

	Doctor()
	{
		//empty default constructor, initialiazation to be performed in the Start function, if the db is not found on disk.
	}
	Doctor(const std::string& name, const std::string& specialization, float fee, uint16_t docID);

	//This method is called after the doctor has been verified for availability by IsAvailable() method
	void AssignPatient(uint16_t patientID)
	{
		m_patientunder.push_back(patientID);
		std::cout << "\nThe patient ID " << patientID << " has been assigned under the care of " << m_name
			<< "\nWe wish you a speedy recovery!!" << std::endl;
	}
};


std::variant<bool, int> isAnyRoomEmpty();	//Returns room number only if any room is empty, else it returns a bool

void ShowRoomData();

void ShowPatientAdmittedData();

