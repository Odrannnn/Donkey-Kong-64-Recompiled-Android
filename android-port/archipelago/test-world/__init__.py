"""Developer-only AP world. Not the DK64 Randomizer world or a full game randomizer."""
from BaseClasses import Item, ItemClassification, Location, Region
from worlds.AutoWorld import World

GAME = "DK64 Recomp Integration Test"
PROFILE = "dk64-recomp-first-moves-japes-v1"

class TestItem(Item):
    game = GAME

class TestLocation(Location):
    game = GAME

class DK64RecompTestWorld(World):
    """Two vanilla Japes checks for verifying the experimental recomp bridge.

    All vanilla rewards and progression remain. Do not use as a campaign randomizer.
    """
    game = GAME
    item_name_to_id = {"Baboon Blast": 14041104, "Chimpy Charge": 14041108,
                       "Orangstand": 14041112, "Mini Monkey": 14041116, "Hunky Chunky": 14041120}
    location_name_to_id = {"Japes in Front of Diddy Cage": 14041180, "Japes Free Diddy Item": 14041181}
    required_client_version = (0, 6, 5)
    topology_present = False

    def create_regions(self):
        menu = Region("Menu", self.player, self.multiworld)
        japes = Region("Jungle Japes (vanilla progression)", self.player, self.multiworld)
        menu.connect(japes)
        japes.locations = [TestLocation(self.player, name, code, japes) for name, code in self.location_name_to_id.items()]
        self.multiworld.regions += [menu, japes]

    def create_item(self, name):
        return TestItem(name, ItemClassification.progression, self.item_name_to_id[name], self.player)

    def create_items(self):
        self.multiworld.itempool += [self.create_item("Baboon Blast"), self.create_item("Chimpy Charge")]

    def set_rules(self):
        # Both checks are reachable through unchanged vanilla progression. AP is not required to reach them.
        self.multiworld.completion_condition[self.player] = lambda state: state.has_all(("Baboon Blast", "Chimpy Charge"), self.player)

    def fill_slot_data(self):
        return {"recomp_profile": PROFILE, "Version": "1.5.8"}
