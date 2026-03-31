import MoeSzyslakPython
import json

from MoeSzyslakPython import *

class Room:
    ID = 1
    OPPOSITE = {
    "north": "south",
    "south": "north",
    "east": "west",
    "west": "east",
    "up": "down",
    "down": "up"
    }
    def __init__(self):
        self._exits = {}
        self._id = Room.ID
        Room.ID = Room.ID + 1

    def add_exit(self, exitRoom, direction):
        self._exits[direction] = exitRoom

        if direction in Room.OPPOSITE:
            reverse = Room.OPPOSITE[direction]
            exitRoom._exits[reverse] = self

    def Print(self):
        print(f"Room: " + str(self._id))
        print("Exits:")
        for direction, target in self._exits.items():
            print(direction + " Room # " + str(target._id))



class Module:
    def __init__(self, hNWNObj):
        self._hObj = hNWNObj
        self._name = "Eye of the Beholder"
        self._startRoom = Room()
        self._startRoom.add_exit(Room(), "north")

    def Update(self):
        MoeSzyslakLibrary.InvokeHandle(self._hObj, 'Module get "Module Name"');
        self._name = MoeSzyslakLibrary.GetReturnString(self._hObj)

    def Commit(self):
        MoeSzyslakLibrary.InvokeHandle(self._hObj, 'Module set "Module Name" "' + self._name + '"')

    def Name(self, name=None):
        if name is not None:
            self._name = name
        return self._name

    def list_rooms(self):
        print(f"\n=== ROOMS IN {self._name} ===")
        self._startRoom.Print()

    

class Nocturne:
    _classID = 977093    

    _moduledata = {
        "name": "New Module",
        "rooms": {         
            "forest": {
                "description": "A quiet forest clearing.",
                "exits": {"north": "mountain", "east": "river"}
            },
            "river": {
                "description": "A rushing river with cold water.",
                "exits": {"west": "forest"}
            }
        }
    }

    _gameFile = "game.json"

    def init():
        MoeSzyslakLibrary.VerifyLibrary()
        Neverwinter._hObj = MoeSzyslakLibrary.CreateHandle(Neverwinter._classID)
        print('Neverwinter._hObj = ' + str(Neverwinter._hObj))
        Nocturne._module = Module(Neverwinter._hObj)
        
        
    def show_intro():
        print(MoeSzyslakLibrary.Version())
        print("\n=== NOCTURNE LEGENDS ===\n")
        print("In the twilight lands of Nocturne, where moonlit forests whisper forgotten tales")
        print("and ancient ruins hum with sleeping magic, every wanderer carries the spark of a legend.")
        print("Here, fates are shaped not by prophecy, but by the choices of those who dare to roam.")
        print("Welcome, traveler, to a realm where stories awaken in the shadows.\n")

    def main_menu():
        play = True
        while play:
            print("\n=== FANTASY SIM MAIN MENU ===")
            print("1. Play Simulation")
            print("2. Edit World")
            print("3. Edit Characters")
            print("4. Edit Quests")
            print("5. Save & Quit")

            choice = input("> ").strip()

            if choice == "1":
                pass
            elif choice == "2":
                Nocturne.edit_world()
            elif choice == "3":
                pass
            elif choice == "4":
                pass
            elif choice == "5":
                Nocturne.save_game()
                print("Goodbye!")
                play = False
            else:
                print("Invalid choice. Try again.")

    def main():
        Nocturne.load_game()
        Nocturne.show_intro()
        Nocturne.main_menu()  
        
    def rename_world(new_name):
        if new_name:
            Nocturne._module.Name(new_name)
            Nocturne._module.Commit()
            print(f"World renamed to '{new_name}'.")
        else:
            print("Name cannot be empty.")

    def edit_world():
        choice = 0

        while choice < 1 or choice > 6:
            print("\n=== WORLD EDITOR ===")
            print(f"World Name: {Nocturne._module.Name()}")
            print("1. Rename World")
            print("2. List Rooms")
            print("3. Add Room")
            print("4. Edit Room")
            print("5. Delete Room")
            print("6. Back to Main Menu")

            choice = input("> ").strip()

            if choice == "1":
                Nocturne.rename_world(input("Enter new world name: ").strip())
                choice = 0
            elif choice == "2":
                Nocturne.list_rooms()
                choice = 0  
            elif choice == "6":
                choice = 6            
            else:
                print("Invalid choice.")
                choice = 0

    def save_game():
        Nocturne._module.Update()
        Nocturne._moduledata['name'] = Nocturne._module.Name() 
        with open(Nocturne._gameFile, "w") as f:
            json.dump(Nocturne._moduledata, f, indent=4)
        print("Game saved.")

    def load_game():
        try:
            with open(Nocturne._gameFile, "r", encoding="utf-8") as f:
                Nocturne._moduledata = json.load(f)
        except (FileNotFoundError, json.JSONDecodeError):
            print("No save found. Creating new game.")            

        # Ensure world structure is complete
        if "name" not in Nocturne._moduledata:
            Nocturne._moduledata["name"] = 'New Module'

        Nocturne._module.Name(Nocturne._moduledata['name'])   

    def list_rooms():
        Nocturne._module.list_rooms()


def test_nocturne():
    print("Running Nocturne tests...")
    Nocturne.init()
    Nocturne.show_intro()

    Nocturne.rename_world('Out of the Abyss')
    Nocturne.save_game()


    print("All Nocturne tests passed!")

if __name__ == "__main__": 
    Nocturne.init()
    #Nocturne.main()
    Nocturne.edit_world()