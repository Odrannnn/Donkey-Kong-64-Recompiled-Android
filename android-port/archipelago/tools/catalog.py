"""Verified subset of 2dos/DK64-Randomizer, pinned in upstream.json.

IDs are copied from archipelago/client/items.py and ap_check_ids.py.
Flags are from check_flag_locations.py. See licenses/DK64-Randomizer.txt.
The game name is deliberately distinct so an ordinary DK64 slot cannot connect.
"""
GAME = "DK64 Recomp Integration Test"
PROFILE = "dk64-recomp-first-moves-japes-v1"
WORLD_VERSION = "1.5.8"
ITEMS = {
    14041104: ("Baboon Blast", 0),
    14041108: ("Chimpy Charge", 1),
    14041112: ("Orangstand", 2),
    14041116: ("Mini Monkey", 3),
    14041120: ("Hunky Chunky", 4),
}
LOCATIONS = {
    14041180: ("Japes in Front of Diddy Cage", 4, 0),
    14041181: ("Japes Free Diddy Item", 5, 1),
}
CHECK_MASK = 3
MOVE_MASK = 31
