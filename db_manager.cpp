#include "vendor/csv.hpp"
#include "db_manager.h"
#include "hospital.h"


void StartFromFile()
{
	//Reading Data from file..if file not found, then calls InitAndCreateDB function
	std::ifstream stream("db/doctor.csv", std::ios::in);
	if (!stream.is_open())
		InitAndCreateDB();
	else   //i.e when the db exists
	{
		std::cout << "Loading from Database" << std::endl;
		
		//Close the existing stream and load the dbs concurrently
		stream.close();
		
		//Load Doctors Data
		std::thread docLoader(LoadDocsData);

		//Load Patient's Data
		std::thread patientLoader(LoadPatientData);

		//Load Bed Data
		std::thread bedLoader(LoadBedData);

		//Wait for the threads to finish loading
		docLoader.join();
		patientLoader.join();
		bedLoader.join();

	}

	//Show Welcome Message
	std::cout << "\t\t\t\tWelcome to MaxCare Hospital\n" << std::endl;
}

void InitAndCreateDB()
{
	std::thread Doctor(InitDocs);
	std::thread Patient(InitPatient);
	std::thread Beds(InitBeds);

	std::cout << "No, existing database found. Creating and Initializing Database" << std::endl;
	
	Doctor.join();
	Patient.join();
	Beds.join();


}

void LoadDocsData()
{
		std::ifstream stream("db/doctor.csv", std::ios::in);
		csv::CSVFormat format; 
		format.delimiter(',');
		csv::CSVReader reader(stream, format);
		int i = 0;
		std::string name, specification; float fees; uint16_t doc_ID;
		for (csv::CSVRow& row : reader)
		{
			if (row.size() == 1)
				break;
			name = row["Name"].get<std::string_view>();
			specification = row["Specialization"].get<std::string_view>();
			fees = row["Fees"].get<float>();
			doc_ID = row["ID"].get<uint16_t>();
			doctor[i] = Doctor(name, specification, fees, doc_ID);
			i++;
		}

		stream.close();
}

void LoadPatientData()
{	
	std::ifstream stream("db/patient.csv", std::ios::in);
	
	csv::CSVFormat format;
	format.delimiter(',');
	csv::CSVReader reader(stream, format);

	uint16_t p_id, doc_id, age, room_num, base_id = 1000;
	char gender; bool status;
	std::string name, address, contactNum;

	for (csv::CSVRow& row : reader)
	{
		if (row.size() == 1)
			break;
		p_id = row["P_ID"].get<uint16_t>();
		if (base_id < p_id)
			base_id = p_id;

		name = row["Name"].get<std::string_view>();
		doc_id = row["DOC_ID"].get<uint16_t>();
		age = row["Age"].get<uint16_t>();
		address = row["Address"].get<std::string_view>();
		contactNum = row["ContactNumber"].get<std::string_view>();
		gender = row["Gender"].get<std::string_view>().data()[0];
		room_num = row["RoomNum"].get<uint16_t>();
		status = row["Status"].get<std::string_view>().data()[0] == 'P';
		patients.push_back({ p_id, name, doc_id, age, address, contactNum, gender, room_num, status });
		

		//patients[patients.size() - 1].SetRoomNumber(row["RoomNum"].get<uint16_t>());
		//patients[patients.size() - 1].SetPresentAbsentStatus((bool)(row["Status"].get<unsigned int>()));
	}

	//set the base id as the maximum P_ID in the patient db
	Patient::SetBaseID(base_id);
	stream.close();
}

void LoadBedData()
{
	std::ifstream stream("db/beds.csv", std::ios::in);
	csv::CSVFormat format;
	format.delimiter(',');
	csv::CSVReader reader(stream, format);
	
	for (csv::CSVRow& row : reader)
	{
		if (row.size() == 1)
			break;
		//Set the room presence status for each room/bed.
		Room[row["B_IND"].get<int>()] = (row["STATUS"].get<std::string_view>().data()[0] == 'P' ? true : false);
	}

	for (size_t i = 0; i < MAX_BEDS; i++)
		if (Room[i])
			numberOfBedsOccupied++;

	stream.close();
}

void InitDocs()
{
	//Perform several initializations
	std::ofstream stream("db/doctor.csv", std::ios::out);
	auto writer = csv::make_csv_writer(stream);

	writer << std::vector<std::string_view>({ "Name", "Specialization", "Fees", "ID" });	//Write the header of the csv file


	//Doctor 1
	doctor[0] = Doctor("Dr. Raman Singh", "Specialized in Cardiology(Heart related issues)", 500, 100);

	//Doctor 2
	doctor[1] = Doctor("Dr. Nitin Sharma", "Specialized in Allergis.", 450, 101);

	//Doctor 3
	doctor[2] = Doctor("Dr. Prachi Tiwary", "Specialized in Infectious Diseaeses", 600, 102);

	//Doctor 4
	doctor[3] = Doctor("Dr. Niraj Kumar", "Specialized in Opthalmology(Eye doctor)", 550, 103);

	//Doctor 5
	doctor[4] = Doctor("Dr. Anand Kumar", "Specialized in Neurology(Brain related issues)", 700, 104);

	//Doctor 6
	doctor[5] = Doctor("Dr. Vidya Bhatia", "Specialized in Gastroenterology(Related to Digestive system", 350, 105);

	//Doctor 7
	doctor[6] = Doctor("Dr. Vijay Malhotra", "Specialized in Nephrology(Kidney related issues)", 600, 106);

	//Doctor 8
	doctor[7] = Doctor("Dr. Jaya Kumari", "Specialized in Urology", 1000, 107);

	//Doctor 9
	doctor[8] = Doctor("Dr. Simran Singh", "Specialized in Pulmonology(Breathing related issues)", 1000, 108);

	//Doctor 10
	doctor[9] = Doctor("Dr. Jitendra Kumar Singh", "Specialized in Otolaryngology(ENT)", 550, 109);

	//Doctor's data initialized
	for (size_t i = 0; i < NUM_OF_DOCTORS; i++)
	{
		writer << std::vector<std::string_view>(
			{ doctor[i].m_name,  doctor[i].m_specialization,
			  std::to_string(doctor[i].m_fee), std::to_string(doctor[i].m_doctorID)
			});
	}



	stream.close();
}

void InitPatient()
{
	std::ofstream stream("db/patient.csv", std::ios::out);

	auto writer = csv::make_csv_writer(stream);
	writer << std::vector<std::string_view>(
		{
			"P_ID", "DOC_ID", "Age", "Gender", "RoomNum", 
			"Name", "Address", "ContactNumber", "Status"
		}
	);

	stream.close();
}

void InitBeds()
{	
	std::ofstream stream("db/beds.csv", std::ios::out);

	auto writer = csv::make_csv_writer(stream);
	//write the header
	writer << std::vector<std::string_view>({ "B_IND", "STATUS" });	
	for (size_t i = 0; i < MAX_BEDS; i++)
	{
		writer << std::vector<std::string_view>({ std::to_string(i), "A" });
		Room[i] = false;
	}

	stream.close();
}

void SaveData()
{
	std::thread savePatient(SavePatientData);
	std::thread saveRoom(SaveRoomData);

	savePatient.join();
	saveRoom.join();
}

void SavePatientData()
{

	std::ofstream stream("db/patient.csv", std::ios::out);
	auto writer = csv::make_csv_writer(stream);
	//write the header of the document
	writer << std::vector<std::string_view>({ "P_ID", "DOC_ID", "Age", "Gender", "RoomNum", "Name", "Address", "ContactNumber", "Status" });

	uint16_t patientID;
	uint16_t doctorID, age;
	std::string gender;
	uint16_t roomNumber;
	std::string_view name, address, contactNumber;
	bool isPresent;

	for (Patient& patient : patients)
	{
		//Get patient's data
		patientID = patient.GetPatientID();
		doctorID = patient.GetDoctorId();
		age = patient.GetAge();
		gender = patient.GetGender();
		roomNumber = patient.GetRoomNum();
		name = patient.GetName();
		address = patient.GetAddrs();
		contactNumber = patient.GetContactNum();
		isPresent = patient.GetStatus();

		//write patient's data
		writer << std::vector<std::string_view>
		({
			std::to_string(patientID),
			std::to_string(doctorID),
			std::to_string(age),
			gender,
			std::to_string(roomNumber),
			name, address, contactNumber,
			(isPresent ? "P" : "A")
		});
	}

	stream.close();
}

void SaveRoomData()
{
	std::ofstream stream("db/beds.csv", std::ios::out);
	auto writer = csv::make_csv_writer(stream);
	//write the db header
	writer << std::vector<std::string_view>({ "B_IND", "STATUS" });

	for (size_t i = 0; i < MAX_BEDS; i++)
	{
		writer << std::vector<std::string_view>
			({
			std::to_string(i),
			(Room[i] ? "P" : "A"),
			});
	}
	stream.close();
}