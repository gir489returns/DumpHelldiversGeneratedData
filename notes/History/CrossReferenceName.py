import json
import os

# Function to load JSON data from a file
def load_json(file_path):
    with open(file_path, 'r') as file:
        return json.load(file)

# Function to load IDs and descriptions from a file
def load_ids(file_path):
    ids = []
    with open(file_path, 'r') as file:
        for line in file:
            parts = line.strip().split(' // ')
            ids.append((int(parts[0][1:]), parts[1]))
    return ids

# Load datasets from disk
data_A = load_json('1.001.003.json')
data_B = load_json('1.001.100.json')

# Load IDs and descriptions from file
ids_and_descriptions = load_ids('WeaponList.txt')

# Function to compare two entries based on m_name
def compare_entries(entry1, entry2):
    return entry1["m_name"] == entry2["m_name"]

# Function to find new positions
def find_new_positions(data_A, data_B):
    new_positions = {}
    used_ids = set()
    for entry_A in data_A:
        start_index = next((index for (index, d) in enumerate(data_B) if d["m_id"] == entry_A["m_id"]), None)
        if start_index is not None:
            for entry_B in data_B[start_index:]:
                if compare_entries(entry_A, entry_B) and entry_B["m_id"] not in used_ids:
                    new_positions[entry_A["m_id"]] = entry_B["m_id"]
                    used_ids.add(entry_B["m_id"])
                    break
    return new_positions

# Find new positions
new_positions = find_new_positions(data_A, data_B)

# Update IDs and descriptions
updated_ids_and_descriptions = []
for old_id, description in ids_and_descriptions:
    new_id = new_positions.get(old_id, old_id)
    if new_id == old_id:
        print(f"Error: ID {old_id} was not updated.")
    updated_ids_and_descriptions.append((new_id, description))

# Output the updated IDs and descriptions
for new_id, description in updated_ids_and_descriptions:
    print(f"#{new_id} // {description}")

os.system("pause")
