#include <iostream>
#include <vector>
#include <fstream>
#include <algorithm>
#include <unordered_map>
#include <queue>
#include <stdexcept>
#include <string>
using namespace std;

// Observer Interface
class Appointment; // Forward declaration
class Observer {
public:
    virtual void Update(const Appointment& a) = 0;
    virtual ~Observer() {}
};

// Subject Class
class Subject {
    vector<Observer*> observers;
public:
    void Attach(Observer* observer) {
        observers.push_back(observer);
    }
    void Detach(Observer* observer) {
        observers.erase(remove(observers.begin(), observers.end(), observer), observers.end());
    }
    void Notify(const Appointment& a) {
        for (Observer* observer : observers) {
            observer->Update(a);
        }
    }
};

// Patient Class
class Patient {
protected:
    string name;
    int age;
    int id;
    static int idCounter;

public:
    Patient() : id(++idCounter), age(0) {}

    static void setId(int lastId) {
        idCounter = lastId;
    }

    void setName(const string& n) { name = n; }
    string getName() const { return name; }

    void setAge(int a) { age = a; }
    int getAge() const { return age; }

    int getId() const { return id; }

    // Save patient to file 
    void SaveToFile() const {
        ofstream file("Patients.txt", ios::app);
        if (!file.is_open()) {
            throw runtime_error("Cannot open Patients.txt");
        }
        file << name << " " << id << " " << age << endl;
        file.close();
    }

    // Load patients from file
    static unordered_map<int, Patient> LoadFromFile() {
        unordered_map<int, Patient> patients;
        ifstream file("Patients.txt");
        if (!file.is_open()) {
            return patients; // Return empty map if file doesn't exist
        }
        string name;
        int id, age;
        int maxId = 0;
        while (file >> name >> id >> age) {
            Patient p;
            p.name = name;
            p.age = age;
            p.id = id;
            patients[id] = p;
            maxId = max(maxId, id);
        }
        setId(maxId); // Update idCounter
        file.close();
        return patients;
    }

    // Add new patient
    void AddPatient(unordered_map<int, Patient>& patients) {
        Patient p;
        cin.ignore(); // Clear buffer
        cout << "Patient name? ";
        getline(cin, p.name);
        cout << "Patient age? ";
        cin >> p.age;
        patients[p.id] = p;
        p.SaveToFile();
    }
};
int Patient::idCounter = 0;

// Doctor Class
class Doctor {
    string name;
    int id;
    string specialization;
    string date;
    string time;

public:
    Doctor() : id(0) {}
    Doctor(string n, int i, string s) : name(n), id(i), specialization(s) {}

    void setName(const string& n) { name = n; }
    void setSpecialization(const string& s) { specialization = s; }
    void setId(int i) { id = i; }
    void setDate(const string& d) { date = d; }
    void setTime(const string& t) { time = t; }

    string getName() const { return name; }
    string getSpecialization() const { return specialization; }
    int getId() const { return id; }
    string getDate() const { return date; }
    string getTime() const { return time; }

    // Save single doctor to file (append mode)
    void SaveToFile() const {
        ofstream file("Doctors.txt", ios::app);
        if (!file.is_open()) {
            throw runtime_error("Cannot open Doctors.txt");
        }
        file << name << " " << specialization << " " << date << " " << time << endl;
        file.close();
    }

    // Load doctors from file
    static unordered_map<int, Doctor> LoadFromFile() {
        unordered_map<int, Doctor> doctors;
        ifstream file("Doctors.txt");
        if (!file.is_open()) {
            return doctors;
        }
        string name, specialization, date, time;
        int id = 1;
        while (file >> name >> specialization >> date >> time) {
            Doctor d(name, id++, specialization);
            d.setDate(date);
            d.setTime(time);
            doctors[d.getId()] = d;
        }
        file.close();
        return doctors;
    }
};

// Reservation Strategy
class ReservationStrategy {
public:
    virtual vector<Doctor> search(const unordered_map<string, vector<Doctor>>& doctors) = 0;
    virtual ~ReservationStrategy() {}
};

// Search by Name
class SearchUsingName : public ReservationStrategy {
public:
    vector<Doctor> search(const unordered_map<string, vector<Doctor>>& doctors) override {
        string name;
        cout << "Doctor Name? ";
        cin >> name;
        auto it = doctors.find(name);
        if (it != doctors.end()) {
            return it->second;
        }
        cout << "No doctors found with name: " << name << endl;
        return {};
    }
};

// Search by Specialization
class SearchUsingSpecialization : public ReservationStrategy {
public:
    vector<Doctor> search(
                         const unordered_map<string, vector<Doctor>>& doctorsBySpec) override {
        string spec;
        cout << "Specialization? ";
        cin >> spec;
        auto it = doctorsBySpec.find(spec);
        if (it != doctorsBySpec.end()) {
            return it->second;
        }
        cout << "No doctors found with specialization: " << spec << endl;
        return {};
    }
};

// Reservation Context
class ReservationContext {
    ReservationStrategy* strategy;
public:
    ReservationContext() : strategy(nullptr) {}
    ~ReservationContext() { delete strategy; }

    void setStrategy(ReservationStrategy* s) {
        delete strategy;
        strategy = s;
    }

    vector<Doctor> search(const unordered_map<string, vector<Doctor>>& doctors) {
        if (strategy) {
            return strategy->search(doctors);
        }
        return {};
    }
};

// Appointment Class
class Appointment : public Subject {
    int patientId;
    int doctorId;
    string date;
    string time;
    int priority; // New field for priority

public:
    Appointment() : patientId(0), doctorId(0), priority(0) {}
    Appointment(int pId, int dId, string d, string t, int p = 2) // Default priority = 2 (normal)
        : patientId(pId), doctorId(dId), date(d), time(t), priority(p) {}

    int getPatientId() const { return patientId; }
    int getDoctorId() const { return doctorId; }
    string getDate() const { return date; }
    string getTime() const { return time; }
    int getPriority() const { return priority; }

    //put the date priority according to the case priority 
    //comparing tne priority
    bool operator>(const Appointment& other) const {
        if (priority != other.priority) {
            return priority > other.priority;
        }
        return date > other.date; // If priorities are equal, sort by date
    }

    // Save appointment to file
    void SaveToFile() const {
        ofstream file("Appointments.txt", ios::app);
        if (!file.is_open()) {
            throw runtime_error("Cannot open Appointments.txt");
        }
        file << patientId << " " << doctorId << " " << date << " " << time << " " << priority << endl;
        file.close();
    }
};

// Appointment Manager with Priority Queue
class AppointmentManager {
    priority_queue<Appointment, vector<Appointment>, greater<Appointment>> appointments;
    unordered_map<int, Patient> patients;
    unordered_map<int, Doctor> doctors;
    unordered_map<string, vector<Doctor>> doctorsByName;
    unordered_map<string, vector<Doctor>> doctorsBySpec;

public:
    AppointmentManager() {
        patients = Patient::LoadFromFile();
        doctors = Doctor::LoadFromFile();
        // Populate doctor maps for fast search
        for (const auto& pair : doctors) {
            const Doctor& d = pair.second;
            doctorsByName[d.getName()].push_back(d);
            doctorsBySpec[d.getSpecialization()].push_back(d);
        }
    }

    void addPatient() {
        Patient p;
        p.AddPatient(patients);
    }

    void addDoctor() {
        Doctor d;
        string name, spec, date, time;
        int id;
        cin.ignore();
        cout << "Doctor name? ";
        getline(cin, name);
        cout << "Specialization? ";
        cin >> spec;
        cout << "Date (YYYY-MM-DD)? ";
        cin >> date;
        cout << "Time (HH:MM)? ";
        cin >> time;
        cout << "Doctor ID? ";
        cin >> id;
        d.setName(name);
        d.setSpecialization(spec);
        d.setDate(date);
        d.setTime(time);
        d.setId(id);
        doctors[id] = d;
        doctorsByName[name].push_back(d);
        doctorsBySpec[spec].push_back(d);
        d.SaveToFile();
    }

    void createAppointment(int patientId, int doctorId, const string& date, const string& time, int priority) {
        if (patients.find(patientId) == patients.end()) {
            throw runtime_error("Patient ID not found");
        }
        if (doctors.find(doctorId) == doctors.end()) {
            throw runtime_error("Doctor ID not found");
        }
        Appointment newApp(patientId, doctorId, date, time, priority);
        appointments.push(newApp);
        newApp.SaveToFile();
        newApp.Attach(new ReciptionNotifier());
        newApp.Attach(new DoctorNotifier());
        newApp.Attach(new PatientNotifier());
        newApp.Notify(newApp);
    }

    Appointment getNextAppointment() {
        if (appointments.empty()) {
            throw runtime_error("No appointments available");
        }
        Appointment top = appointments.top();
        appointments.pop();
        return top;
    }

    vector<Doctor> searchDoctors(ReservationStrategy* strategy) {
        ReservationContext context;
        context.setStrategy(strategy);
        return context.search(doctorsByName);
    }
};

// Notifiers
class ReciptionNotifier : public Observer {
public:
    void Update(const Appointment& a) override {
        cout << "[Reception] تم حجز موعد جديد للمريض ID: " << a.getPatientId()
             << " مع الطبيب ID: " << a.getDoctorId() << endl;
    }
};

class DoctorNotifier : public Observer {
public:
    void Update(const Appointment& a) override {
        cout << "[Doctor] إشعار: لديك حجز جديد في " << a.getDate()
             << " الساعة " << a.getTime() << endl;
    }
};

class PatientNotifier : public Observer {
public:
    void Update(const Appointment& a) override {
        cout << "[Patient] تم تأكيد موعدك بنجاح!\n";
    }
};

// Main for testing
int main() {
    AppointmentManager manager;
    // Example: Add a patient
    manager.addPatient();
    // Example: Add a doctor
    manager.addDoctor();
    // Example: Search for doctor by specialization
    SearchUsingSpecialization specSearch;
    vector<Doctor> matched = manager.searchDoctors(&specSearch);
    for (const auto& d : matched) {
        cout << "Found Doctor: " << d.getName() << ", ID: " << d.getId() << endl;
    }
    // Example: Create appointment
    try {
        manager.createAppointment(1, 1, "2025-06-08", "10:00", 1); // Priority 1 (emergency)
    } catch (const runtime_error& e) {
        cout << "Error: " << e.what() << endl;
    }
    // Example: Get next appointment
    try {
        Appointment next = manager.getNextAppointment();
        cout << "Next Appointment: Patient ID " << next.getPatientId()
             << ", Doctor ID " << next.getDoctorId() << endl;
    } catch (const runtime_error& e) {
        cout << "Error: " << e.what() << endl;
    }
    return 0;