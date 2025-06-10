#include <iostream>
using namespace std;
#include "Appointment.h"
int main()
{
  cout << "Welcome to .... Hospital Reservation system  \n";
  cout << "1-for reservation via speciliztion  \n";
  cout << "2-for reservation via DR's Name  \n";
  int choice;
  cin >> choice;
  unordered_map<int, Doctor> Doctors;

  Doctors[1] = Doctor("Ali", 1, "Cardiology");
  Doctors[2] = Doctor("Sara", 2, "Dermatology");
  Doctors[3] = Doctor("Ali", 3, "Neurology");

  ReservationContext context;
  SerachUsingName name;
  SearchUsingSpec spec;
  vector<Doctor> reservation;
  switch (choice)
  {
  case 1:
    // BookUsingSpecilization();
    context.SetReservationContext(&spec);
    reservation = context.search(Doctors);

    break;
  case 2:
    // BookUsingDrName();
    context.SetReservationContext(&name);
    reservation = context.search(Doctors);
    break;
  default:
    cout << "invalid reservation entry\n";
  }

  cout << "\nSearch Result:\n";
  for (const auto &doctor : reservation)
  {
    cout << "Doctor Name: " << doctor.getName()
         << ", ID: " << doctor.getId()
         << ", Specialization: " << doctor.getSpecilization() << endl;
  }
  Appointment manager;

    ReciptionNotifier reception;
    DoctorNotifier doctor;
    PatientNotifier patient;

    manager.Attach(&reception);
    manager.Attach(&doctor);
    manager.Attach(&patient);

    manager.createNewAppointment(101, 202, "2025-06-10", "10:00 AM");
}