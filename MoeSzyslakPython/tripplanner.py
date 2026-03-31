from dataclasses import dataclass, field
from typing import List, Optional


@dataclass
class Stop:
    destination: str
    distance_from_prev: float  # in miles
    gas_used: float            # in gallons

    @property
    def mpg(self) -> Optional[float]:
        if self.gas_used > 0:
            return self.distance_from_prev / self.gas_used
        return None


@dataclass
class Trip:
    name: str
    stops: List[Stop] = field(default_factory=list)

    def add_stop(self, destination: str, distance_from_prev: float, gas_used: float):
        self.stops.append(Stop(destination, distance_from_prev, gas_used))

    def edit_stop(self, index: int, destination: Optional[str] = None,
                  distance_from_prev: Optional[float] = None,
                  gas_used: Optional[float] = None) -> bool:
        if 0 <= index < len(self.stops):
            stop = self.stops[index]
            if destination is not None:
                stop.destination = destination
            if distance_from_prev is not None:
                stop.distance_from_prev = distance_from_prev
            if gas_used is not None:
                stop.gas_used = gas_used
            return True
        return False

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
        if not self.stops:
            print("No stops added yet.")
            return
        print(f"\nStops for trip: {self.name}")
        for i, stop in enumerate(self.stops):
            mpg_str = f"{stop.mpg:.2f} mpg" if stop.mpg is not None else "N/A"
            print(
                f"[{i}] Destination: {stop.destination} | "
                f"Distance from previous: {stop.distance_from_prev:.1f} mi | "
                f"Gas used: {stop.gas_used:.2f} gal | "
                f"MPG: {mpg_str}"
            )
        print()


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
        print(f"Current trip: {trip.name}")
        print("1. Add stop")
        print("2. Edit stop")
        print("3. List stops")
        print("4. View trip summary")
        print("5. Rename trip")
        print("0. Exit")

        choice = input("Choose an option: ").strip()

        if choice == "1":
            add_stop_menu(trip)
        elif choice == "2":
            edit_stop_menu(trip)
        elif choice == "3":
            trip.list_stops()
        elif choice == "4":
            show_summary(trip)
        elif choice == "5":
            rename_trip(trip)
        elif choice == "0":
            print("Goodbye!")
            break
        else:
            print("Invalid choice, try again.")


def add_stop_menu(trip: Trip):
    print("\n=== Add Stop ===")
    destination = prompt_nonempty("Destination name: ")
    distance = prompt_float("Distance from previous stop (miles): ")
    gas_used = prompt_float("Gas used for this leg (gallons): ")
    trip.add_stop(destination, distance, gas_used)
    print("Stop added.")


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
    print(f"Trip name: {trip.name}")
    print(f"Total distance: {trip.total_distance:.1f} miles")
    print(f"Total gas used: {trip.total_gas:.2f} gallons")
    if trip.average_mpg is not None:
        print(f"Average MPG: {trip.average_mpg:.2f}")
    else:
        print("Average MPG: N/A (no gas usage recorded)")
    print()


def rename_trip(trip: Trip):
    print("\n=== Rename Trip ===")
    new_name = prompt_nonempty("New trip name: ")
    trip.name = new_name
    print("Trip renamed.")


if __name__ == "__main__":
    print("Welcome to the Trip Planner.")
    trip_name = input("Enter a name for your trip (default: 'My Trip'): ").strip()
    if not trip_name:
        trip_name = "My Trip"
    trip = Trip(trip_name)
    main_menu(trip)
