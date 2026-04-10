import MoeSzyslakPython

from dataclasses import dataclass, field
from typing import List, Optional
from MoeSzyslakPython import *


@dataclass
class Stop:
    destination: str
    distance_from_prev: float  # in miles
    gas_used: float            # in gallons
    travel_time: int = field(default=0)  # in minutes, optional for future use

    @property
    def mpg(self) -> Optional[float]:
        if self.gas_used > 0:
            return self.distance_from_prev / self.gas_used
        return None


class Trip:
    _classID = 507734      

    def __init__(self):
        MoeSzyslakLibrary.VerifyLibrary()
        self._hObj = MoeSzyslakLibrary.CreateHandle(Trip._classID)
        print('Trip._hObj = ' + str(self._hObj))

        self.trip_name = "My Trip"  
        self._stops = []

    def AddStop(self, stopName, distance, gas_used):        
        self._stops.append(Stop(stopName, distance, gas_used))
        MoeSzyslakLibrary.InvokeHandle(self._hObj, 'AddStop')
        print("Stop added.")

    def edit_stop(self, index: int, destination: Optional[str] = None,
                  distance_from_prev: Optional[float] = None,
                  gas_used: Optional[float] = None) -> bool:
        if 0 <= index < len(self.stops):
            stop = elf._stops[index]
            if destination is not None:
                stop.destination = destination
            if distance_from_prev is not None:
                stop.distance_from_prev = distance_from_prev
            if gas_used is not None:
                stop.gas_used = gas_used
            return True
        return False

    def Name(self, strName=None):

        if (strName is not None):
            if strName:
                self.trip_name = strName
                MoeSzyslakLibrary.InvokeHandle(self._hObj, 'set "Trip Name" "{self._name}"')

        return self.trip_name

    @property
    def total_distance(self) -> float:
        return sum(s.distance_from_prev for s in self.stops)

    @property
    def total_gas(self) -> float:
        return sum(s.gas_used for s in self.stops)

    @property
    def average_mpg(self) -> Optional[float]:
        if self.total_gas > 0:
            return self.total_distance / self.total_gas
        return None

    def list_stops(self):
        if not self._stops:
            print("No stops added yet.")
            return
        print(f"\nStops for trip: {self.trip_name}")
        for i, stop in enumerate(self._stops):
            mpg_str = f"{stop.mpg:.2f} mpg" if stop.mpg is not None else "N/A"
            print(
                f"[{i}] Destination: {stop.destination} | "
                f"Distance from previous: {stop.distance_from_prev:.1f} mi | "
                f"Gas used: {stop.gas_used:.2f} gal | "
                f"MPG: {mpg_str}"
            )
        print()

    def print(self):
        print(f"Trip Name: {self.trip_name}")        



def prompt_float(prompt: str) -> float:
    while True:
        value = input(prompt).strip()
        try:
            return float(value)
        except ValueError:
            print("Please enter a valid number.")


def prompt_nonempty(prompt: str) -> str:
    while True:
        value = input(prompt).strip()
        if value:
            return value
        print("Value cannot be empty.")


def main_menu(trip: Trip):
    while True:
        print("\n=== Trip Planner ===")
        print(f"Current trip: {trip.trip_name}")
        print("1. Add stop")
        print("2. Edit stop")
        print("3. List stops")
        print("4. View trip summary")
        print("5. Rename trip")
        print("0. Exit")

        choice = input("Choose an option: ").strip()

        if choice == "1":
            print("\n=== Add Stop ===")
            destination = prompt_nonempty("Stop name: ")
            distance = prompt_float("Distance to next stop (miles): ")
            gas_used = prompt_float("Gas used for this leg (gallons): ")    
            trip.AddStop(destination, distance, gas_used)
        elif choice == "2":
            edit_stop_menu(trip)
        elif choice == "3":
            trip.list_stops()
        elif choice == "4":
            show_summary(trip)
        elif choice == "5":            
            print("\n=== Rename Trip ===")
            trip.Name(prompt_nonempty("New trip name: "))
            print("Trip renamed.")
        elif choice == "0":
            print("Goodbye!")
            break
        else:
            print("Invalid choice, try again.")


def edit_stop_menu(trip: Trip):
    if not trip.stops:
        print("No stops to edit.")
        return

    trip.list_stops()
    index_str = input("Enter the index of the stop to edit: ").strip()
    try:
        index = int(index_str)
    except ValueError:
        print("Invalid index.")
        return

    if not (0 <= index < len(trip.stops)):
        print("Index out of range.")
        return

    stop = trip.stops[index]
    print(f"\nEditing stop [{index}] - {stop.destination}")

    new_dest = input("New destination name (leave blank to keep current): ").strip()
    new_distance_str = input("New distance from previous (leave blank to keep current): ").strip()
    new_gas_str = input("New gas used (leave blank to keep current): ").strip()

    destination = stop.destination if new_dest == "" else new_dest

    if new_distance_str == "":
        distance = stop.distance_from_prev
    else:
        try:
            distance = float(new_distance_str)
        except ValueError:
            print("Invalid distance. Edit cancelled.")
            return

    if new_gas_str == "":
        gas_used = stop.gas_used
    else:
        try:
            gas_used = float(new_gas_str)
        except ValueError:
            print("Invalid gas value. Edit cancelled.")
            return

    success = trip.edit_stop(index, destination, distance, gas_used)
    if success:
        print("Stop updated.")
    else:
        print("Failed to update stop.")


def show_summary(trip: Trip):
    print("\n=== Trip Summary ===")
    print(f"Trip name: {trip.trip_name}")
    print(f"Total distance: {trip.total_distance:.1f} miles")
    print(f"Total gas used: {trip.total_gas:.2f} gallons")
    if trip.average_mpg is not None:
        print(f"Average MPG: {trip.average_mpg:.2f}")
    else:
        print("Average MPG: N/A (no gas usage recorded)")
    print()

def test_tripplanner():
    print("Running Trip Planner tests...")
    trip = Trip()
    trip.Name("Chicago Trip")

    trip.AddStop('Home', 100, 2.5)
    trip.print()
    print("All Trip Planner tests passed!")



if __name__ == "__main__":
    print("Welcome to the Trip Planner.")
    
    trip = Trip()
    trip.Name(input("Enter a name for your trip (default: 'My Trip'): ").strip())   
            
    main_menu(trip)
