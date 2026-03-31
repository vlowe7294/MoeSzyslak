
                           ┌────────────────────────┐
                           │      START GAME        │
                           └───────────┬────────────┘
                                       │
                                       ▼
                         ┌────────────────────────────┐
                         │ Load game data (JSON files) │
                         └───────────┬────────────────┘
                                     │
                                     ▼
                      ┌──────────────────────────────────┐
                      │ Does game data exist / valid?     │
                      └───────────┬───────────┬──────────┘
                                  │ Yes        │ No
                                  ▼            ▼
                     ┌─────────────────┐   ┌────────────────────┐
                     │ Use existing     │   │ Create new world / │
                     │ world + chars    │   │ empty templates     │
                     └───────────┬─────┘   └───────────┬────────┘
                                 │                      │
                                 └──────────┬───────────┘
                                            ▼
                         ┌────────────────────────────────┐
                         │ Show Main Menu:                 │
                         │ 1. Play Simulation              │
                         │ 2. Edit World                   │
                         │ 3. Edit Characters              │
                         │ 4. Edit Quests                  │
                         │ 5. Save & Quit                  │
                         └───────────┬────────────────────┘
                                     │
                                     ▼
                     ┌────────────────────────────────────────┐
                     │ User selects an option from Main Menu   │
                     └───────────┬────────────────────────────┘
                                 │
     ┌───────────────────────────┼──────────────────────────────────────────────┐
     │                           │                                              │
     ▼                           ▼                                              ▼

┌──────────────────┐   ┌────────────────────────┐                ┌──────────────────────────┐
│ PLAY SIMULATION   │   │ WORLD EDITOR           │                │ CHARACTER EDITOR         │
└───────┬───────────┘   └───────────┬────────────┘                └───────────┬────────────┘
        │                           │                                         │
        ▼                           ▼                                         ▼

┌──────────────────────────────┐   ┌──────────────────────────────┐   ┌──────────────────────────────┐
│ Begin Simulation Loop         │   │ Add/Edit/Delete Locations    │   │ Create/Edit/Delete Characters │
│ For each character:           │   │ Modify descriptions, exits   │   │ Modify stats, inventory, etc. │
│  - Choose action              │   │ Add monsters/resources       │   │ Assign quests                 │
│  - Move / fight / gather      │   │ Add events                   │   └──────────────────────────────┘
│  - Progress quest             │   └──────────────────────────────┘
│  - Update stats               │
│  - Log actions to player      │
└───────────┬──────────────────┘
            │
            ▼

┌──────────────────────────────────────────┐
│ After each turn:                         │
│  - Continue simulation?                  │
│  - Return to Main Menu?                  │
└───────────┬──────────────────────────────┘
            │
            ▼

     ┌──────────────────────────────┐
     │ QUEST EDITOR                 │
     └───────────┬──────────────────┘
                 │
                 ▼
     ┌──────────────────────────────┐
     │ Create/Edit/Delete Quests    │
     │ Set goals, rewards, triggers │
     └──────────────────────────────┘

                 ▼
     ┌──────────────────────────────┐
     │ Return to Main Menu          │
     └───────────┬──────────────────┘
                 │
                 ▼
     ┌──────────────────────────────┐
     │ SAVE & QUIT                  │
     │ Write JSON files to disk     │
     └──────────────────────────────┘
