/* Generated from pinned DK64 Randomizer; MIT notice: licenses/DK64-Randomizer.txt. */
#pragma once
#define AP_CATALOG_SHA256 "7b4db5979d1741956a83df50ca0463eaa281e360a57347f2909101067253e2ac"
#define AP_LOCATION_COUNT 868
#define AP_LOCATION_WORDS 28
typedef struct { unsigned int id; unsigned short flag; unsigned char detector; } ap_location_def;
enum { AP_DETECT_STOCK_FLAG, AP_DETECT_EVENT, AP_DETECT_SHOP, AP_DETECT_ENEMY };
static const ap_location_def ap_locations[AP_LOCATION_COUNT] = {
    {14041140u, 381, AP_DETECT_STOCK_FLAG}, /* Isles Japes Lobby Entrance Item */
    {14041141u, 420, AP_DETECT_STOCK_FLAG}, /* Isles Tiny Feather Cage */
    {14041142u, 425, AP_DETECT_STOCK_FLAG}, /* Isles Tiny Saxophone Pad */
    {14041143u, 421, AP_DETECT_STOCK_FLAG}, /* Isles Lanky Grape Cage */
    {14041144u, 422, AP_DETECT_STOCK_FLAG}, /* Isles Chunky Pineapple Cage */
    {14041145u, 424, AP_DETECT_STOCK_FLAG}, /* Isles Chunky Triangle Pad */
    {14041146u, 431, AP_DETECT_STOCK_FLAG}, /* Isles Chunky Pound the X */
    {14041147u, 606, AP_DETECT_STOCK_FLAG}, /* Isles Fairy (Small Island) */
    {14041148u, 607, AP_DETECT_STOCK_FLAG}, /* Isles Fairy (Upper Krem Isle) */
    {14041149u, 429, AP_DETECT_STOCK_FLAG}, /* Isles Lanky Sprint Cage */
    {14041150u, 377, AP_DETECT_STOCK_FLAG}, /* The Banana Fairy's Gift */
    {14041151u, 301, AP_DETECT_STOCK_FLAG}, /* Returning the Banana Fairies */
    {14041152u, 398, AP_DETECT_STOCK_FLAG}, /* Isles Lanky Japes Instrument */
    {14041153u, 402, AP_DETECT_STOCK_FLAG}, /* Isles Tiny Aztec Lobby Barrel */
    {14041154u, 419, AP_DETECT_STOCK_FLAG}, /* Isles Donkey Coconut Cage */
    {14041155u, 416, AP_DETECT_STOCK_FLAG}, /* Isles Diddy Snides Spring Barrel */
    {14041156u, 615, AP_DETECT_STOCK_FLAG}, /* Isles Battle Arena 1 (Snide's Room: Under Rock) */
    {14041157u, 404, AP_DETECT_STOCK_FLAG}, /* Isles Donkey Bongos Pad */
    {14041158u, 507, AP_DETECT_STOCK_FLAG}, /* Isles Kasplat: Factory Lobby Box */
    {14041159u, 593, AP_DETECT_STOCK_FLAG}, /* Isles Fairy (Factory Lobby) */
    {14041160u, 403, AP_DETECT_STOCK_FLAG}, /* Isles Tiny Galleon Lobby Swim */
    {14041161u, 508, AP_DETECT_STOCK_FLAG}, /* Isles Kasplat: Galleon Lobby */
    {14041162u, 423, AP_DETECT_STOCK_FLAG}, /* Isles Diddy Peanut Cage */
    {14041163u, 428, AP_DETECT_STOCK_FLAG}, /* Isles Diddy Summit Barrel */
    {14041164u, 614, AP_DETECT_STOCK_FLAG}, /* Isles Battle Arena 2 (Fungi Lobby: Gorilla Gone Box) */
    {14041165u, 594, AP_DETECT_STOCK_FLAG}, /* Isles Fairy (Forest Lobby) */
    {14041166u, 411, AP_DETECT_STOCK_FLAG}, /* Isles Donkey Caves Lava */
    {14041167u, 410, AP_DETECT_STOCK_FLAG}, /* Isles Diddy Guitar Pad */
    {14041168u, 506, AP_DETECT_STOCK_FLAG}, /* Isles Kasplat: Caves Lobby Punch */
    {14041169u, 415, AP_DETECT_STOCK_FLAG}, /* Isles Lanky Castle Lobby Barrel */
    {14041170u, 505, AP_DETECT_STOCK_FLAG}, /* Isles Kasplat: Castle Lobby */
    {14041171u, 406, AP_DETECT_STOCK_FLAG}, /* Isles Chunky Helm Lobby Barrel */
    {14041172u, 504, AP_DETECT_STOCK_FLAG}, /* Isles Kasplat: Helm Lobby */
    {14041173u, 432, AP_DETECT_EVENT}, /* Banana Hoard */
    {14041174u, 549, AP_DETECT_STOCK_FLAG}, /* Japes Donkey Medal */
    {14041175u, 550, AP_DETECT_STOCK_FLAG}, /* Japes Diddy Medal */
    {14041176u, 551, AP_DETECT_STOCK_FLAG}, /* Japes Lanky Medal */
    {14041177u, 552, AP_DETECT_STOCK_FLAG}, /* Japes Tiny Medal */
    {14041178u, 553, AP_DETECT_STOCK_FLAG}, /* Japes Chunky Medal */
    {14041179u, 6, AP_DETECT_STOCK_FLAG}, /* Japes Cage: Diddy Kong */
    {14041180u, 4, AP_DETECT_STOCK_FLAG}, /* Japes in Front of Diddy Cage */
    {14041181u, 5, AP_DETECT_STOCK_FLAG}, /* Japes Free Diddy Item */
    {14041182u, 20, AP_DETECT_STOCK_FLAG}, /* Japes Donkey Floor Cage Banana */
    {14041183u, 3, AP_DETECT_STOCK_FLAG}, /* Japes Donkey Baboon Blast */
    {14041184u, 18, AP_DETECT_STOCK_FLAG}, /* Japes Diddy Timed Cage Banana */
    {14041185u, 23, AP_DETECT_STOCK_FLAG}, /* Japes Diddy Top of Mountain */
    {14041186u, 19, AP_DETECT_STOCK_FLAG}, /* Japes Lanky Timed Cage Banana */
    {14041187u, 21, AP_DETECT_STOCK_FLAG}, /* Japes Tiny Timed Cage Banana */
    {14041188u, 25, AP_DETECT_STOCK_FLAG}, /* Japes Chunky Boulder */
    {14041189u, 22, AP_DETECT_STOCK_FLAG}, /* Japes Chunky Timed Cage Banana */
    {14041190u, 609, AP_DETECT_STOCK_FLAG}, /* Japes Battle Arena (Near Low Hill Shop) */
    {14041191u, 31, AP_DETECT_STOCK_FLAG}, /* Japes Diddy Peanut Tunnel */
    {14041192u, 1, AP_DETECT_STOCK_FLAG}, /* Japes Lanky Grape Gate Barrel */
    {14041193u, 2, AP_DETECT_STOCK_FLAG}, /* Japes Tiny Feather Gate Barrel */
    {14041194u, 469, AP_DETECT_STOCK_FLAG}, /* Japes Kasplat: Hive Tunnel Lower */
    {14041195u, 472, AP_DETECT_STOCK_FLAG}, /* Japes Kasplat: Hive Tunnel Upper */
    {14041196u, 8, AP_DETECT_STOCK_FLAG}, /* Japes Tiny Stump */
    {14041197u, 28, AP_DETECT_STOCK_FLAG}, /* Japes Chunky Giant Bonus Barrel */
    {14041198u, 9, AP_DETECT_STOCK_FLAG}, /* Japes Tiny Beehive */
    {14041199u, 11, AP_DETECT_STOCK_FLAG}, /* Japes Lanky Slope Barrel */
    {14041200u, 470, AP_DETECT_STOCK_FLAG}, /* Japes Kasplat: Tunnel Near Painting Room */
    {14041201u, 471, AP_DETECT_STOCK_FLAG}, /* Japes Kasplat: By Lanky Slope Bonus */
    {14041202u, 589, AP_DETECT_STOCK_FLAG}, /* Japes Fairy (Rambi Door Pool) */
    {14041203u, 10, AP_DETECT_STOCK_FLAG}, /* Japes Lanky Painting Room Zingers */
    {14041204u, 590, AP_DETECT_STOCK_FLAG}, /* Japes Fairy (Painting Room) */
    {14041205u, 24, AP_DETECT_STOCK_FLAG}, /* Japes Diddy Minecart */
    {14041206u, 12, AP_DETECT_STOCK_FLAG}, /* Japes Chunky Underground */
    {14041207u, 473, AP_DETECT_STOCK_FLAG}, /* Japes Kasplat: Underground */
    {14041208u, 26, AP_DETECT_STOCK_FLAG}, /* Japes Boss Defeated */
    {14041209u, 554, AP_DETECT_STOCK_FLAG}, /* Aztec Donkey Medal */
    {14041210u, 555, AP_DETECT_STOCK_FLAG}, /* Aztec Diddy Medal */
    {14041211u, 556, AP_DETECT_STOCK_FLAG}, /* Aztec Lanky Medal */
    {14041212u, 557, AP_DETECT_STOCK_FLAG}, /* Aztec Tiny Medal */
    {14041213u, 558, AP_DETECT_STOCK_FLAG}, /* Aztec Chunky Medal */
    {14041214u, 51, AP_DETECT_STOCK_FLAG}, /* Aztec Donkey Free Llama Blast */
    {14041215u, 49, AP_DETECT_STOCK_FLAG}, /* Aztec Chunky Vases */
    {14041216u, 474, AP_DETECT_STOCK_FLAG}, /* Aztec Kasplat: Behind DK Stone Door */
    {14041217u, 475, AP_DETECT_STOCK_FLAG}, /* Aztec Kasplat: On Tiny Temple */
    {14041218u, 65, AP_DETECT_STOCK_FLAG}, /* Aztec Tiny Klaptrap Room */
    {14041219u, 64, AP_DETECT_STOCK_FLAG}, /* Aztec Chunky Klaptrap Room */
    {14041220u, 66, AP_DETECT_STOCK_FLAG}, /* Aztec Cage: Tiny Kong */
    {14041221u, 67, AP_DETECT_STOCK_FLAG}, /* Aztec Free Tiny Item */
    {14041222u, 68, AP_DETECT_STOCK_FLAG}, /* Aztec Lanky Vulture Shooting */
    {14041223u, 610, AP_DETECT_STOCK_FLAG}, /* Aztec Battle Arena (Tiny Temple: Vulture Room) */
    {14041224u, 62, AP_DETECT_STOCK_FLAG}, /* Aztec Donkey Sealed Quicksand Tunnel Barrel */
    {14041225u, 54, AP_DETECT_STOCK_FLAG}, /* Aztec Diddy Ram Gongs */
    {14041226u, 63, AP_DETECT_STOCK_FLAG}, /* Aztec Diddy Vulture Race */
    {14041227u, 52, AP_DETECT_STOCK_FLAG}, /* Aztec Chunky Giant Caged Barrel */
    {14041228u, 477, AP_DETECT_STOCK_FLAG}, /* Aztec Kasplat: Hunky Chunky Barrel */
    {14041229u, 57, AP_DETECT_STOCK_FLAG}, /* Aztec Donkey 5 Door Temple */
    {14041230u, 56, AP_DETECT_STOCK_FLAG}, /* Aztec Diddy 5 Door Temple */
    {14041231u, 60, AP_DETECT_STOCK_FLAG}, /* Aztec Lanky 5 Door Temple */
    {14041232u, 58, AP_DETECT_STOCK_FLAG}, /* Aztec Tiny 5 Door Temple */
    {14041233u, 601, AP_DETECT_STOCK_FLAG}, /* Aztec Fairy (Tiny 5-Door Temple) */
    {14041234u, 59, AP_DETECT_STOCK_FLAG}, /* Aztec Chunky 5 Door Temple */
    {14041235u, 478, AP_DETECT_STOCK_FLAG}, /* Aztec Kasplat: Chunky 5-Door Temple */
    {14041236u, 75, AP_DETECT_STOCK_FLAG}, /* Aztec Tiny Beetle Race */
    {14041237u, 70, AP_DETECT_STOCK_FLAG}, /* Aztec Cage: Lanky Kong */
    {14041238u, 77, AP_DETECT_STOCK_FLAG}, /* Aztec Free Lanky Item */
    {14041239u, 73, AP_DETECT_STOCK_FLAG}, /* Aztec Lanky Llama Temple Barrel */
    {14041240u, 72, AP_DETECT_STOCK_FLAG}, /* Aztec Lanky Matching Game */
    {14041241u, 600, AP_DETECT_STOCK_FLAG}, /* Aztec Fairy (Llama Temple) */
    {14041242u, 71, AP_DETECT_STOCK_FLAG}, /* Aztec Tiny Llama Temple Lava Pedestals */
    {14041243u, 476, AP_DETECT_STOCK_FLAG}, /* Aztec Kasplat: Llama Temple Lava */
    {14041244u, 74, AP_DETECT_STOCK_FLAG}, /* Aztec Boss Defeated */
    {14041245u, 559, AP_DETECT_STOCK_FLAG}, /* Factory Donkey Medal */
    {14041246u, 560, AP_DETECT_STOCK_FLAG}, /* Factory Diddy Medal */
    {14041247u, 561, AP_DETECT_STOCK_FLAG}, /* Factory Lanky Medal */
    {14041248u, 562, AP_DETECT_STOCK_FLAG}, /* Factory Tiny Medal */
    {14041249u, 563, AP_DETECT_STOCK_FLAG}, /* Factory Chunky Medal */
    {14041250u, 122, AP_DETECT_STOCK_FLAG}, /* Factory Donkey Number Game */
    {14041251u, 135, AP_DETECT_STOCK_FLAG}, /* Factory Diddy Block Tower */
    {14041252u, 137, AP_DETECT_STOCK_FLAG}, /* Factory Lanky Testing Room Barrel */
    {14041253u, 124, AP_DETECT_STOCK_FLAG}, /* Factory Tiny Dartboard */
    {14041254u, 483, AP_DETECT_STOCK_FLAG}, /* Factory Kasplat: Block Tower */
    {14041255u, 602, AP_DETECT_STOCK_FLAG}, /* Factory Fairy (Number Game) */
    {14041256u, 591, AP_DETECT_STOCK_FLAG}, /* Factory Fairy (Testing Shop) */
    {14041257u, 126, AP_DETECT_STOCK_FLAG}, /* Factory Diddy Charge Enemies */
    {14041258u, 125, AP_DETECT_STOCK_FLAG}, /* Factory Lanky Piano Game */
    {14041259u, 127, AP_DETECT_STOCK_FLAG}, /* Factory Chunky Toy Monster */
    {14041260u, 481, AP_DETECT_STOCK_FLAG}, /* Factory Kasplat: Research and Development */
    {14041261u, 611, AP_DETECT_STOCK_FLAG}, /* Factory Battle Arena (Under R&D Grate) */
    {14041262u, 139, AP_DETECT_STOCK_FLAG}, /* Factory Tiny Car Race */
    {14041263u, 134, AP_DETECT_STOCK_FLAG}, /* Factory Diddy Storage Room Barrel */
    {14041264u, 112, AP_DETECT_STOCK_FLAG}, /* Factory Donkey Power Hut */
    {14041265u, 117, AP_DETECT_STOCK_FLAG}, /* Factory Cage: Chunky Kong */
    {14041266u, 132, AP_DETECT_STOCK_FLAG}, /* DK Arcade Round 2 */
    {14041267u, 130, AP_DETECT_STOCK_FLAG}, /* Factory Donkey DK Arcade Round 1 */
    {14041268u, 118, AP_DETECT_STOCK_FLAG}, /* Factory Free Chunky Item */
    {14041269u, 123, AP_DETECT_STOCK_FLAG}, /* Factory Tiny Mini by Arcade */
    {14041270u, 121, AP_DETECT_STOCK_FLAG}, /* Factory Chunky Dark Room */
    {14041271u, 136, AP_DETECT_STOCK_FLAG}, /* Factory Chunky Barrel by Arcade */
    {14041272u, 480, AP_DETECT_STOCK_FLAG}, /* Factory Kasplat: Base of Production */
    {14041273u, 482, AP_DETECT_STOCK_FLAG}, /* Factory Kasplat: Pole to Arcade */
    {14041274u, 128, AP_DETECT_STOCK_FLAG}, /* Factory Donkey Crusher Room */
    {14041275u, 113, AP_DETECT_STOCK_FLAG}, /* Factory Diddy Production Spring */
    {14041276u, 115, AP_DETECT_STOCK_FLAG}, /* Factory Lanky Production Handstand */
    {14041277u, 116, AP_DETECT_STOCK_FLAG}, /* Factory Tiny Production Twirl */
    {14041278u, 114, AP_DETECT_STOCK_FLAG}, /* Factory Chunky Production Timer */
    {14041279u, 479, AP_DETECT_STOCK_FLAG}, /* Factory Kasplat: Upper Production Pipe */
    {14041280u, 138, AP_DETECT_STOCK_FLAG}, /* Factory Boss Defeated */
    {14041281u, 564, AP_DETECT_STOCK_FLAG}, /* Galleon Donkey Medal */
    {14041282u, 565, AP_DETECT_STOCK_FLAG}, /* Galleon Diddy Medal */
    {14041283u, 566, AP_DETECT_STOCK_FLAG}, /* Galleon Lanky Medal */
    {14041284u, 567, AP_DETECT_STOCK_FLAG}, /* Galleon Tiny Medal */
    {14041285u, 568, AP_DETECT_STOCK_FLAG}, /* Galleon Chunky Medal */
    {14041286u, 182, AP_DETECT_STOCK_FLAG}, /* Galleon Chunky Chest */
    {14041287u, 487, AP_DETECT_STOCK_FLAG}, /* Galleon Kasplat: Past Vines */
    {14041288u, 612, AP_DETECT_STOCK_FLAG}, /* Galleon Battle Arena (Under Cavern Shop) */
    {14041289u, 592, AP_DETECT_STOCK_FLAG}, /* Galleon Fairy (In Punch Chest) */
    {14041290u, 154, AP_DETECT_STOCK_FLAG}, /* Galleon Chunky Cannon Game */
    {14041291u, 486, AP_DETECT_STOCK_FLAG}, /* Galleon Kasplat: Cannon Game Room */
    {14041292u, 204, AP_DETECT_STOCK_FLAG}, /* Galleon Diddy Top of Lighthouse */
    {14041293u, 192, AP_DETECT_STOCK_FLAG}, /* Galleon Lanky Enguarde Chest */
    {14041294u, 485, AP_DETECT_STOCK_FLAG}, /* Galleon Kasplat: Lighthouse Alcove */
    {14041295u, 157, AP_DETECT_STOCK_FLAG}, /* Galleon Donkey Lighthouse */
    {14041296u, 191, AP_DETECT_STOCK_FLAG}, /* Galleon Tiny Mermaid Reward */
    {14041297u, 166, AP_DETECT_STOCK_FLAG}, /* Galleon Chunky Seasick */
    {14041298u, 193, AP_DETECT_STOCK_FLAG}, /* Galleon Donkey Free the Seal */
    {14041299u, 488, AP_DETECT_STOCK_FLAG}, /* Galleon Kasplat: Musical Cactus */
    {14041300u, 165, AP_DETECT_STOCK_FLAG}, /* Galleon Donkey Seal Race */
    {14041301u, 163, AP_DETECT_STOCK_FLAG}, /* Galleon Diddy Gold Tower Barrel */
    {14041302u, 164, AP_DETECT_STOCK_FLAG}, /* Galleon Lanky Gold Tower Barrel */
    {14041303u, 484, AP_DETECT_STOCK_FLAG}, /* Galleon Kasplat: Diddy Gold Tower */
    {14041304u, 202, AP_DETECT_STOCK_FLAG}, /* Galleon Tiny Submarine Barrel */
    {14041305u, 167, AP_DETECT_STOCK_FLAG}, /* Galleon Diddy Mechfish */
    {14041306u, 183, AP_DETECT_STOCK_FLAG}, /* Galleon Lanky 2 Door Ship */
    {14041307u, 184, AP_DETECT_STOCK_FLAG}, /* Galleon Tiny 2 Door Ship */
    {14041308u, 200, AP_DETECT_STOCK_FLAG}, /* Galleon Donkey 5 Door Ship */
    {14041309u, 198, AP_DETECT_STOCK_FLAG}, /* Galleon Diddy 5 Door Ship */
    {14041310u, 199, AP_DETECT_STOCK_FLAG}, /* Galleon Lanky 5 Door Ship */
    {14041311u, 201, AP_DETECT_STOCK_FLAG}, /* Galleon Tiny 5 Door Ship */
    {14041312u, 603, AP_DETECT_STOCK_FLAG}, /* Galleon Fairy (In Tiny's 5-Door Ship) */
    {14041313u, 197, AP_DETECT_STOCK_FLAG}, /* Galleon Chunky 5 Door Ship */
    {14041314u, 186, AP_DETECT_STOCK_FLAG}, /* Treasure Chest Far Left Clam */
    {14041315u, 187, AP_DETECT_STOCK_FLAG}, /* Treasure Chest Center Clam */
    {14041316u, 188, AP_DETECT_STOCK_FLAG}, /* Treasure Chest Far Right Clam */
    {14041317u, 189, AP_DETECT_STOCK_FLAG}, /* Treasure Chest Close Right Clam */
    {14041318u, 190, AP_DETECT_STOCK_FLAG}, /* Treasure Chest Close Left Clam */
    {14041319u, 168, AP_DETECT_STOCK_FLAG}, /* Galleon Boss Defeated */
    {14041320u, 569, AP_DETECT_STOCK_FLAG}, /* Forest Donkey Medal */
    {14041321u, 570, AP_DETECT_STOCK_FLAG}, /* Forest Diddy Medal */
    {14041322u, 571, AP_DETECT_STOCK_FLAG}, /* Forest Lanky Medal */
    {14041323u, 572, AP_DETECT_STOCK_FLAG}, /* Forest Tiny Medal */
    {14041324u, 573, AP_DETECT_STOCK_FLAG}, /* Forest Chunky Medal */
    {14041325u, 215, AP_DETECT_STOCK_FLAG}, /* Forest Chunky Minecart */
    {14041326u, 211, AP_DETECT_STOCK_FLAG}, /* Forest Diddy Top of Mushroom Barrel */
    {14041327u, 227, AP_DETECT_STOCK_FLAG}, /* Forest Tiny Mushroom Barrel */
    {14041328u, 254, AP_DETECT_STOCK_FLAG}, /* Forest Donkey Baboon Blast */
    {14041329u, 492, AP_DETECT_STOCK_FLAG}, /* Forest Kasplat: Mushroom Exterior Lower Floor */
    {14041330u, 228, AP_DETECT_STOCK_FLAG}, /* Forest Donkey Mushroom Cannons */
    {14041331u, 490, AP_DETECT_STOCK_FLAG}, /* Forest Kasplat: Inside Giant Mushroom */
    {14041332u, 493, AP_DETECT_STOCK_FLAG}, /* Forest Kasplat: Mushroom Exterior Night Door */
    {14041333u, 613, AP_DETECT_STOCK_FLAG}, /* Forest Battle Arena (Mushroom Exterior Top Ladder) */
    {14041334u, 225, AP_DETECT_STOCK_FLAG}, /* Forest Chunky Face Puzzle */
    {14041335u, 226, AP_DETECT_STOCK_FLAG}, /* Forest Lanky Zinger Bounce */
    {14041336u, 224, AP_DETECT_STOCK_FLAG}, /* Forest Lanky Colored Mushroom Slam */
    {14041337u, 250, AP_DETECT_STOCK_FLAG}, /* Forest Diddy Owl Race */
    {14041338u, 249, AP_DETECT_STOCK_FLAG}, /* Forest Lanky Rabbit Race */
    {14041339u, 491, AP_DETECT_STOCK_FLAG}, /* Forest Kasplat: Under Owl Tree */
    {14041340u, 205, AP_DETECT_STOCK_FLAG}, /* Forest Tiny Anthill Banana */
    {14041341u, 219, AP_DETECT_STOCK_FLAG}, /* Forest Donkey Mill Levers */
    {14041342u, 214, AP_DETECT_STOCK_FLAG}, /* Forest Diddy Winch Cage */
    {14041343u, 247, AP_DETECT_STOCK_FLAG}, /* Forest Tiny Spider Boss */
    {14041344u, 221, AP_DETECT_STOCK_FLAG}, /* Forest Chunky Keg Crushing */
    {14041345u, 216, AP_DETECT_STOCK_FLAG}, /* Forest Diddy Dark Rafters */
    {14041346u, 595, AP_DETECT_STOCK_FLAG}, /* Forest Fairy (Dark Rafters) */
    {14041347u, 217, AP_DETECT_STOCK_FLAG}, /* Forest Lanky Attic Shooting */
    {14041348u, 489, AP_DETECT_STOCK_FLAG}, /* Forest Kasplat: Behind Thornvine Barn */
    {14041349u, 235, AP_DETECT_STOCK_FLAG}, /* Forest Donkey Thornvine Barn Barrel */
    {14041350u, 596, AP_DETECT_STOCK_FLAG}, /* Forest Fairy (Thornvine Barn) */
    {14041351u, 209, AP_DETECT_STOCK_FLAG}, /* Forest Tiny Top of the Beanstalk */
    {14041352u, 253, AP_DETECT_STOCK_FLAG}, /* Forest Chunky Apple Rescue */
    {14041353u, 768, AP_DETECT_EVENT}, /* Forest Second Anthill Reward */
    {14041354u, 236, AP_DETECT_STOCK_FLAG}, /* Forest Boss Defeated */
    {14041355u, 574, AP_DETECT_STOCK_FLAG}, /* Caves Donkey Medal */
    {14041356u, 575, AP_DETECT_STOCK_FLAG}, /* Caves Diddy Medal */
    {14041357u, 576, AP_DETECT_STOCK_FLAG}, /* Caves Lanky Medal */
    {14041358u, 577, AP_DETECT_STOCK_FLAG}, /* Caves Tiny Medal */
    {14041359u, 578, AP_DETECT_STOCK_FLAG}, /* Caves Chunky Medal */
    {14041360u, 298, AP_DETECT_STOCK_FLAG}, /* Caves Donkey Baboon Blast */
    {14041361u, 294, AP_DETECT_STOCK_FLAG}, /* Caves Diddy Jetpack Barrel */
    {14041362u, 295, AP_DETECT_STOCK_FLAG}, /* Caves Tiny Mini Cave Barrel */
    {14041363u, 297, AP_DETECT_STOCK_FLAG}, /* Caves Tiny Monkeyport Igloo */
    {14041364u, 268, AP_DETECT_STOCK_FLAG}, /* Caves Chunky Gorilla Gone */
    {14041365u, 494, AP_DETECT_STOCK_FLAG}, /* Caves Kasplat: Near Ice Castle */
    {14041366u, 495, AP_DETECT_STOCK_FLAG}, /* Caves Kasplat: Mini Room by Funky */
    {14041367u, 496, AP_DETECT_STOCK_FLAG}, /* Caves Kasplat: On Warp 5 Pillar */
    {14041368u, 497, AP_DETECT_STOCK_FLAG}, /* Caves Kasplat: Cabins Central Rise */
    {14041369u, 259, AP_DETECT_STOCK_FLAG}, /* Caves Lanky Beetle Race */
    {14041370u, 271, AP_DETECT_STOCK_FLAG}, /* Caves Lanky Ice Castle Slam Challenge */
    {14041371u, 270, AP_DETECT_STOCK_FLAG}, /* Caves Chunky Transparent Igloo */
    {14041372u, 498, AP_DETECT_STOCK_FLAG}, /* Caves Kasplat: On the Igloo */
    {14041373u, 275, AP_DETECT_STOCK_FLAG}, /* Caves Donkey 5 Door Igloo */
    {14041374u, 274, AP_DETECT_STOCK_FLAG}, /* Caves Diddy 5 Door Igloo */
    {14041375u, 281, AP_DETECT_STOCK_FLAG}, /* Caves Lanky 5 Door Igloo */
    {14041376u, 279, AP_DETECT_STOCK_FLAG}, /* Caves Tiny 5 Door Igloo */
    {14041377u, 597, AP_DETECT_STOCK_FLAG}, /* Caves Fairy (Tiny Igloo) */
    {14041378u, 278, AP_DETECT_STOCK_FLAG}, /* Caves Chunky 5 Door Igloo */
    {14041379u, 276, AP_DETECT_STOCK_FLAG}, /* Caves Donkey Rotating Cabin */
    {14041380u, 616, AP_DETECT_STOCK_FLAG}, /* Caves Battle Arena (Rotating Cabin: Wall) */
    {14041381u, 261, AP_DETECT_STOCK_FLAG}, /* Caves Donkey 5 Door Cabin */
    {14041382u, 262, AP_DETECT_STOCK_FLAG}, /* Caves Diddy 5 Door Cabin Lower */
    {14041383u, 293, AP_DETECT_STOCK_FLAG}, /* Caves Diddy 5 Door Cabin Upper */
    {14041384u, 608, AP_DETECT_STOCK_FLAG}, /* Caves Fairy (Diddy Candles Cabin) */
    {14041385u, 264, AP_DETECT_STOCK_FLAG}, /* Caves Lanky Sprint Cabin */
    {14041386u, 260, AP_DETECT_STOCK_FLAG}, /* Caves Tiny 5 Door Cabin */
    {14041387u, 263, AP_DETECT_STOCK_FLAG}, /* Caves Chunky 5 Door Cabin */
    {14041388u, 292, AP_DETECT_STOCK_FLAG}, /* Caves Boss Defeated */
    {14041389u, 579, AP_DETECT_STOCK_FLAG}, /* Castle Donkey Medal */
    {14041390u, 580, AP_DETECT_STOCK_FLAG}, /* Castle Diddy Medal */
    {14041391u, 581, AP_DETECT_STOCK_FLAG}, /* Castle Lanky Medal */
    {14041392u, 582, AP_DETECT_STOCK_FLAG}, /* Castle Tiny Medal */
    {14041393u, 583, AP_DETECT_STOCK_FLAG}, /* Castle Chunky Medal */
    {14041394u, 350, AP_DETECT_STOCK_FLAG}, /* Castle Diddy Above Castle */
    {14041395u, 501, AP_DETECT_STOCK_FLAG}, /* Castle Kasplat: Near Rocketbarrel Overhang */
    {14041396u, 502, AP_DETECT_STOCK_FLAG}, /* Castle Kasplat: On a lone platform */
    {14041397u, 320, AP_DETECT_STOCK_FLAG}, /* Castle Donkey Tree Sniping */
    {14041398u, 319, AP_DETECT_STOCK_FLAG}, /* Castle Chunky Tree Sniping Barrel */
    {14041399u, 499, AP_DETECT_STOCK_FLAG}, /* Castle Kasplat: Inside the Tree */
    {14041400u, 605, AP_DETECT_STOCK_FLAG}, /* Castle Fairy (Tree Sniping Room) */
    {14041401u, 313, AP_DETECT_STOCK_FLAG}, /* Castle Donkey Library */
    {14041402u, 305, AP_DETECT_STOCK_FLAG}, /* Castle Diddy Ballroom */
    {14041403u, 604, AP_DETECT_STOCK_FLAG}, /* Castle Fairy (Near Car Race) */
    {14041404u, 325, AP_DETECT_STOCK_FLAG}, /* Castle Tiny Car Race */
    {14041405u, 306, AP_DETECT_STOCK_FLAG}, /* Castle Lanky Tower */
    {14041406u, 323, AP_DETECT_STOCK_FLAG}, /* Castle Lanky Greenhouse */
    {14041407u, 617, AP_DETECT_STOCK_FLAG}, /* Castle Battle Arena (Greenhouse: Center) */
    {14041408u, 351, AP_DETECT_STOCK_FLAG}, /* Castle Tiny Trash Can */
    {14041409u, 322, AP_DETECT_STOCK_FLAG}, /* Castle Chunky Shed */
    {14041410u, 314, AP_DETECT_STOCK_FLAG}, /* Castle Chunky Museum */
    {14041411u, 500, AP_DETECT_STOCK_FLAG}, /* Castle Kasplat: Lower Cave Center */
    {14041412u, 310, AP_DETECT_STOCK_FLAG}, /* Castle Diddy Crypt */
    {14041413u, 311, AP_DETECT_STOCK_FLAG}, /* Castle Chunky Crypt */
    {14041414u, 318, AP_DETECT_STOCK_FLAG}, /* Castle Donkey Minecart */
    {14041415u, 308, AP_DETECT_STOCK_FLAG}, /* Castle Lanky Mausoleum */
    {14041416u, 309, AP_DETECT_STOCK_FLAG}, /* Castle Tiny Mausoleum */
    {14041417u, 315, AP_DETECT_STOCK_FLAG}, /* Castle Tiny Over Chasm */
    {14041418u, 503, AP_DETECT_STOCK_FLAG}, /* Castle Kasplat: Near Upper Cave Shop */
    {14041419u, 326, AP_DETECT_STOCK_FLAG}, /* Castle Donkey Dungeon */
    {14041420u, 353, AP_DETECT_STOCK_FLAG}, /* Castle Diddy Dungeon */
    {14041421u, 316, AP_DETECT_STOCK_FLAG}, /* Castle Lanky Dungeon */
    {14041422u, 317, AP_DETECT_STOCK_FLAG}, /* Castle Boss Defeated */
    {14041423u, 972, AP_DETECT_EVENT}, /* Helm Donkey Barrel 1 */
    {14041424u, 973, AP_DETECT_EVENT}, /* Helm Donkey Barrel 2 */
    {14041425u, 974, AP_DETECT_EVENT}, /* Helm Diddy Barrel 1 */
    {14041426u, 975, AP_DETECT_EVENT}, /* Helm Diddy Barrel 2 */
    {14041427u, 976, AP_DETECT_EVENT}, /* Helm Lanky Barrel 1 */
    {14041428u, 977, AP_DETECT_EVENT}, /* Helm Lanky Barrel 2 */
    {14041429u, 978, AP_DETECT_EVENT}, /* Helm Tiny Barrel 1 */
    {14041430u, 979, AP_DETECT_EVENT}, /* Helm Tiny Barrel 2 */
    {14041431u, 980, AP_DETECT_EVENT}, /* Helm Chunky Barrel 1 */
    {14041432u, 981, AP_DETECT_EVENT}, /* Helm Chunky Barrel 2 */
    {14041433u, 618, AP_DETECT_STOCK_FLAG}, /* Helm Battle Arena (Top of Blast-o-Matic) */
    {14041434u, 584, AP_DETECT_STOCK_FLAG}, /* Helm Donkey Medal */
    {14041435u, 588, AP_DETECT_STOCK_FLAG}, /* Helm Chunky Medal */
    {14041436u, 587, AP_DETECT_STOCK_FLAG}, /* Helm Tiny Medal */
    {14041437u, 586, AP_DETECT_STOCK_FLAG}, /* Helm Lanky Medal */
    {14041438u, 585, AP_DETECT_STOCK_FLAG}, /* Helm Diddy Medal */
    {14041439u, 598, AP_DETECT_STOCK_FLAG}, /* Helm Fairy (Key 8 Room (1)) */
    {14041440u, 599, AP_DETECT_STOCK_FLAG}, /* Helm Fairy (Key 8 Room (2)) */
    {14041441u, 380, AP_DETECT_STOCK_FLAG}, /* The End of Helm */
    {14041442u, 835, AP_DETECT_SHOP}, /* Isles Cranky Shared */
    {14041443u, 800, AP_DETECT_SHOP}, /* Japes Cranky Donkey */
    {14041444u, 801, AP_DETECT_SHOP}, /* Japes Cranky Diddy */
    {14041445u, 802, AP_DETECT_SHOP}, /* Japes Cranky Lanky */
    {14041446u, 803, AP_DETECT_SHOP}, /* Japes Cranky Tiny */
    {14041447u, 804, AP_DETECT_SHOP}, /* Japes Cranky Chunky */
    {14041448u, 840, AP_DETECT_SHOP}, /* Japes Funky Donkey */
    {14041449u, 841, AP_DETECT_SHOP}, /* Japes Funky Diddy */
    {14041450u, 842, AP_DETECT_SHOP}, /* Japes Funky Lanky */
    {14041451u, 843, AP_DETECT_SHOP}, /* Japes Funky Tiny */
    {14041452u, 844, AP_DETECT_SHOP}, /* Japes Funky Chunky */
    {14041453u, 805, AP_DETECT_SHOP}, /* Aztec Cranky Donkey */
    {14041454u, 806, AP_DETECT_SHOP}, /* Aztec Cranky Diddy */
    {14041455u, 875, AP_DETECT_SHOP}, /* Aztec Candy Donkey */
    {14041456u, 876, AP_DETECT_SHOP}, /* Aztec Candy Diddy */
    {14041457u, 877, AP_DETECT_SHOP}, /* Aztec Candy Lanky */
    {14041458u, 878, AP_DETECT_SHOP}, /* Aztec Candy Tiny */
    {14041459u, 879, AP_DETECT_SHOP}, /* Aztec Candy Chunky */
    {14041460u, 810, AP_DETECT_SHOP}, /* Factory Cranky Donkey */
    {14041461u, 811, AP_DETECT_SHOP}, /* Factory Cranky Diddy */
    {14041462u, 812, AP_DETECT_SHOP}, /* Factory Cranky Lanky */
    {14041463u, 813, AP_DETECT_SHOP}, /* Factory Cranky Tiny */
    {14041464u, 814, AP_DETECT_SHOP}, /* Factory Cranky Chunky */
    {14041465u, 850, AP_DETECT_SHOP}, /* Factory Funky Shared */
    {14041466u, 885, AP_DETECT_SHOP}, /* Galleon Candy Shared */
    {14041467u, 820, AP_DETECT_SHOP}, /* Forest Cranky Shared */
    {14041468u, 860, AP_DETECT_SHOP}, /* Forest Funky Shared */
    {14041469u, 827, AP_DETECT_SHOP}, /* Caves Cranky Lanky */
    {14041470u, 828, AP_DETECT_SHOP}, /* Caves Cranky Tiny */
    {14041471u, 829, AP_DETECT_SHOP}, /* Caves Cranky Chunky */
    {14041472u, 865, AP_DETECT_SHOP}, /* Caves Funky Shared */
    {14041473u, 890, AP_DETECT_SHOP}, /* Caves Candy Shared */
    {14041474u, 830, AP_DETECT_SHOP}, /* Castle Cranky Shared */
    {14041475u, 870, AP_DETECT_SHOP}, /* Castle Funky Shared */
    {14041476u, 895, AP_DETECT_SHOP}, /* Castle Candy Shared */
    {14041477u, 379, AP_DETECT_STOCK_FLAG}, /* Jetpac */
    {14041478u, 800, AP_DETECT_SHOP}, /* Japes Cranky Shared */
    {14041479u, 840, AP_DETECT_SHOP}, /* Japes Funky Shared */
    {14041480u, 805, AP_DETECT_SHOP}, /* Aztec Cranky Shared */
    {14041481u, 807, AP_DETECT_SHOP}, /* Aztec Cranky Lanky */
    {14041482u, 808, AP_DETECT_SHOP}, /* Aztec Cranky Tiny */
    {14041483u, 809, AP_DETECT_SHOP}, /* Aztec Cranky Chunky */
    {14041484u, 845, AP_DETECT_SHOP}, /* Aztec Funky Shared */
    {14041485u, 845, AP_DETECT_SHOP}, /* Aztec Funky Donkey */
    {14041486u, 846, AP_DETECT_SHOP}, /* Aztec Funky Diddy */
    {14041487u, 847, AP_DETECT_SHOP}, /* Aztec Funky Lanky */
    {14041488u, 848, AP_DETECT_SHOP}, /* Aztec Funky Tiny */
    {14041489u, 849, AP_DETECT_SHOP}, /* Aztec Funky Chunky */
    {14041490u, 875, AP_DETECT_SHOP}, /* Aztec Candy Shared */
    {14041491u, 810, AP_DETECT_SHOP}, /* Factory Cranky Shared */
    {14041492u, 850, AP_DETECT_SHOP}, /* Factory Funky Donkey */
    {14041493u, 851, AP_DETECT_SHOP}, /* Factory Funky Diddy */
    {14041494u, 852, AP_DETECT_SHOP}, /* Factory Funky Lanky */
    {14041495u, 853, AP_DETECT_SHOP}, /* Factory Funky Tiny */
    {14041496u, 854, AP_DETECT_SHOP}, /* Factory Funky Chunky */
    {14041497u, 880, AP_DETECT_SHOP}, /* Factory Candy Shared */
    {14041498u, 880, AP_DETECT_SHOP}, /* Factory Candy Donkey */
    {14041499u, 881, AP_DETECT_SHOP}, /* Factory Candy Diddy */
    {14041500u, 882, AP_DETECT_SHOP}, /* Factory Candy Lanky */
    {14041501u, 883, AP_DETECT_SHOP}, /* Factory Candy Tiny */
    {14041502u, 884, AP_DETECT_SHOP}, /* Factory Candy Chunky */
    {14041503u, 815, AP_DETECT_SHOP}, /* Galleon Cranky Shared */
    {14041504u, 815, AP_DETECT_SHOP}, /* Galleon Cranky Donkey */
    {14041505u, 816, AP_DETECT_SHOP}, /* Galleon Cranky Diddy */
    {14041506u, 817, AP_DETECT_SHOP}, /* Galleon Cranky Lanky */
    {14041507u, 818, AP_DETECT_SHOP}, /* Galleon Cranky Tiny */
    {14041508u, 819, AP_DETECT_SHOP}, /* Galleon Cranky Chunky */
    {14041509u, 855, AP_DETECT_SHOP}, /* Galleon Funky Shared */
    {14041510u, 855, AP_DETECT_SHOP}, /* Galleon Funky Donkey */
    {14041511u, 856, AP_DETECT_SHOP}, /* Galleon Funky Diddy */
    {14041512u, 857, AP_DETECT_SHOP}, /* Galleon Funky Lanky */
    {14041513u, 858, AP_DETECT_SHOP}, /* Galleon Funky Tiny */
    {14041514u, 859, AP_DETECT_SHOP}, /* Galleon Funky Chunky */
    {14041515u, 885, AP_DETECT_SHOP}, /* Galleon Candy Donkey */
    {14041516u, 886, AP_DETECT_SHOP}, /* Galleon Candy Diddy */
    {14041517u, 887, AP_DETECT_SHOP}, /* Galleon Candy Lanky */
    {14041518u, 888, AP_DETECT_SHOP}, /* Galleon Candy Tiny */
    {14041519u, 889, AP_DETECT_SHOP}, /* Galleon Candy Chunky */
    {14041520u, 820, AP_DETECT_SHOP}, /* Forest Cranky Donkey */
    {14041521u, 821, AP_DETECT_SHOP}, /* Forest Cranky Diddy */
    {14041522u, 822, AP_DETECT_SHOP}, /* Forest Cranky Lanky */
    {14041523u, 823, AP_DETECT_SHOP}, /* Forest Cranky Tiny */
    {14041524u, 824, AP_DETECT_SHOP}, /* Forest Cranky Chunky */
    {14041525u, 860, AP_DETECT_SHOP}, /* Forest Funky Donkey */
    {14041526u, 861, AP_DETECT_SHOP}, /* Forest Funky Diddy */
    {14041527u, 862, AP_DETECT_SHOP}, /* Forest Funky Lanky */
    {14041528u, 863, AP_DETECT_SHOP}, /* Forest Funky Tiny */
    {14041529u, 864, AP_DETECT_SHOP}, /* Forest Funky Chunky */
    {14041530u, 825, AP_DETECT_SHOP}, /* Caves Cranky Shared */
    {14041531u, 825, AP_DETECT_SHOP}, /* Caves Cranky Donkey */
    {14041532u, 826, AP_DETECT_SHOP}, /* Caves Cranky Diddy */
    {14041533u, 865, AP_DETECT_SHOP}, /* Caves Funky Donkey */
    {14041534u, 866, AP_DETECT_SHOP}, /* Caves Funky Diddy */
    {14041535u, 867, AP_DETECT_SHOP}, /* Caves Funky Lanky */
    {14041536u, 868, AP_DETECT_SHOP}, /* Caves Funky Tiny */
    {14041537u, 869, AP_DETECT_SHOP}, /* Caves Funky Chunky */
    {14041538u, 890, AP_DETECT_SHOP}, /* Caves Candy Donkey */
    {14041539u, 891, AP_DETECT_SHOP}, /* Caves Candy Diddy */
    {14041540u, 892, AP_DETECT_SHOP}, /* Caves Candy Lanky */
    {14041541u, 893, AP_DETECT_SHOP}, /* Caves Candy Tiny */
    {14041542u, 894, AP_DETECT_SHOP}, /* Caves Candy Chunky */
    {14041543u, 830, AP_DETECT_SHOP}, /* Castle Cranky Donkey */
    {14041544u, 831, AP_DETECT_SHOP}, /* Castle Cranky Diddy */
    {14041545u, 832, AP_DETECT_SHOP}, /* Castle Cranky Lanky */
    {14041546u, 833, AP_DETECT_SHOP}, /* Castle Cranky Tiny */
    {14041547u, 834, AP_DETECT_SHOP}, /* Castle Cranky Chunky */
    {14041548u, 870, AP_DETECT_SHOP}, /* Castle Funky Donkey */
    {14041549u, 871, AP_DETECT_SHOP}, /* Castle Funky Diddy */
    {14041550u, 872, AP_DETECT_SHOP}, /* Castle Funky Lanky */
    {14041551u, 873, AP_DETECT_SHOP}, /* Castle Funky Tiny */
    {14041552u, 874, AP_DETECT_SHOP}, /* Castle Funky Chunky */
    {14041553u, 895, AP_DETECT_SHOP}, /* Castle Candy Donkey */
    {14041554u, 896, AP_DETECT_SHOP}, /* Castle Candy Diddy */
    {14041555u, 897, AP_DETECT_SHOP}, /* Castle Candy Lanky */
    {14041556u, 898, AP_DETECT_SHOP}, /* Castle Candy Tiny */
    {14041557u, 899, AP_DETECT_SHOP}, /* Castle Candy Chunky */
    {14041558u, 835, AP_DETECT_SHOP}, /* Isles Cranky Donkey */
    {14041559u, 836, AP_DETECT_SHOP}, /* Isles Cranky Diddy */
    {14041560u, 837, AP_DETECT_SHOP}, /* Isles Cranky Lanky */
    {14041561u, 838, AP_DETECT_SHOP}, /* Isles Cranky Tiny */
    {14041562u, 839, AP_DETECT_SHOP}, /* Isles Cranky Chunky */
    {14041563u, 1022, AP_DETECT_EVENT}, /* Turning In 1 Blueprint */
    {14041564u, 1023, AP_DETECT_EVENT}, /* Turning In 2 Blueprints */
    {14041565u, 1024, AP_DETECT_EVENT}, /* Turning In 3 Blueprints */
    {14041566u, 1025, AP_DETECT_EVENT}, /* Turning In 4 Blueprints */
    {14041567u, 1026, AP_DETECT_EVENT}, /* Turning In 5 Blueprints */
    {14041568u, 1027, AP_DETECT_EVENT}, /* Turning In 6 Blueprints */
    {14041569u, 1028, AP_DETECT_EVENT}, /* Turning In 7 Blueprints */
    {14041570u, 1029, AP_DETECT_EVENT}, /* Turning In 8 Blueprints */
    {14041571u, 1030, AP_DETECT_EVENT}, /* Turning In 9 Blueprints */
    {14041572u, 1031, AP_DETECT_EVENT}, /* Turning In 10 Blueprints */
    {14041573u, 1032, AP_DETECT_EVENT}, /* Turning In 11 Blueprints */
    {14041574u, 1033, AP_DETECT_EVENT}, /* Turning In 12 Blueprints */
    {14041575u, 1034, AP_DETECT_EVENT}, /* Turning In 13 Blueprints */
    {14041576u, 1035, AP_DETECT_EVENT}, /* Turning In 14 Blueprints */
    {14041577u, 1036, AP_DETECT_EVENT}, /* Turning In 15 Blueprints */
    {14041578u, 1037, AP_DETECT_EVENT}, /* Turning In 16 Blueprints */
    {14041579u, 1038, AP_DETECT_EVENT}, /* Turning In 17 Blueprints */
    {14041580u, 1039, AP_DETECT_EVENT}, /* Turning In 18 Blueprints */
    {14041581u, 1040, AP_DETECT_EVENT}, /* Turning In 19 Blueprints */
    {14041582u, 1041, AP_DETECT_EVENT}, /* Turning In 20 Blueprints */
    {14041583u, 1042, AP_DETECT_EVENT}, /* Turning In 21 Blueprints */
    {14041584u, 1043, AP_DETECT_EVENT}, /* Turning In 22 Blueprints */
    {14041585u, 1044, AP_DETECT_EVENT}, /* Turning In 23 Blueprints */
    {14041586u, 1045, AP_DETECT_EVENT}, /* Turning In 24 Blueprints */
    {14041587u, 1046, AP_DETECT_EVENT}, /* Turning In 25 Blueprints */
    {14041588u, 1047, AP_DETECT_EVENT}, /* Turning In 26 Blueprints */
    {14041589u, 1048, AP_DETECT_EVENT}, /* Turning In 27 Blueprints */
    {14041590u, 1049, AP_DETECT_EVENT}, /* Turning In 28 Blueprints */
    {14041591u, 1050, AP_DETECT_EVENT}, /* Turning In 29 Blueprints */
    {14041592u, 1051, AP_DETECT_EVENT}, /* Turning In 30 Blueprints */
    {14041593u, 1052, AP_DETECT_EVENT}, /* Turning In 31 Blueprints */
    {14041594u, 1053, AP_DETECT_EVENT}, /* Turning In 32 Blueprints */
    {14041595u, 1054, AP_DETECT_EVENT}, /* Turning In 33 Blueprints */
    {14041596u, 1055, AP_DETECT_EVENT}, /* Turning In 34 Blueprints */
    {14041597u, 1056, AP_DETECT_EVENT}, /* Turning In 35 Blueprints */
    {14041598u, 1057, AP_DETECT_EVENT}, /* Turning In 36 Blueprints */
    {14041599u, 1058, AP_DETECT_EVENT}, /* Turning In 37 Blueprints */
    {14041600u, 1059, AP_DETECT_EVENT}, /* Turning In 38 Blueprints */
    {14041601u, 1060, AP_DETECT_EVENT}, /* Turning In 39 Blueprints */
    {14041602u, 1061, AP_DETECT_EVENT}, /* Turning In 40 Blueprints */
    {14041603u, 900, AP_DETECT_EVENT}, /* Japes Donkey Hint Door */
    {14041604u, 901, AP_DETECT_EVENT}, /* Japes Diddy Hint Door */
    {14041605u, 902, AP_DETECT_EVENT}, /* Japes Lanky Hint Door */
    {14041606u, 903, AP_DETECT_EVENT}, /* Japes Tiny Hint Door */
    {14041607u, 904, AP_DETECT_EVENT}, /* Japes Chunky Hint Door */
    {14041608u, 905, AP_DETECT_EVENT}, /* Aztec Donkey Hint Door */
    {14041609u, 906, AP_DETECT_EVENT}, /* Aztec Diddy Hint Door */
    {14041610u, 907, AP_DETECT_EVENT}, /* Aztec Lanky Hint Door */
    {14041611u, 908, AP_DETECT_EVENT}, /* Aztec Tiny Hint Door */
    {14041612u, 909, AP_DETECT_EVENT}, /* Aztec Chunky Hint Door */
    {14041613u, 910, AP_DETECT_EVENT}, /* Factory Donkey Hint Door */
    {14041614u, 911, AP_DETECT_EVENT}, /* Factory Diddy Hint Door */
    {14041615u, 912, AP_DETECT_EVENT}, /* Factory Lanky Hint Door */
    {14041616u, 913, AP_DETECT_EVENT}, /* Factory Tiny Hint Door */
    {14041617u, 914, AP_DETECT_EVENT}, /* Factory Chunky Hint Door */
    {14041618u, 915, AP_DETECT_EVENT}, /* Galleon Donkey Hint Door */
    {14041619u, 916, AP_DETECT_EVENT}, /* Galleon Diddy Hint Door */
    {14041620u, 917, AP_DETECT_EVENT}, /* Galleon Lanky Hint Door */
    {14041621u, 918, AP_DETECT_EVENT}, /* Galleon Tiny Hint Door */
    {14041622u, 919, AP_DETECT_EVENT}, /* Galleon Chunky Hint Door */
    {14041623u, 920, AP_DETECT_EVENT}, /* Forest Donkey Hint Door */
    {14041624u, 921, AP_DETECT_EVENT}, /* Forest Diddy Hint Door */
    {14041625u, 922, AP_DETECT_EVENT}, /* Forest Lanky Hint Door */
    {14041626u, 923, AP_DETECT_EVENT}, /* Forest Tiny Hint Door */
    {14041627u, 924, AP_DETECT_EVENT}, /* Forest Chunky Hint Door */
    {14041628u, 925, AP_DETECT_EVENT}, /* Caves Donkey Hint Door */
    {14041629u, 926, AP_DETECT_EVENT}, /* Caves Diddy Hint Door */
    {14041630u, 927, AP_DETECT_EVENT}, /* Caves Lanky Hint Door */
    {14041631u, 928, AP_DETECT_EVENT}, /* Caves Tiny Hint Door */
    {14041632u, 929, AP_DETECT_EVENT}, /* Caves Chunky Hint Door */
    {14041633u, 930, AP_DETECT_EVENT}, /* Castle Donkey Hint Door */
    {14041634u, 931, AP_DETECT_EVENT}, /* Castle Diddy Hint Door */
    {14041635u, 932, AP_DETECT_EVENT}, /* Castle Lanky Hint Door */
    {14041636u, 933, AP_DETECT_EVENT}, /* Castle Tiny Hint Door */
    {14041637u, 934, AP_DETECT_EVENT}, /* Castle Chunky Hint Door */
    {14041673u, 678, AP_DETECT_EVENT}, /* Japes Dirt (On Painting Hill) */
    {14041674u, 679, AP_DETECT_EVENT}, /* Aztec Dirt (Chunky 5DT) */
    {14041675u, 680, AP_DETECT_EVENT}, /* Factory Dirt (Dark Room) */
    {14041676u, 681, AP_DETECT_EVENT}, /* Isles Dirt (Front of Fungi Building) */
    {14041677u, 682, AP_DETECT_EVENT}, /* Isles Dirt (Under Caves Lobby Entrance) */
    {14041678u, 683, AP_DETECT_EVENT}, /* Isles Dirt (On Aztec Building) */
    {14041679u, 684, AP_DETECT_EVENT}, /* Aztec Dirt (Oasis) */
    {14041680u, 685, AP_DETECT_EVENT}, /* Forest Dirt (Mill Grass) */
    {14041681u, 686, AP_DETECT_EVENT}, /* Forest Dirt (Front of Beanstalk) */
    {14041682u, 687, AP_DETECT_EVENT}, /* Galleon Dirt (Lighthouse: Interior Rear) */
    {14041683u, 688, AP_DETECT_EVENT}, /* Caves Dirt (Giant Kosha) */
    {14041684u, 689, AP_DETECT_EVENT}, /* Castle Dirt (Top of Castle Near Shop) */
    {14041685u, 690, AP_DETECT_EVENT}, /* Isles Dirt (Prison: Back) */
    {14041686u, 691, AP_DETECT_EVENT}, /* Isles Dirt (Training Grounds: Rear Tunnel) */
    {14041687u, 692, AP_DETECT_EVENT}, /* Isles Dirt (Training Grounds: Banana Hoard) */
    {14041688u, 693, AP_DETECT_EVENT}, /* Isles Dirt (Castle Lobby: Center) */
    {14041689u, 940, AP_DETECT_EVENT}, /* Japes Crate (Behind the Mountain) */
    {14041690u, 941, AP_DETECT_EVENT}, /* Japes Crate (In the Rambi Cave) */
    {14041691u, 942, AP_DETECT_EVENT}, /* Aztec Crate (Llama Temple: Entrance) */
    {14041692u, 943, AP_DETECT_EVENT}, /* Factory Crate (Testing Shop Entrance Right) */
    {14041693u, 944, AP_DETECT_EVENT}, /* Factory Crate (Near Right Storage Shop) */
    {14041694u, 945, AP_DETECT_EVENT}, /* Galleon Crate (Near Cactus) */
    {14041695u, 946, AP_DETECT_EVENT}, /* Aztec Crate (On Llama Temple) */
    {14041696u, 947, AP_DETECT_EVENT}, /* Aztec Crate (Near Gong Tower) */
    {14041697u, 948, AP_DETECT_EVENT}, /* Forest Crate (Owl Tree Clearing) */
    {14041698u, 949, AP_DETECT_EVENT}, /* Forest Crate (Near Thornvine Barn) */
    {14041699u, 950, AP_DETECT_EVENT}, /* Forest Crate (Behind Rafters Barn) */
    {14041700u, 951, AP_DETECT_EVENT}, /* Forest Crate (Thornvine Barn: Back Left) */
    {14041701u, 952, AP_DETECT_EVENT}, /* Castle Crate (Lower Cave: Behind Mausoleum) */
    {14041702u, 694, AP_DETECT_EVENT}, /* Isles Boulder (Near Level 2) */
    {14041703u, 695, AP_DETECT_EVENT}, /* Isles Boulder (Near Level 6) */
    {14041704u, 696, AP_DETECT_EVENT}, /* Aztec Boulder (Tunnel) */
    {14041705u, 697, AP_DETECT_EVENT}, /* Caves Boulder (Small) */
    {14041706u, 698, AP_DETECT_EVENT}, /* Caves Boulder (Large) */
    {14041707u, 699, AP_DETECT_EVENT}, /* Castle Boulder (Museum) */
    {14041708u, 700, AP_DETECT_EVENT}, /* Isles Boulder (Japes Lobby) */
    {14041709u, 701, AP_DETECT_EVENT}, /* Isles Boulder (Castle Lobby) */
    {14041710u, 702, AP_DETECT_EVENT}, /* Isles Boulder (Caves Lobby) */
    {14041711u, 703, AP_DETECT_EVENT}, /* Forest Keg (Mill Front: Near) */
    {14041712u, 704, AP_DETECT_EVENT}, /* Forest Keg (Mill Front: Far) */
    {14041713u, 705, AP_DETECT_EVENT}, /* Forest Keg (Mill Back) */
    {14041714u, 706, AP_DETECT_EVENT}, /* Aztec Vase (Circle) */
    {14041715u, 707, AP_DETECT_EVENT}, /* Aztec Vase (Colon) */
    {14041716u, 708, AP_DETECT_EVENT}, /* Aztec Vase (Triangle) */
    {14041717u, 709, AP_DETECT_EVENT}, /* Aztec Vase (Plus) */
    {14041718u, 1062, AP_DETECT_ENEMY}, /* Japes Enemy: Start */
    {14041719u, 1063, AP_DETECT_ENEMY}, /* Japes Enemy: Diddy Cavern */
    {14041720u, 1064, AP_DETECT_ENEMY}, /* Japes Enemy: Tunnel (0) */
    {14041721u, 1065, AP_DETECT_ENEMY}, /* Japes Enemy: Tunnel (1) */
    {14041722u, 1066, AP_DETECT_ENEMY}, /* Japes Enemy: Storm (0) */
    {14041723u, 1067, AP_DETECT_ENEMY}, /* Japes Enemy: Storm (1) */
    {14041724u, 1068, AP_DETECT_ENEMY}, /* Japes Enemy: Storm (2) */
    {14041725u, 1069, AP_DETECT_ENEMY}, /* Japes Enemy: Hive (0) */
    {14041726u, 1070, AP_DETECT_ENEMY}, /* Japes Enemy: Hive (1) */
    {14041727u, 1071, AP_DETECT_ENEMY}, /* Japes Enemy: Hive (2) */
    {14041728u, 1072, AP_DETECT_ENEMY}, /* Japes Enemy: Hive (3) */
    {14041729u, 1073, AP_DETECT_ENEMY}, /* Japes Enemy: Hive (4) */
    {14041730u, 1074, AP_DETECT_ENEMY}, /* Japes Enemy: Killed In Demo */
    {14041731u, 1075, AP_DETECT_ENEMY}, /* Japes Enemy: Near Underground */
    {14041732u, 1076, AP_DETECT_ENEMY}, /* Japes Enemy: Near Painting (0) */
    {14041733u, 1077, AP_DETECT_ENEMY}, /* Japes Enemy: Near Painting (1) */
    {14041734u, 1078, AP_DETECT_ENEMY}, /* Japes Enemy: Near Painting (2) */
    {14041735u, 1079, AP_DETECT_ENEMY}, /* Japes Enemy: Mountain */
    {14041736u, 1080, AP_DETECT_ENEMY}, /* Japes Enemy: Feather Tunnel */
    {14041737u, 1081, AP_DETECT_ENEMY}, /* Japes Enemy: Middle Tunnel */
    {14041738u, 1082, AP_DETECT_ENEMY}, /* Isles Japes Lobby Enemy: Enemy (0) */
    {14041739u, 1083, AP_DETECT_ENEMY}, /* Isles Japes Lobby Enemy: Enemy (1) */
    {14041740u, 1089, AP_DETECT_ENEMY}, /* Japes Mountain Enemy: Start (0) */
    {14041741u, 1090, AP_DETECT_ENEMY}, /* Japes Mountain Enemy: Start (1) */
    {14041742u, 1091, AP_DETECT_ENEMY}, /* Japes Mountain Enemy: Start (2) */
    {14041743u, 1092, AP_DETECT_ENEMY}, /* Japes Mountain Enemy: Start (3) */
    {14041744u, 1093, AP_DETECT_ENEMY}, /* Japes Mountain Enemy: Start (4) */
    {14041745u, 1094, AP_DETECT_ENEMY}, /* Japes Mountain Enemy: Near Gate Switch (0) */
    {14041746u, 1095, AP_DETECT_ENEMY}, /* Japes Mountain Enemy: Near Gate Switch (1) */
    {14041747u, 1096, AP_DETECT_ENEMY}, /* Japes Mountain Enemy: Hi Lo */
    {14041748u, 1097, AP_DETECT_ENEMY}, /* Japes Mountain Enemy: Conveyor (0) */
    {14041749u, 1098, AP_DETECT_ENEMY}, /* Japes Mountain Enemy: Conveyor (1) */
    {14041750u, 1099, AP_DETECT_ENEMY}, /* Japes Tiny Hive Enemy: First Room */
    {14041751u, 1100, AP_DETECT_ENEMY}, /* Japes Tiny Hive Enemy: Second Room (0) */
    {14041752u, 1101, AP_DETECT_ENEMY}, /* Japes Tiny Hive Enemy: Second Room (1) */
    {14041753u, 1102, AP_DETECT_ENEMY}, /* Japes Tiny Hive Enemy: Third Room (0) */
    {14041754u, 1103, AP_DETECT_ENEMY}, /* Japes Tiny Hive Enemy: Third Room (1) */
    {14041755u, 1104, AP_DETECT_ENEMY}, /* Japes Tiny Hive Enemy: Third Room (2) */
    {14041756u, 1105, AP_DETECT_ENEMY}, /* Japes Tiny Hive Enemy: Third Room (3) */
    {14041757u, 1106, AP_DETECT_ENEMY}, /* Japes Tiny Hive Enemy: Main Room */
    {14041758u, 1107, AP_DETECT_ENEMY}, /* Aztec Enemy: Vase Room (0) */
    {14041759u, 1108, AP_DETECT_ENEMY}, /* Aztec Enemy: Vase Room (1) */
    {14041760u, 1109, AP_DETECT_ENEMY}, /* Aztec Enemy: Vase Room (2) */
    {14041761u, 1110, AP_DETECT_ENEMY}, /* Aztec Enemy: Tunnel Pad (0) */
    {14041762u, 1111, AP_DETECT_ENEMY}, /* Aztec Enemy: Tunnel Cage (0) */
    {14041763u, 1112, AP_DETECT_ENEMY}, /* Aztec Enemy: Tunnel Cage (1) */
    {14041764u, 1113, AP_DETECT_ENEMY}, /* Aztec Enemy: Tunnel Cage (2) */
    {14041765u, 1114, AP_DETECT_ENEMY}, /* Aztec Enemy: Starting Tunnel (0) */
    {14041766u, 1115, AP_DETECT_ENEMY}, /* Aztec Enemy: Starting Tunnel (1) */
    {14041767u, 1116, AP_DETECT_ENEMY}, /* Aztec Enemy: Oasis Door */
    {14041768u, 1117, AP_DETECT_ENEMY}, /* Aztec Enemy: Tunnel Cage (3) */
    {14041769u, 1118, AP_DETECT_ENEMY}, /* Aztec Enemy: Outside Llama */
    {14041770u, 1119, AP_DETECT_ENEMY}, /* Aztec Enemy: Outside Tower */
    {14041771u, 1120, AP_DETECT_ENEMY}, /* Aztec Enemy: Tunnel Pad (1) */
    {14041772u, 1121, AP_DETECT_ENEMY}, /* Aztec Enemy: Near Candy */
    {14041773u, 1122, AP_DETECT_ENEMY}, /* Aztec Enemy: Around Totem */
    {14041774u, 1123, AP_DETECT_ENEMY}, /* Aztec Enemy: Starting Tunnel (2) */
    {14041775u, 1124, AP_DETECT_ENEMY}, /* Aztec Enemy: Starting Tunnel (3) */
    {14041776u, 1125, AP_DETECT_ENEMY}, /* Aztec Enemy: Outside Snide */
    {14041777u, 1126, AP_DETECT_ENEMY}, /* Aztec Enemy: Outside 5DT */
    {14041778u, 1127, AP_DETECT_ENEMY}, /* Aztec Enemy: Near Sealed Quicksand Tunnel */
    {14041779u, 1130, AP_DETECT_ENEMY}, /* Aztec Donkey 5DTemple Enemy: Start Trap (0) */
    {14041780u, 1131, AP_DETECT_ENEMY}, /* Aztec Donkey 5DTemple Enemy: Start Trap (1) */
    {14041781u, 1132, AP_DETECT_ENEMY}, /* Aztec Donkey 5DTemple Enemy: Start Trap (2) */
    {14041782u, 1133, AP_DETECT_ENEMY}, /* Aztec Donkey 5DTemple Enemy: End Trap (0) */
    {14041783u, 1134, AP_DETECT_ENEMY}, /* Aztec Donkey 5DTemple Enemy: End Trap (1) */
    {14041784u, 1135, AP_DETECT_ENEMY}, /* Aztec Donkey 5DTemple Enemy: End Trap (2) */
    {14041785u, 1136, AP_DETECT_ENEMY}, /* Aztec Donkey 5DTemple Enemy: End Path (0) */
    {14041786u, 1137, AP_DETECT_ENEMY}, /* Aztec Donkey 5DTemple Enemy: End Path (1) */
    {14041787u, 1138, AP_DETECT_ENEMY}, /* Aztec Donkey 5DTemple Enemy: Start Path */
    {14041788u, 1139, AP_DETECT_ENEMY}, /* Aztec Diddy 5DTemple Enemy: End Trap (0) */
    {14041789u, 1140, AP_DETECT_ENEMY}, /* Aztec Diddy 5DTemple Enemy: End Trap (1) */
    {14041790u, 1141, AP_DETECT_ENEMY}, /* Aztec Diddy 5DTemple Enemy: End Trap (2) */
    {14041791u, 1142, AP_DETECT_ENEMY}, /* Aztec Diddy 5DTemple Enemy: Start Left (0) */
    {14041792u, 1143, AP_DETECT_ENEMY}, /* Aztec Diddy 5DTemple Enemy: Start Left (1) */
    {14041793u, 1144, AP_DETECT_ENEMY}, /* Aztec Diddy 5DTemple Enemy: Reward */
    {14041794u, 1145, AP_DETECT_ENEMY}, /* Aztec Diddy 5DTemple Enemy: Second Switch */
    {14041795u, 1146, AP_DETECT_ENEMY}, /* Aztec Lanky 5DTemple Enemy: Joining Paths */
    {14041796u, 1147, AP_DETECT_ENEMY}, /* Aztec Lanky 5DTemple Enemy: End Trap */
    {14041797u, 1148, AP_DETECT_ENEMY}, /* Aztec Lanky 5DTemple Enemy: Reward */
    {14041798u, 1149, AP_DETECT_ENEMY}, /* Aztec Tiny 5DTemple Enemy: Start Right Front */
    {14041799u, 1150, AP_DETECT_ENEMY}, /* Aztec Tiny 5DTemple Enemy: Start Left Back */
    {14041800u, 1151, AP_DETECT_ENEMY}, /* Aztec Tiny 5DTemple Enemy: Start Right Back */
    {14041801u, 1152, AP_DETECT_ENEMY}, /* Aztec Tiny 5DTemple Enemy: Start Left Front */
    {14041802u, 1153, AP_DETECT_ENEMY}, /* Aztec Tiny 5DTemple Enemy: Reward (0) */
    {14041803u, 1154, AP_DETECT_ENEMY}, /* Aztec Tiny 5DTemple Enemy: Reward (1) */
    {14041804u, 1155, AP_DETECT_ENEMY}, /* Aztec Tiny 5DTemple Enemy: Dead End (0) */
    {14041805u, 1156, AP_DETECT_ENEMY}, /* Aztec Tiny 5DTemple Enemy: Dead End (1) */
    {14041806u, 1157, AP_DETECT_ENEMY}, /* Aztec Chunky 5DTemple Enemy: Start Right */
    {14041807u, 1158, AP_DETECT_ENEMY}, /* Aztec Chunky 5DTemple Enemy: Start Left */
    {14041808u, 1159, AP_DETECT_ENEMY}, /* Aztec Chunky 5DTemple Enemy: Second Right */
    {14041809u, 1160, AP_DETECT_ENEMY}, /* Aztec Chunky 5DTemple Enemy: Second Left */
    {14041810u, 1161, AP_DETECT_ENEMY}, /* Aztec Chunky 5DTemple Enemy: Reward */
    {14041811u, 1162, AP_DETECT_ENEMY}, /* Aztec Llama Temple Enemy: Kong Free Instrument */
    {14041812u, 1163, AP_DETECT_ENEMY}, /* Aztec Llama Temple Enemy: Dino Instrument */
    {14041813u, 1164, AP_DETECT_ENEMY}, /* Aztec Llama Temple Enemy: Matching0 */
    {14041814u, 1165, AP_DETECT_ENEMY}, /* Aztec Llama Temple Enemy: Matching1 */
    {14041815u, 1166, AP_DETECT_ENEMY}, /* Aztec Llama Temple Enemy: Right */
    {14041816u, 1167, AP_DETECT_ENEMY}, /* Aztec Llama Temple Enemy: Left */
    {14041817u, 1168, AP_DETECT_ENEMY}, /* Aztec Llama Temple Enemy: Melon Crate */
    {14041818u, 1169, AP_DETECT_ENEMY}, /* Aztec Llama Temple Enemy: Slam Switch */
    {14041819u, 1190, AP_DETECT_ENEMY}, /* Aztec Tiny Temple Enemy: Guard Rotating (0) */
    {14041820u, 1191, AP_DETECT_ENEMY}, /* Aztec Tiny Temple Enemy: Guard Rotating (1) */
    {14041821u, 1192, AP_DETECT_ENEMY}, /* Aztec Tiny Temple Enemy: Main Room (0) */
    {14041822u, 1193, AP_DETECT_ENEMY}, /* Aztec Tiny Temple Enemy: Main Room (1) */
    {14041823u, 1194, AP_DETECT_ENEMY}, /* Aztec Tiny Temple Enemy: Main Room (2) */
    {14041824u, 1195, AP_DETECT_ENEMY}, /* Aztec Tiny Temple Enemy: Kong Room (0) */
    {14041825u, 1196, AP_DETECT_ENEMY}, /* Aztec Tiny Temple Enemy: Kong Room (1) */
    {14041826u, 1197, AP_DETECT_ENEMY}, /* Aztec Tiny Temple Enemy: Kong Room (2) */
    {14041827u, 1198, AP_DETECT_ENEMY}, /* Aztec Tiny Temple Enemy: Kong Room (3) */
    {14041828u, 1199, AP_DETECT_ENEMY}, /* Aztec Tiny Temple Enemy: Kong Room (4) */
    {14041829u, 1201, AP_DETECT_ENEMY}, /* Factory Enemy: Candy Cranky (0) */
    {14041830u, 1202, AP_DETECT_ENEMY}, /* Factory Enemy: Candy Cranky (1) */
    {14041831u, 1203, AP_DETECT_ENEMY}, /* Factory Enemy: Lobby Left */
    {14041832u, 1204, AP_DETECT_ENEMY}, /* Factory Enemy: Lobby Right */
    {14041833u, 1205, AP_DETECT_ENEMY}, /* Factory Enemy: Storage Room */
    {14041834u, 1206, AP_DETECT_ENEMY}, /* Factory Enemy: Block Tower (0) */
    {14041835u, 1207, AP_DETECT_ENEMY}, /* Factory Enemy: Block Tower (1) */
    {14041836u, 1208, AP_DETECT_ENEMY}, /* Factory Enemy: Block Tower (2) */
    {14041837u, 1209, AP_DETECT_ENEMY}, /* Factory Enemy: Tunnel To Hatch */
    {14041838u, 1210, AP_DETECT_ENEMY}, /* Factory Enemy: Tunnel To Prod (0) */
    {14041839u, 1211, AP_DETECT_ENEMY}, /* Factory Enemy: Tunnel To Prod (1) */
    {14041840u, 1212, AP_DETECT_ENEMY}, /* Factory Enemy: Tunnel To Block Tower */
    {14041841u, 1213, AP_DETECT_ENEMY}, /* Factory Enemy: Tunnel To Race (0) */
    {14041842u, 1214, AP_DETECT_ENEMY}, /* Factory Enemy: Tunnel To Race (1) */
    {14041843u, 1215, AP_DETECT_ENEMY}, /* Factory Enemy: Low Warp 4 */
    {14041844u, 1216, AP_DETECT_ENEMY}, /* Factory Enemy: Diddy Switch */
    {14041845u, 1217, AP_DETECT_ENEMY}, /* Factory Enemy: To Block Tower Tunnel */
    {14041846u, 1218, AP_DETECT_ENEMY}, /* Factory Enemy: Dark Room (0) */
    {14041847u, 1219, AP_DETECT_ENEMY}, /* Factory Enemy: Dark Room (1) */
    {14041848u, 1238, AP_DETECT_ENEMY}, /* Isles Factory Lobby Enemy: Enemy (0) */
    {14041849u, 1239, AP_DETECT_ENEMY}, /* Galleon Enemy: Chest Room (0) */
    {14041850u, 1240, AP_DETECT_ENEMY}, /* Galleon Enemy: Chest Room (1) */
    {14041851u, 1241, AP_DETECT_ENEMY}, /* Galleon Enemy: Near Vine Cannon */
    {14041852u, 1242, AP_DETECT_ENEMY}, /* Galleon Enemy: Cranky Cannon */
    {14041853u, 1246, AP_DETECT_ENEMY}, /* Galleon Enemy: Peanut Tunnel */
    {14041854u, 1247, AP_DETECT_ENEMY}, /* Galleon Enemy: Coconut Tunnel */
    {14041855u, 1248, AP_DETECT_ENEMY}, /* Galleon Lighthouse Enemy: Enemy (0) */
    {14041856u, 1249, AP_DETECT_ENEMY}, /* Galleon Lighthouse Enemy: Enemy (1) */
    {14041857u, 1264, AP_DETECT_ENEMY}, /* Forest Enemy: Hollow Tree (0) */
    {14041858u, 1265, AP_DETECT_ENEMY}, /* Forest Enemy: Hollow Tree (1) */
    {14041859u, 1266, AP_DETECT_ENEMY}, /* Forest Enemy: Hollow Tree Entrance */
    {14041860u, 1267, AP_DETECT_ENEMY}, /* Forest Enemy: Tree Melon Crate (0) */
    {14041861u, 1268, AP_DETECT_ENEMY}, /* Forest Enemy: Tree Melon Crate (1) */
    {14041862u, 1269, AP_DETECT_ENEMY}, /* Forest Enemy: Tree Melon Crate (2) */
    {14041863u, 1270, AP_DETECT_ENEMY}, /* Forest Enemy: Apple Gauntlet (0) */
    {14041864u, 1271, AP_DETECT_ENEMY}, /* Forest Enemy: Apple Gauntlet (1) */
    {14041865u, 1272, AP_DETECT_ENEMY}, /* Forest Enemy: Apple Gauntlet (2) */
    {14041866u, 1273, AP_DETECT_ENEMY}, /* Forest Enemy: Apple Gauntlet (3) */
    {14041867u, 1274, AP_DETECT_ENEMY}, /* Forest Enemy: Near Beanstalk (0) */
    {14041868u, 1275, AP_DETECT_ENEMY}, /* Forest Enemy: Near Beanstalk (1) */
    {14041869u, 1276, AP_DETECT_ENEMY}, /* Forest Enemy: Green Tunnel */
    {14041870u, 1277, AP_DETECT_ENEMY}, /* Forest Enemy: Near Low Warp 5 */
    {14041871u, 1278, AP_DETECT_ENEMY}, /* Forest Enemy: Near Pink Tunnel Bounce Tag */
    {14041872u, 1279, AP_DETECT_ENEMY}, /* Forest Enemy: Near Giant Mushroom Rocketbarrel */
    {14041873u, 1280, AP_DETECT_ENEMY}, /* Forest Enemy: Between Yellow Tunnel And RB */
    {14041874u, 1281, AP_DETECT_ENEMY}, /* Forest Enemy: Near Cranky */
    {14041875u, 1282, AP_DETECT_ENEMY}, /* Forest Enemy: Near Pink Tunnel Giant Mushroom */
    {14041876u, 1283, AP_DETECT_ENEMY}, /* Forest Enemy: Giant Mushroom Rear Tag */
    {14041877u, 1284, AP_DETECT_ENEMY}, /* Forest Enemy: Near Face Puzzle */
    {14041878u, 1285, AP_DETECT_ENEMY}, /* Forest Enemy: Near Crown */
    {14041879u, 1286, AP_DETECT_ENEMY}, /* Forest Enemy: Near High Warp 5 */
    {14041880u, 1287, AP_DETECT_ENEMY}, /* Forest Enemy: Top Of Mushroom */
    {14041881u, 1288, AP_DETECT_ENEMY}, /* Forest Enemy: Near Apple Dropoff */
    {14041882u, 1289, AP_DETECT_ENEMY}, /* Forest Enemy: Near DKPortal */
    {14041883u, 1290, AP_DETECT_ENEMY}, /* Forest Enemy: Near Well Tag */
    {14041884u, 1291, AP_DETECT_ENEMY}, /* Forest Enemy: Yellow Tunnel (0) */
    {14041885u, 1292, AP_DETECT_ENEMY}, /* Forest Enemy: Yellow Tunnel (1) */
    {14041886u, 1293, AP_DETECT_ENEMY}, /* Forest Enemy: Yellow Tunnel (2) */
    {14041887u, 1294, AP_DETECT_ENEMY}, /* Forest Enemy: Yellow Tunnel (3) */
    {14041888u, 1295, AP_DETECT_ENEMY}, /* Forest Enemy: Near Snide */
    {14041889u, 1296, AP_DETECT_ENEMY}, /* Forest Enemy: Near the hidden Rainbow Coin */
    {14041890u, 1297, AP_DETECT_ENEMY}, /* Forest Enemy: Near BBlast */
    {14041891u, 1298, AP_DETECT_ENEMY}, /* Forest Enemy: Near Dark Attic */
    {14041892u, 1299, AP_DETECT_ENEMY}, /* Forest Enemy: Near Well Exit */
    {14041893u, 1300, AP_DETECT_ENEMY}, /* Forest Enemy: Near Blue Tunnel */
    {14041894u, 1301, AP_DETECT_ENEMY}, /* Forest Enemy: Thornvine (0) */
    {14041895u, 1302, AP_DETECT_ENEMY}, /* Forest Enemy: Thornvine (1) */
    {14041896u, 1303, AP_DETECT_ENEMY}, /* Forest Enemy: Thornvine (2) */
    {14041897u, 1304, AP_DETECT_ENEMY}, /* Forest Enemy: Thornvine Entrance */
    {14041898u, 1305, AP_DETECT_ENEMY}, /* Forest Anthill Enemy: Gauntlet (0) */
    {14041899u, 1306, AP_DETECT_ENEMY}, /* Forest Anthill Enemy: Gauntlet (1) */
    {14041900u, 1307, AP_DETECT_ENEMY}, /* Forest Anthill Enemy: Gauntlet (2) */
    {14041901u, 1308, AP_DETECT_ENEMY}, /* Forest Anthill Enemy: Gauntlet (3) */
    {14041902u, 1309, AP_DETECT_ENEMY}, /* Forest Winch Room Enemy: Enemy */
    {14041903u, 1310, AP_DETECT_ENEMY}, /* Forest Thornvine Barn Enemy: Enemy */
    {14041904u, 1311, AP_DETECT_ENEMY}, /* Forest Mill Front Enemy: Enemy */
    {14041905u, 1312, AP_DETECT_ENEMY}, /* Forest Mill Back Enemy: Enemy */
    {14041906u, 1313, AP_DETECT_ENEMY}, /* Forest Giant Mushroom Enemy: Above Night Door */
    {14041907u, 1314, AP_DETECT_ENEMY}, /* Forest Giant Mushroom Enemy: Path (0) */
    {14041908u, 1315, AP_DETECT_ENEMY}, /* Forest Giant Mushroom Enemy: Path (1) */
    {14041909u, 1319, AP_DETECT_ENEMY}, /* Forest Lanky Zingers Room Enemy: Enemy (0) */
    {14041910u, 1320, AP_DETECT_ENEMY}, /* Forest Lanky Zingers Room Enemy: Enemy (1) */
    {14041911u, 1321, AP_DETECT_ENEMY}, /* Forest Chunky Face Room Enemy: Enemy */
    {14041912u, 1325, AP_DETECT_ENEMY}, /* Caves Enemy: Start */
    {14041913u, 1326, AP_DETECT_ENEMY}, /* Caves Enemy: Near Ice Castle */
    {14041914u, 1327, AP_DETECT_ENEMY}, /* Caves Enemy: Outside 5DC */
    {14041915u, 1328, AP_DETECT_ENEMY}, /* Caves Enemy: 1DC Waterfall */
    {14041916u, 1329, AP_DETECT_ENEMY}, /* Caves Enemy: Near Funky */
    {14041917u, 1330, AP_DETECT_ENEMY}, /* Caves Enemy: Near Snide */
    {14041918u, 1331, AP_DETECT_ENEMY}, /* Caves Enemy: Near Bonus Room */
    {14041919u, 1332, AP_DETECT_ENEMY}, /* Caves Enemy: 1DC Headphones */
    {14041920u, 1334, AP_DETECT_ENEMY}, /* Caves Donkey Igloo Enemy: Right */
    {14041921u, 1335, AP_DETECT_ENEMY}, /* Caves Donkey Igloo Enemy: Left */
    {14041922u, 1341, AP_DETECT_ENEMY}, /* Caves Tiny Igloo Enemy: Big Enemy */
    {14041923u, 1347, AP_DETECT_ENEMY}, /* Caves Lanky Cabin Enemy: Near */
    {14041924u, 1370, AP_DETECT_ENEMY}, /* Castle Enemy: Near Bridge (0) */
    {14041925u, 1371, AP_DETECT_ENEMY}, /* Castle Enemy: Near Bridge (1) */
    {14041926u, 1372, AP_DETECT_ENEMY}, /* Castle Enemy: Wooden Extrusion (0) */
    {14041927u, 1373, AP_DETECT_ENEMY}, /* Castle Enemy: Wooden Extrusion (1) */
    {14041928u, 1374, AP_DETECT_ENEMY}, /* Castle Enemy: Near Shed */
    {14041929u, 1375, AP_DETECT_ENEMY}, /* Castle Enemy: Near Library */
    {14041930u, 1376, AP_DETECT_ENEMY}, /* Castle Enemy: Near Tower */
    {14041931u, 1377, AP_DETECT_ENEMY}, /* Castle Enemy: Museum Steps */
    {14041932u, 1378, AP_DETECT_ENEMY}, /* Castle Enemy: Near Low Cave */
    {14041933u, 1379, AP_DETECT_ENEMY}, /* Castle Enemy: Path To Low Kasplat */
    {14041934u, 1380, AP_DETECT_ENEMY}, /* Castle Enemy: Low TnS */
    {14041935u, 1381, AP_DETECT_ENEMY}, /* Castle Enemy: Path To Dungeon */
    {14041936u, 1382, AP_DETECT_ENEMY}, /* Castle Enemy: Near Headphones */
    {14041937u, 1383, AP_DETECT_ENEMY}, /* Isles Castle Lobby Enemy: Left */
    {14041938u, 1384, AP_DETECT_ENEMY}, /* Isles Castle Lobby Enemy: Far Right */
    {14041939u, 1385, AP_DETECT_ENEMY}, /* Isles Castle Lobby Enemy: Near Right */
    {14041940u, 1391, AP_DETECT_ENEMY}, /* Castle Ballroom Enemy: Start */
    {14041941u, 1392, AP_DETECT_ENEMY}, /* Castle Dungeon Enemy: Face Room */
    {14041942u, 1393, AP_DETECT_ENEMY}, /* Castle Dungeon Enemy: Chair Room */
    {14041943u, 1394, AP_DETECT_ENEMY}, /* Castle Dungeon Enemy: Outside Lanky Room */
    {14041944u, 1400, AP_DETECT_ENEMY}, /* Castle Lower Cave Enemy: Near Crypt */
    {14041945u, 1401, AP_DETECT_ENEMY}, /* Castle Lower Cave Enemy: Stair Right */
    {14041946u, 1402, AP_DETECT_ENEMY}, /* Castle Lower Cave Enemy: Stair Left */
    {14041947u, 1403, AP_DETECT_ENEMY}, /* Castle Lower Cave Enemy: Near Mausoleum */
    {14041948u, 1404, AP_DETECT_ENEMY}, /* Castle Lower Cave Enemy: Near Funky */
    {14041949u, 1405, AP_DETECT_ENEMY}, /* Castle Lower Cave Enemy: Near Tag */
    {14041950u, 1406, AP_DETECT_ENEMY}, /* Castle Crypt Enemy: Diddy Coffin (0) */
    {14041951u, 1407, AP_DETECT_ENEMY}, /* Castle Crypt Enemy: Diddy Coffin (1) */
    {14041952u, 1408, AP_DETECT_ENEMY}, /* Castle Crypt Enemy: Diddy Coffin (2) */
    {14041953u, 1409, AP_DETECT_ENEMY}, /* Castle Crypt Enemy: Diddy Coffin (3) */
    {14041954u, 1410, AP_DETECT_ENEMY}, /* Castle Crypt Enemy: Chunky Coffin (0) */
    {14041955u, 1411, AP_DETECT_ENEMY}, /* Castle Crypt Enemy: Chunky Coffin (1) */
    {14041956u, 1412, AP_DETECT_ENEMY}, /* Castle Crypt Enemy: Chunky Coffin (2) */
    {14041957u, 1413, AP_DETECT_ENEMY}, /* Castle Crypt Enemy: Chunky Coffin (3) */
    {14041958u, 1414, AP_DETECT_ENEMY}, /* Castle Crypt Enemy: Minecart Entry */
    {14041959u, 1415, AP_DETECT_ENEMY}, /* Castle Crypt Enemy: Fork */
    {14041960u, 1416, AP_DETECT_ENEMY}, /* Castle Crypt Enemy: Near Diddy */
    {14041961u, 1417, AP_DETECT_ENEMY}, /* Castle Crypt Enemy: Near Chunky */
    {14041962u, 1418, AP_DETECT_ENEMY}, /* Castle Mausoleum Enemy: Tiny Path */
    {14041963u, 1419, AP_DETECT_ENEMY}, /* Castle Mausoleum Enemy: Lanky Path (0) */
    {14041964u, 1420, AP_DETECT_ENEMY}, /* Castle Mausoleum Enemy: Lanky Path (1) */
    {14041965u, 1421, AP_DETECT_ENEMY}, /* Castle Upper Cave Enemy: Near Dungeon */
    {14041966u, 1423, AP_DETECT_ENEMY}, /* Castle Upper Cave Enemy: Near Pit */
    {14041967u, 1424, AP_DETECT_ENEMY}, /* Castle Upper Cave Enemy: Near Entrance */
    {14041968u, 1440, AP_DETECT_ENEMY}, /* Castle Library Enemy: Fork Left (0) */
    {14041969u, 1441, AP_DETECT_ENEMY}, /* Castle Library Enemy: Fork Left (1) */
    {14041970u, 1442, AP_DETECT_ENEMY}, /* Castle Library Enemy: Fork Center */
    {14041971u, 1443, AP_DETECT_ENEMY}, /* Castle Library Enemy: Fork Right */
    {14041972u, 1444, AP_DETECT_ENEMY}, /* Castle Museum Enemy: Main Floor (0) */
    {14041973u, 1445, AP_DETECT_ENEMY}, /* Castle Museum Enemy: Main Floor (1) */
    {14041974u, 1446, AP_DETECT_ENEMY}, /* Castle Museum Enemy: Main Floor (2) */
    {14041975u, 1447, AP_DETECT_ENEMY}, /* Castle Museum Enemy: Main Floor (3) */
    {14041976u, 1448, AP_DETECT_ENEMY}, /* Castle Museum Enemy: Start */
    {14041977u, 1459, AP_DETECT_ENEMY}, /* Castle Tree Enemy: Start Room (0) */
    {14041978u, 1460, AP_DETECT_ENEMY}, /* Castle Tree Enemy: Start Room (1) */
    {14041979u, 1461, AP_DETECT_ENEMY}, /* Helm Enemy: Start (0) */
    {14041980u, 1462, AP_DETECT_ENEMY}, /* Helm Enemy: Start (1) */
    {14041981u, 1463, AP_DETECT_ENEMY}, /* Helm Enemy: Hill */
    {14041982u, 1464, AP_DETECT_ENEMY}, /* Helm Enemy: Switch Room (0) */
    {14041983u, 1465, AP_DETECT_ENEMY}, /* Helm Enemy: Switch Room (1) */
    {14041984u, 1466, AP_DETECT_ENEMY}, /* Helm Enemy: Mini Room (0) */
    {14041985u, 1467, AP_DETECT_ENEMY}, /* Helm Enemy: Mini Room (1) */
    {14041986u, 1468, AP_DETECT_ENEMY}, /* Helm Enemy: Mini Room (2) */
    {14041987u, 1469, AP_DETECT_ENEMY}, /* Helm Enemy: Mini Room (3) */
    {14041988u, 1470, AP_DETECT_ENEMY}, /* Helm Enemy: DKRoom */
    {14041989u, 1471, AP_DETECT_ENEMY}, /* Helm Enemy: Chunky Room (0) */
    {14041990u, 1472, AP_DETECT_ENEMY}, /* Helm Enemy: Chunky Room (1) */
    {14041991u, 1473, AP_DETECT_ENEMY}, /* Helm Enemy: Tiny Room */
    {14041992u, 1474, AP_DETECT_ENEMY}, /* Helm Enemy: Lanky Room (0) */
    {14041993u, 1475, AP_DETECT_ENEMY}, /* Helm Enemy: Lanky Room (1) */
    {14041994u, 1476, AP_DETECT_ENEMY}, /* Helm Enemy: Diddy Room (0) */
    {14041995u, 1477, AP_DETECT_ENEMY}, /* Helm Enemy: Diddy Room (1) */
    {14041996u, 1478, AP_DETECT_ENEMY}, /* Helm Enemy: Nav Right */
    {14041997u, 1479, AP_DETECT_ENEMY}, /* Helm Enemy: Nav Left */
    {14041998u, 1480, AP_DETECT_ENEMY}, /* Isles Enemy: Pineapple Cage (0) */
    {14041999u, 1481, AP_DETECT_ENEMY}, /* Isles Enemy: Fungi Cannon (0) */
    {14042000u, 1482, AP_DETECT_ENEMY}, /* Isles Enemy: Japes Entrance */
    {14042001u, 1483, AP_DETECT_ENEMY}, /* Isles Enemy: Monkeyport Pad */
    {14042002u, 1484, AP_DETECT_ENEMY}, /* Isles Enemy: Upper Factory Path */
    {14042003u, 1485, AP_DETECT_ENEMY}, /* Isles Enemy: Near Aztec */
    {14042004u, 1486, AP_DETECT_ENEMY}, /* Isles Enemy: Fungi Cannon (1) */
    {14042005u, 1487, AP_DETECT_ENEMY}, /* Isles Enemy: Pineapple Cage (1) */
    {14042006u, 1488, AP_DETECT_ENEMY}, /* Isles Enemy: Lower Factory Path (0) */
    {14042007u, 1489, AP_DETECT_ENEMY}, /* Isles Enemy: Lower Factory Path (1) */
    {14042329u, 982, AP_DETECT_EVENT}, /* Japes Donkey Half Medal */
    {14042330u, 983, AP_DETECT_EVENT}, /* Japes Diddy Half Medal */
    {14042331u, 984, AP_DETECT_EVENT}, /* Japes Lanky Half Medal */
    {14042332u, 985, AP_DETECT_EVENT}, /* Japes Tiny Half Medal */
    {14042333u, 986, AP_DETECT_EVENT}, /* Japes Chunky Half Medal */
    {14042334u, 987, AP_DETECT_EVENT}, /* Aztec Donkey Half Medal */
    {14042335u, 988, AP_DETECT_EVENT}, /* Aztec Diddy Half Medal */
    {14042336u, 989, AP_DETECT_EVENT}, /* Aztec Lanky Half Medal */
    {14042337u, 990, AP_DETECT_EVENT}, /* Aztec Tiny Half Medal */
    {14042338u, 991, AP_DETECT_EVENT}, /* Aztec Chunky Half Medal */
    {14042339u, 992, AP_DETECT_EVENT}, /* Factory Donkey Half Medal */
    {14042340u, 993, AP_DETECT_EVENT}, /* Factory Diddy Half Medal */
    {14042341u, 994, AP_DETECT_EVENT}, /* Factory Lanky Half Medal */
    {14042342u, 995, AP_DETECT_EVENT}, /* Factory Tiny Half Medal */
    {14042343u, 996, AP_DETECT_EVENT}, /* Factory Chunky Half Medal */
    {14042344u, 997, AP_DETECT_EVENT}, /* Galleon Donkey Half Medal */
    {14042345u, 998, AP_DETECT_EVENT}, /* Galleon Diddy Half Medal */
    {14042346u, 999, AP_DETECT_EVENT}, /* Galleon Lanky Half Medal */
    {14042347u, 1000, AP_DETECT_EVENT}, /* Galleon Tiny Half Medal */
    {14042348u, 1001, AP_DETECT_EVENT}, /* Galleon Chunky Half Medal */
    {14042349u, 1002, AP_DETECT_EVENT}, /* Forest Donkey Half Medal */
    {14042350u, 1003, AP_DETECT_EVENT}, /* Forest Diddy Half Medal */
    {14042351u, 1004, AP_DETECT_EVENT}, /* Forest Lanky Half Medal */
    {14042352u, 1005, AP_DETECT_EVENT}, /* Forest Tiny Half Medal */
    {14042353u, 1006, AP_DETECT_EVENT}, /* Forest Chunky Half Medal */
    {14042354u, 1007, AP_DETECT_EVENT}, /* Caves Donkey Half Medal */
    {14042355u, 1008, AP_DETECT_EVENT}, /* Caves Diddy Half Medal */
    {14042356u, 1009, AP_DETECT_EVENT}, /* Caves Lanky Half Medal */
    {14042357u, 1010, AP_DETECT_EVENT}, /* Caves Tiny Half Medal */
    {14042358u, 1011, AP_DETECT_EVENT}, /* Caves Chunky Half Medal */
    {14042359u, 1012, AP_DETECT_EVENT}, /* Castle Donkey Half Medal */
    {14042360u, 1013, AP_DETECT_EVENT}, /* Castle Diddy Half Medal */
    {14042361u, 1014, AP_DETECT_EVENT}, /* Castle Lanky Half Medal */
    {14042362u, 1015, AP_DETECT_EVENT}, /* Castle Tiny Half Medal */
    {14042363u, 1016, AP_DETECT_EVENT}, /* Castle Chunky Half Medal */
};
#define AP_ITEM_COUNT 184
static const unsigned int ap_item_ids[AP_ITEM_COUNT] = {
    14041088u, /* Victory */
    14041089u, /* No Item */
    14041090u, /* Fill Helper Item - SHOULD NOT BE PLACED */
    14041091u, /* Donkey */
    14041092u, /* Diddy */
    14041093u, /* Lanky */
    14041094u, /* Tiny */
    14041095u, /* Chunky */
    14041096u, /* Vines */
    14041097u, /* Diving */
    14041098u, /* Oranges */
    14041099u, /* Barrels */
    14041100u, /* Climbing */
    14041101u, /* progression Slam */
    14041102u, /* progression Slam  */
    14041103u, /* progression Donkey Potion */
    14041104u, /* Baboon Blast */
    14041105u, /* Strong Kong */
    14041106u, /* Gorilla Grab */
    14041107u, /* progression Diddy Potion */
    14041108u, /* Chimpy Charge */
    14041109u, /* Rocketbarrel Boost */
    14041110u, /* Simian Spring */
    14041111u, /* progression Lanky Potion */
    14041112u, /* Orangstand */
    14041113u, /* Baboon Balloon */
    14041114u, /* Orangstand Sprint */
    14041115u, /* progression Tiny Potion */
    14041116u, /* Mini Monkey */
    14041117u, /* Pony Tail Twirl */
    14041118u, /* Monkeyport */
    14041119u, /* progression Chunky Potion */
    14041120u, /* Hunky Chunky */
    14041121u, /* Primate Punch */
    14041122u, /* Gorilla Gone */
    14041123u, /* Coconut */
    14041124u, /* Peanut */
    14041125u, /* Grape */
    14041126u, /* Feather */
    14041127u, /* Pineapple */
    14041128u, /* Homing Ammo */
    14041129u, /* Sniper Sight */
    14041130u, /* progression Ammo Belt */
    14041131u, /* progression Ammo Belt  */
    14041132u, /* Bongos */
    14041133u, /* Guitar */
    14041134u, /* Trombone */
    14041135u, /* Saxophone */
    14041136u, /* Triangle */
    14041137u, /* progression Instrument Upgrade */
    14041138u, /* progression Instrument Upgrade  */
    14041139u, /* progression Instrument Upgrade   */
    14041140u, /* Fairy Camera */
    14041141u, /* Shockwave */
    14041142u, /* Camera and Shockwave */
    14041143u, /* Nintendo Coin */
    14041144u, /* Rareware Coin */
    14041145u, /* Key 1 */
    14041146u, /* Key 2 */
    14041147u, /* Key 3 */
    14041148u, /* Key 4 */
    14041149u, /* Key 5 */
    14041150u, /* Key 6 */
    14041151u, /* Key 7 */
    14041152u, /* Key 8 */
    14041153u, /* Helm Donkey Barrel 1 */
    14041154u, /* Helm Donkey Barrel 2 */
    14041155u, /* Helm Diddy Barrel 1 */
    14041156u, /* Helm Diddy Barrel 2 */
    14041157u, /* Helm Lanky Barrel 1 */
    14041158u, /* Helm Lanky Barrel 2 */
    14041159u, /* Helm Tiny Barrel 1 */
    14041160u, /* Helm Tiny Barrel 2 */
    14041161u, /* Helm Chunky Barrel 1 */
    14041162u, /* Helm Chunky Barrel 2 */
    14041167u, /* Bean */
    14041170u, /* Bubble Trap */
    14041171u, /* Reverse Trap */
    14041172u, /* Slow Trap */
    14041173u, /* Junk Item (Crystal) */
    14041174u, /* Junk Item (Melon Slice) */
    14041175u, /* Junk Item (Ammo Crate) */
    14041176u, /* Junk Item (Film) */
    14041177u, /* Junk Item (Orange) */
    14041178u, /* Crate Melon */
    14041179u, /* Enemy Item */
    14041180u, /* Cranky */
    14041181u, /* Funky */
    14041182u, /* Candy */
    14041183u, /* Snide */
    14041184u, /* Banana Hoard */
    14041185u, /* Japes Donkey Hint */
    14041186u, /* Japes Diddy Hint */
    14041187u, /* Japes Lanky Hint */
    14041188u, /* Japes Tiny Hint */
    14041189u, /* Japes Chunky Hint */
    14041190u, /* Aztec Donkey Hint */
    14041191u, /* Aztec Diddy Hint */
    14041192u, /* Aztec Lanky Hint */
    14041193u, /* Aztec Tiny Hint */
    14041194u, /* Aztec Chunky Hint */
    14041195u, /* Factory Donkey Hint */
    14041196u, /* Factory Diddy Hint */
    14041197u, /* Factory Lanky Hint */
    14041198u, /* Factory Tiny Hint */
    14041199u, /* Factory Chunky Hint */
    14041200u, /* Galleon Donkey Hint */
    14041201u, /* Galleon Diddy Hint */
    14041202u, /* Galleon Lanky Hint */
    14041203u, /* Galleon Tiny Hint */
    14041204u, /* Galleon Chunky Hint */
    14041205u, /* Forest Donkey Hint */
    14041206u, /* Forest Diddy Hint */
    14041207u, /* Forest Lanky Hint */
    14041208u, /* Forest Tiny Hint */
    14041209u, /* Forest Chunky Hint */
    14041210u, /* Caves Donkey Hint */
    14041211u, /* Caves Diddy Hint */
    14041212u, /* Caves Lanky Hint */
    14041213u, /* Caves Tiny Hint */
    14041214u, /* Caves Chunky Hint */
    14041215u, /* Castle Donkey Hint */
    14041216u, /* Castle Diddy Hint */
    14041217u, /* Castle Lanky Hint */
    14041218u, /* Castle Tiny Hint */
    14041219u, /* Castle Chunky Hint */
    14041260u, /* progression Slam   */
    14041262u, /* Ice Trap (Bubble - Bean) */
    14041263u, /* Ice Trap (Reverse - Bean) */
    14041264u, /* Ice Trap (Slow - Bean) */
    14041265u, /* Ice Trap (Bubble - Key) */
    14041266u, /* Ice Trap (Reverse - Key) */
    14041267u, /* Ice Trap (Slow - Key) */
    14041269u, /* Pearl */
    14041270u, /* Golden Banana */
    14041271u, /* Banana Fairy */
    14041272u, /* Battle Crown */
    14041273u, /* Banana Medal */
    14041274u, /* Disable A Trap */
    14041275u, /* Ice Trap (Disable A - Bean) */
    14041276u, /* Ice Trap (Disable A - Key) */
    14041277u, /* Disable B Trap */
    14041278u, /* Ice Trap (Disable B - Bean) */
    14041279u, /* Ice Trap (Disable B - Key) */
    14041280u, /* Disable Z Trap */
    14041281u, /* Ice Trap (Disable Z - Bean) */
    14041282u, /* Ice Trap (Disable Z - Key) */
    14041283u, /* Disable C Up Trap */
    14041284u, /* Ice Trap (Disable C Up - Bean) */
    14041285u, /* Ice Trap (Disable C Up - Key) */
    14041314u, /* Treasure Chest Far Left Clam */
    14041315u, /* Treasure Chest Center Clam */
    14041316u, /* Treasure Chest Far Right Clam */
    14041317u, /* Treasure Chest Close Right Clam */
    14041318u, /* Treasure Chest Close Left Clam */
    14041323u, /* Get Out Trap */
    14041326u, /* Dry Trap */
    14041329u, /* Flip Trap */
    14041342u, /* Ice Floor Trap */
    14041346u, /* Paper Trap */
    14041350u, /* Slip Trap */
    14041353u, /* Forest Second Anthill Reward */
    14041354u, /* Rainbow Coin */
    14041358u, /* Donkey Blueprint */
    14041359u, /* Diddy Blueprint */
    14041360u, /* Lanky Blueprint */
    14041361u, /* Tiny Blueprint */
    14041362u, /* Chunky Blueprint */
    14041369u, /* Animal Trap */
    14041373u, /* Rockfall Trap */
    14041377u, /* Disable Tag Trap */
    14041689u, /* Japes Crate: Behind the Mountain */
    14041690u, /* Japes Crate: In the Rambi Cave */
    14041691u, /* Aztec Crate: Llama Temple Entrance */
    14041692u, /* Factory Crate: Near Funky */
    14041693u, /* Factory Crate: Near Candy */
    14041694u, /* Galleon Crate: Near Cactus */
    14041695u, /* Aztec Crate: On Llama Temple */
    14041696u, /* Aztec Crate: Near Gong Tower */
    14041697u, /* Forest Crate: Near Owl Tree */
    14041698u, /* Forest Crate: Near Thornvine Barn */
    14041699u, /* Forest Crate: Behind Dark Attic */
    14041700u, /* Forest Crate: In Thornvine Barn */
    14041701u, /* Castle Crate: Behind Mausoleum Entrance */
};
enum { AP_EFFECT_UNSUPPORTED, AP_EFFECT_NONE, AP_EFFECT_FLAG, AP_EFFECT_KONG, AP_EFFECT_KEY,
    AP_EFFECT_ABILITY, AP_EFFECT_SPECIAL, AP_EFFECT_HINT, AP_EFFECT_BLUEPRINT, AP_EFFECT_FAIRY,
    AP_EFFECT_CROWN, AP_EFFECT_MEDAL, AP_EFFECT_PEARL, AP_EFFECT_RAINBOW, AP_EFFECT_JUNK,
    AP_EFFECT_TRAP, AP_EFFECT_TRANSFER };
typedef struct { unsigned char kind, arg0; unsigned short arg1; } ap_item_effect;
typedef struct { unsigned int id; ap_item_effect effects[2]; } ap_item_effect_def;
#define AP_TRAP_TYPE_COUNT 16
#define AP_ITEM_FLAG_WORDS 31
enum {
    AP_TRAP_TYPE_ANIMAL = 0,
    AP_TRAP_TYPE_BUBBLE = 1,
    AP_TRAP_TYPE_DISABLE_A = 2,
    AP_TRAP_TYPE_DISABLE_B = 3,
    AP_TRAP_TYPE_DISABLE_C_UP = 4,
    AP_TRAP_TYPE_DISABLE_Z = 5,
    AP_TRAP_TYPE_DISABLETAG = 6,
    AP_TRAP_TYPE_DRY = 7,
    AP_TRAP_TYPE_FLIP = 8,
    AP_TRAP_TYPE_GET_OUT = 9,
    AP_TRAP_TYPE_ICEFLOOR = 10,
    AP_TRAP_TYPE_PAPER = 11,
    AP_TRAP_TYPE_REVERSE = 12,
    AP_TRAP_TYPE_ROCKFALL = 13,
    AP_TRAP_TYPE_SLIP = 14,
    AP_TRAP_TYPE_SLOW = 15,
};
static const char* const ap_trap_type_names[AP_TRAP_TYPE_COUNT] = {
    "animal",
    "bubble",
    "disable_a",
    "disable_b",
    "disable_c_up",
    "disable_z",
    "disabletag",
    "dry",
    "flip",
    "get_out",
    "icefloor",
    "paper",
    "reverse",
    "rockfall",
    "slip",
    "slow",
};
enum { AP_TRAP_MAP_BANNED, AP_TRAP_MAP_SUPER, AP_TRAP_MAP_ALLOW };
#define AP_TRAP_MAP_COUNT 221
static const unsigned char ap_trap_map_state[AP_TRAP_MAP_COUNT] = {
    2, 0, 0, 1, 2, 0, 0, 2, 1, 0, 0, 0, 2, 2, 1, 0, 2, 2, 0, 2, 2, 2, 2, 2,
    2, 0, 2, 0, 0, 2, 2, 2, 0, 2, 2, 1, 2, 1, 2, 0, 0, 1, 1, 2, 2, 2, 2, 2,
    2, 2, 1, 2, 2, 1, 1, 0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 2, 2,
    2, 1, 1, 1, 0, 0, 0, 0, 0, 0, 2, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    1, 2, 2, 1, 2, 0, 0, 0, 0, 2, 2, 0, 2, 0, 1, 0, 2, 2, 2, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 1, 2, 2,
    2, 2, 2, 2, 0, 2, 2, 2, 2, 1, 2, 2, 1, 1, 1, 2, 0, 0, 1, 1, 1, 2, 0, 0,
    0, 2, 2, 2, 1, 1, 0, 1, 2, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 0, 1, 0,
    0, 0, 0, 0, 0,
};
#define AP_TRAP_MOVEMENT_COUNT 137
static const unsigned char ap_trap_movement_banned[AP_TRAP_MOVEMENT_COUNT] = {
    0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0,
    0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0,
    0, 0, 0, 1, 1, 1, 0, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1,
};
static const ap_item_effect_def ap_item_effects[AP_ITEM_COUNT] = {
    {14041088u, {{AP_EFFECT_UNSUPPORTED, 0, 0}, {AP_EFFECT_NONE, 0, 0}}}, /* Victory */
    {14041089u, {{AP_EFFECT_NONE, 0, 0}, {AP_EFFECT_NONE, 0, 0}}}, /* No Item */
    {14041090u, {{AP_EFFECT_UNSUPPORTED, 0, 0}, {AP_EFFECT_NONE, 0, 0}}}, /* Fill Helper Item - SHOULD NOT BE PLACED */
    {14041091u, {{AP_EFFECT_KONG, 0, 0}, {AP_EFFECT_NONE, 0, 0}}}, /* Donkey */
    {14041092u, {{AP_EFFECT_KONG, 1, 0}, {AP_EFFECT_NONE, 0, 0}}}, /* Diddy */
    {14041093u, {{AP_EFFECT_KONG, 2, 0}, {AP_EFFECT_NONE, 0, 0}}}, /* Lanky */
    {14041094u, {{AP_EFFECT_KONG, 3, 0}, {AP_EFFECT_NONE, 0, 0}}}, /* Tiny */
    {14041095u, {{AP_EFFECT_KONG, 4, 0}, {AP_EFFECT_NONE, 0, 0}}}, /* Chunky */
    {14041096u, {{AP_EFFECT_ABILITY, 16, 0}, {AP_EFFECT_NONE, 0, 0}}}, /* Vines */
    {14041097u, {{AP_EFFECT_ABILITY, 128, 0}, {AP_EFFECT_NONE, 0, 0}}}, /* Diving */
    {14041098u, {{AP_EFFECT_ABILITY, 64, 0}, {AP_EFFECT_NONE, 0, 0}}}, /* Oranges */
    {14041099u, {{AP_EFFECT_ABILITY, 32, 0}, {AP_EFFECT_NONE, 0, 0}}}, /* Barrels */
    {14041100u, {{AP_EFFECT_FLAG, 0, 671}, {AP_EFFECT_NONE, 0, 0}}}, /* Climbing */
    {14041101u, {{AP_EFFECT_TRANSFER, 2, 0}, {AP_EFFECT_NONE, 0, 0}}}, /* progression Slam */
    {14041102u, {{AP_EFFECT_TRANSFER, 2, 0}, {AP_EFFECT_NONE, 0, 0}}}, /* progression Slam  */
    {14041103u, {{AP_EFFECT_UNSUPPORTED, 0, 0}, {AP_EFFECT_NONE, 0, 0}}}, /* progression Donkey Potion */
    {14041104u, {{AP_EFFECT_TRANSFER, 26, 0}, {AP_EFFECT_NONE, 0, 0}}}, /* Baboon Blast */
    {14041105u, {{AP_EFFECT_TRANSFER, 27, 0}, {AP_EFFECT_NONE, 0, 0}}}, /* Strong Kong */
    {14041106u, {{AP_EFFECT_TRANSFER, 28, 0}, {AP_EFFECT_NONE, 0, 0}}}, /* Gorilla Grab */
    {14041107u, {{AP_EFFECT_UNSUPPORTED, 0, 0}, {AP_EFFECT_NONE, 0, 0}}}, /* progression Diddy Potion */
    {14041108u, {{AP_EFFECT_TRANSFER, 29, 0}, {AP_EFFECT_NONE, 0, 0}}}, /* Chimpy Charge */
    {14041109u, {{AP_EFFECT_TRANSFER, 30, 0}, {AP_EFFECT_NONE, 0, 0}}}, /* Rocketbarrel Boost */
    {14041110u, {{AP_EFFECT_TRANSFER, 31, 0}, {AP_EFFECT_NONE, 0, 0}}}, /* Simian Spring */
    {14041111u, {{AP_EFFECT_UNSUPPORTED, 0, 0}, {AP_EFFECT_NONE, 0, 0}}}, /* progression Lanky Potion */
    {14041112u, {{AP_EFFECT_TRANSFER, 32, 0}, {AP_EFFECT_NONE, 0, 0}}}, /* Orangstand */
    {14041113u, {{AP_EFFECT_TRANSFER, 33, 0}, {AP_EFFECT_NONE, 0, 0}}}, /* Baboon Balloon */
    {14041114u, {{AP_EFFECT_TRANSFER, 34, 0}, {AP_EFFECT_NONE, 0, 0}}}, /* Orangstand Sprint */
    {14041115u, {{AP_EFFECT_UNSUPPORTED, 0, 0}, {AP_EFFECT_NONE, 0, 0}}}, /* progression Tiny Potion */
    {14041116u, {{AP_EFFECT_TRANSFER, 35, 0}, {AP_EFFECT_NONE, 0, 0}}}, /* Mini Monkey */
    {14041117u, {{AP_EFFECT_TRANSFER, 36, 0}, {AP_EFFECT_NONE, 0, 0}}}, /* Pony Tail Twirl */
    {14041118u, {{AP_EFFECT_TRANSFER, 37, 0}, {AP_EFFECT_NONE, 0, 0}}}, /* Monkeyport */
    {14041119u, {{AP_EFFECT_UNSUPPORTED, 0, 0}, {AP_EFFECT_NONE, 0, 0}}}, /* progression Chunky Potion */
    {14041120u, {{AP_EFFECT_TRANSFER, 38, 0}, {AP_EFFECT_NONE, 0, 0}}}, /* Hunky Chunky */
    {14041121u, {{AP_EFFECT_TRANSFER, 39, 0}, {AP_EFFECT_NONE, 0, 0}}}, /* Primate Punch */
    {14041122u, {{AP_EFFECT_TRANSFER, 40, 0}, {AP_EFFECT_NONE, 0, 0}}}, /* Gorilla Gone */
    {14041123u, {{AP_EFFECT_TRANSFER, 46, 0}, {AP_EFFECT_NONE, 0, 0}}}, /* Coconut */
    {14041124u, {{AP_EFFECT_TRANSFER, 47, 0}, {AP_EFFECT_NONE, 0, 0}}}, /* Peanut */
    {14041125u, {{AP_EFFECT_TRANSFER, 48, 0}, {AP_EFFECT_NONE, 0, 0}}}, /* Grape */
    {14041126u, {{AP_EFFECT_TRANSFER, 49, 0}, {AP_EFFECT_NONE, 0, 0}}}, /* Feather */
    {14041127u, {{AP_EFFECT_TRANSFER, 50, 0}, {AP_EFFECT_NONE, 0, 0}}}, /* Pineapple */
    {14041128u, {{AP_EFFECT_TRANSFER, 52, 0}, {AP_EFFECT_NONE, 0, 0}}}, /* Homing Ammo */
    {14041129u, {{AP_EFFECT_TRANSFER, 53, 0}, {AP_EFFECT_NONE, 0, 0}}}, /* Sniper Sight */
    {14041130u, {{AP_EFFECT_TRANSFER, 54, 0}, {AP_EFFECT_NONE, 0, 0}}}, /* progression Ammo Belt */
    {14041131u, {{AP_EFFECT_TRANSFER, 54, 0}, {AP_EFFECT_NONE, 0, 0}}}, /* progression Ammo Belt  */
    {14041132u, {{AP_EFFECT_TRANSFER, 41, 0}, {AP_EFFECT_NONE, 0, 0}}}, /* Bongos */
    {14041133u, {{AP_EFFECT_TRANSFER, 42, 0}, {AP_EFFECT_NONE, 0, 0}}}, /* Guitar */
    {14041134u, {{AP_EFFECT_TRANSFER, 43, 0}, {AP_EFFECT_NONE, 0, 0}}}, /* Trombone */
    {14041135u, {{AP_EFFECT_TRANSFER, 44, 0}, {AP_EFFECT_NONE, 0, 0}}}, /* Saxophone */
    {14041136u, {{AP_EFFECT_TRANSFER, 45, 0}, {AP_EFFECT_NONE, 0, 0}}}, /* Triangle */
    {14041137u, {{AP_EFFECT_TRANSFER, 55, 0}, {AP_EFFECT_NONE, 0, 0}}}, /* progression Instrument Upgrade */
    {14041138u, {{AP_EFFECT_TRANSFER, 55, 0}, {AP_EFFECT_NONE, 0, 0}}}, /* progression Instrument Upgrade  */
    {14041139u, {{AP_EFFECT_TRANSFER, 55, 0}, {AP_EFFECT_NONE, 0, 0}}}, /* progression Instrument Upgrade   */
    {14041140u, {{AP_EFFECT_ABILITY, 8, 0}, {AP_EFFECT_NONE, 0, 0}}}, /* Fairy Camera */
    {14041141u, {{AP_EFFECT_ABILITY, 4, 0}, {AP_EFFECT_NONE, 0, 0}}}, /* Shockwave */
    {14041142u, {{AP_EFFECT_ABILITY, 8, 0}, {AP_EFFECT_ABILITY, 4, 0}}}, /* Camera and Shockwave */
    {14041143u, {{AP_EFFECT_SPECIAL, 1, 0}, {AP_EFFECT_NONE, 0, 0}}}, /* Nintendo Coin */
    {14041144u, {{AP_EFFECT_SPECIAL, 2, 0}, {AP_EFFECT_NONE, 0, 0}}}, /* Rareware Coin */
    {14041145u, {{AP_EFFECT_KEY, 0, 0}, {AP_EFFECT_NONE, 0, 0}}}, /* Key 1 */
    {14041146u, {{AP_EFFECT_KEY, 1, 0}, {AP_EFFECT_NONE, 0, 0}}}, /* Key 2 */
    {14041147u, {{AP_EFFECT_KEY, 2, 0}, {AP_EFFECT_NONE, 0, 0}}}, /* Key 3 */
    {14041148u, {{AP_EFFECT_KEY, 3, 0}, {AP_EFFECT_NONE, 0, 0}}}, /* Key 4 */
    {14041149u, {{AP_EFFECT_KEY, 4, 0}, {AP_EFFECT_NONE, 0, 0}}}, /* Key 5 */
    {14041150u, {{AP_EFFECT_KEY, 5, 0}, {AP_EFFECT_NONE, 0, 0}}}, /* Key 6 */
    {14041151u, {{AP_EFFECT_KEY, 6, 0}, {AP_EFFECT_NONE, 0, 0}}}, /* Key 7 */
    {14041152u, {{AP_EFFECT_KEY, 7, 0}, {AP_EFFECT_NONE, 0, 0}}}, /* Key 8 */
    {14041153u, {{AP_EFFECT_UNSUPPORTED, 0, 0}, {AP_EFFECT_NONE, 0, 0}}}, /* Helm Donkey Barrel 1 */
    {14041154u, {{AP_EFFECT_UNSUPPORTED, 0, 0}, {AP_EFFECT_NONE, 0, 0}}}, /* Helm Donkey Barrel 2 */
    {14041155u, {{AP_EFFECT_UNSUPPORTED, 0, 0}, {AP_EFFECT_NONE, 0, 0}}}, /* Helm Diddy Barrel 1 */
    {14041156u, {{AP_EFFECT_UNSUPPORTED, 0, 0}, {AP_EFFECT_NONE, 0, 0}}}, /* Helm Diddy Barrel 2 */
    {14041157u, {{AP_EFFECT_UNSUPPORTED, 0, 0}, {AP_EFFECT_NONE, 0, 0}}}, /* Helm Lanky Barrel 1 */
    {14041158u, {{AP_EFFECT_UNSUPPORTED, 0, 0}, {AP_EFFECT_NONE, 0, 0}}}, /* Helm Lanky Barrel 2 */
    {14041159u, {{AP_EFFECT_UNSUPPORTED, 0, 0}, {AP_EFFECT_NONE, 0, 0}}}, /* Helm Tiny Barrel 1 */
    {14041160u, {{AP_EFFECT_UNSUPPORTED, 0, 0}, {AP_EFFECT_NONE, 0, 0}}}, /* Helm Tiny Barrel 2 */
    {14041161u, {{AP_EFFECT_UNSUPPORTED, 0, 0}, {AP_EFFECT_NONE, 0, 0}}}, /* Helm Chunky Barrel 1 */
    {14041162u, {{AP_EFFECT_UNSUPPORTED, 0, 0}, {AP_EFFECT_NONE, 0, 0}}}, /* Helm Chunky Barrel 2 */
    {14041167u, {{AP_EFFECT_SPECIAL, 4, 0}, {AP_EFFECT_NONE, 0, 0}}}, /* Bean */
    {14041170u, {{AP_EFFECT_TRAP, 1, 0}, {AP_EFFECT_NONE, 0, 0}}}, /* Bubble Trap */
    {14041171u, {{AP_EFFECT_TRAP, 12, 0}, {AP_EFFECT_NONE, 0, 0}}}, /* Reverse Trap */
    {14041172u, {{AP_EFFECT_TRAP, 15, 0}, {AP_EFFECT_NONE, 0, 0}}}, /* Slow Trap */
    {14041173u, {{AP_EFFECT_JUNK, 0, 0}, {AP_EFFECT_NONE, 0, 0}}}, /* Junk Item (Crystal) */
    {14041174u, {{AP_EFFECT_JUNK, 0, 0}, {AP_EFFECT_NONE, 0, 0}}}, /* Junk Item (Melon Slice) */
    {14041175u, {{AP_EFFECT_JUNK, 0, 0}, {AP_EFFECT_NONE, 0, 0}}}, /* Junk Item (Ammo Crate) */
    {14041176u, {{AP_EFFECT_JUNK, 0, 0}, {AP_EFFECT_NONE, 0, 0}}}, /* Junk Item (Film) */
    {14041177u, {{AP_EFFECT_JUNK, 0, 0}, {AP_EFFECT_NONE, 0, 0}}}, /* Junk Item (Orange) */
    {14041178u, {{AP_EFFECT_JUNK, 0, 0}, {AP_EFFECT_NONE, 0, 0}}}, /* Crate Melon */
    {14041179u, {{AP_EFFECT_UNSUPPORTED, 0, 0}, {AP_EFFECT_NONE, 0, 0}}}, /* Enemy Item */
    {14041180u, {{AP_EFFECT_FLAG, 0, 962}, {AP_EFFECT_NONE, 0, 0}}}, /* Cranky */
    {14041181u, {{AP_EFFECT_FLAG, 0, 963}, {AP_EFFECT_NONE, 0, 0}}}, /* Funky */
    {14041182u, {{AP_EFFECT_FLAG, 0, 964}, {AP_EFFECT_NONE, 0, 0}}}, /* Candy */
    {14041183u, {{AP_EFFECT_FLAG, 0, 965}, {AP_EFFECT_NONE, 0, 0}}}, /* Snide */
    {14041184u, {{AP_EFFECT_UNSUPPORTED, 0, 0}, {AP_EFFECT_NONE, 0, 0}}}, /* Banana Hoard */
    {14041185u, {{AP_EFFECT_HINT, 0, 0}, {AP_EFFECT_NONE, 0, 0}}}, /* Japes Donkey Hint */
    {14041186u, {{AP_EFFECT_HINT, 1, 0}, {AP_EFFECT_NONE, 0, 0}}}, /* Japes Diddy Hint */
    {14041187u, {{AP_EFFECT_HINT, 2, 0}, {AP_EFFECT_NONE, 0, 0}}}, /* Japes Lanky Hint */
    {14041188u, {{AP_EFFECT_HINT, 3, 0}, {AP_EFFECT_NONE, 0, 0}}}, /* Japes Tiny Hint */
    {14041189u, {{AP_EFFECT_HINT, 4, 0}, {AP_EFFECT_NONE, 0, 0}}}, /* Japes Chunky Hint */
    {14041190u, {{AP_EFFECT_HINT, 0, 1}, {AP_EFFECT_NONE, 0, 0}}}, /* Aztec Donkey Hint */
    {14041191u, {{AP_EFFECT_HINT, 1, 1}, {AP_EFFECT_NONE, 0, 0}}}, /* Aztec Diddy Hint */
    {14041192u, {{AP_EFFECT_HINT, 2, 1}, {AP_EFFECT_NONE, 0, 0}}}, /* Aztec Lanky Hint */
    {14041193u, {{AP_EFFECT_HINT, 3, 1}, {AP_EFFECT_NONE, 0, 0}}}, /* Aztec Tiny Hint */
    {14041194u, {{AP_EFFECT_HINT, 4, 1}, {AP_EFFECT_NONE, 0, 0}}}, /* Aztec Chunky Hint */
    {14041195u, {{AP_EFFECT_HINT, 0, 2}, {AP_EFFECT_NONE, 0, 0}}}, /* Factory Donkey Hint */
    {14041196u, {{AP_EFFECT_HINT, 1, 2}, {AP_EFFECT_NONE, 0, 0}}}, /* Factory Diddy Hint */
    {14041197u, {{AP_EFFECT_HINT, 2, 2}, {AP_EFFECT_NONE, 0, 0}}}, /* Factory Lanky Hint */
    {14041198u, {{AP_EFFECT_HINT, 3, 2}, {AP_EFFECT_NONE, 0, 0}}}, /* Factory Tiny Hint */
    {14041199u, {{AP_EFFECT_HINT, 4, 2}, {AP_EFFECT_NONE, 0, 0}}}, /* Factory Chunky Hint */
    {14041200u, {{AP_EFFECT_HINT, 0, 3}, {AP_EFFECT_NONE, 0, 0}}}, /* Galleon Donkey Hint */
    {14041201u, {{AP_EFFECT_HINT, 1, 3}, {AP_EFFECT_NONE, 0, 0}}}, /* Galleon Diddy Hint */
    {14041202u, {{AP_EFFECT_HINT, 2, 3}, {AP_EFFECT_NONE, 0, 0}}}, /* Galleon Lanky Hint */
    {14041203u, {{AP_EFFECT_HINT, 3, 3}, {AP_EFFECT_NONE, 0, 0}}}, /* Galleon Tiny Hint */
    {14041204u, {{AP_EFFECT_HINT, 4, 3}, {AP_EFFECT_NONE, 0, 0}}}, /* Galleon Chunky Hint */
    {14041205u, {{AP_EFFECT_HINT, 0, 4}, {AP_EFFECT_NONE, 0, 0}}}, /* Forest Donkey Hint */
    {14041206u, {{AP_EFFECT_HINT, 1, 4}, {AP_EFFECT_NONE, 0, 0}}}, /* Forest Diddy Hint */
    {14041207u, {{AP_EFFECT_HINT, 2, 4}, {AP_EFFECT_NONE, 0, 0}}}, /* Forest Lanky Hint */
    {14041208u, {{AP_EFFECT_HINT, 3, 4}, {AP_EFFECT_NONE, 0, 0}}}, /* Forest Tiny Hint */
    {14041209u, {{AP_EFFECT_HINT, 4, 4}, {AP_EFFECT_NONE, 0, 0}}}, /* Forest Chunky Hint */
    {14041210u, {{AP_EFFECT_HINT, 0, 5}, {AP_EFFECT_NONE, 0, 0}}}, /* Caves Donkey Hint */
    {14041211u, {{AP_EFFECT_HINT, 1, 5}, {AP_EFFECT_NONE, 0, 0}}}, /* Caves Diddy Hint */
    {14041212u, {{AP_EFFECT_HINT, 2, 5}, {AP_EFFECT_NONE, 0, 0}}}, /* Caves Lanky Hint */
    {14041213u, {{AP_EFFECT_HINT, 3, 5}, {AP_EFFECT_NONE, 0, 0}}}, /* Caves Tiny Hint */
    {14041214u, {{AP_EFFECT_HINT, 4, 5}, {AP_EFFECT_NONE, 0, 0}}}, /* Caves Chunky Hint */
    {14041215u, {{AP_EFFECT_HINT, 0, 6}, {AP_EFFECT_NONE, 0, 0}}}, /* Castle Donkey Hint */
    {14041216u, {{AP_EFFECT_HINT, 1, 6}, {AP_EFFECT_NONE, 0, 0}}}, /* Castle Diddy Hint */
    {14041217u, {{AP_EFFECT_HINT, 2, 6}, {AP_EFFECT_NONE, 0, 0}}}, /* Castle Lanky Hint */
    {14041218u, {{AP_EFFECT_HINT, 3, 6}, {AP_EFFECT_NONE, 0, 0}}}, /* Castle Tiny Hint */
    {14041219u, {{AP_EFFECT_HINT, 4, 6}, {AP_EFFECT_NONE, 0, 0}}}, /* Castle Chunky Hint */
    {14041260u, {{AP_EFFECT_TRANSFER, 2, 0}, {AP_EFFECT_NONE, 0, 0}}}, /* progression Slam   */
    {14041262u, {{AP_EFFECT_TRAP, 1, 0}, {AP_EFFECT_NONE, 0, 0}}}, /* Ice Trap (Bubble - Bean) */
    {14041263u, {{AP_EFFECT_TRAP, 12, 0}, {AP_EFFECT_NONE, 0, 0}}}, /* Ice Trap (Reverse - Bean) */
    {14041264u, {{AP_EFFECT_TRAP, 15, 0}, {AP_EFFECT_NONE, 0, 0}}}, /* Ice Trap (Slow - Bean) */
    {14041265u, {{AP_EFFECT_TRAP, 1, 0}, {AP_EFFECT_NONE, 0, 0}}}, /* Ice Trap (Bubble - Key) */
    {14041266u, {{AP_EFFECT_TRAP, 12, 0}, {AP_EFFECT_NONE, 0, 0}}}, /* Ice Trap (Reverse - Key) */
    {14041267u, {{AP_EFFECT_TRAP, 15, 0}, {AP_EFFECT_NONE, 0, 0}}}, /* Ice Trap (Slow - Key) */
    {14041269u, {{AP_EFFECT_PEARL, 0, 0}, {AP_EFFECT_NONE, 0, 0}}}, /* Pearl */
    {14041270u, {{AP_EFFECT_TRANSFER, 1, 0}, {AP_EFFECT_NONE, 0, 0}}}, /* Golden Banana */
    {14041271u, {{AP_EFFECT_FAIRY, 0, 0}, {AP_EFFECT_NONE, 0, 0}}}, /* Banana Fairy */
    {14041272u, {{AP_EFFECT_CROWN, 0, 0}, {AP_EFFECT_NONE, 0, 0}}}, /* Battle Crown */
    {14041273u, {{AP_EFFECT_MEDAL, 0, 0}, {AP_EFFECT_NONE, 0, 0}}}, /* Banana Medal */
    {14041274u, {{AP_EFFECT_TRAP, 2, 0}, {AP_EFFECT_NONE, 0, 0}}}, /* Disable A Trap */
    {14041275u, {{AP_EFFECT_TRAP, 2, 0}, {AP_EFFECT_NONE, 0, 0}}}, /* Ice Trap (Disable A - Bean) */
    {14041276u, {{AP_EFFECT_TRAP, 2, 0}, {AP_EFFECT_NONE, 0, 0}}}, /* Ice Trap (Disable A - Key) */
    {14041277u, {{AP_EFFECT_TRAP, 3, 0}, {AP_EFFECT_NONE, 0, 0}}}, /* Disable B Trap */
    {14041278u, {{AP_EFFECT_TRAP, 3, 0}, {AP_EFFECT_NONE, 0, 0}}}, /* Ice Trap (Disable B - Bean) */
    {14041279u, {{AP_EFFECT_TRAP, 3, 0}, {AP_EFFECT_NONE, 0, 0}}}, /* Ice Trap (Disable B - Key) */
    {14041280u, {{AP_EFFECT_TRAP, 5, 0}, {AP_EFFECT_NONE, 0, 0}}}, /* Disable Z Trap */
    {14041281u, {{AP_EFFECT_TRAP, 5, 0}, {AP_EFFECT_NONE, 0, 0}}}, /* Ice Trap (Disable Z - Bean) */
    {14041282u, {{AP_EFFECT_TRAP, 5, 0}, {AP_EFFECT_NONE, 0, 0}}}, /* Ice Trap (Disable Z - Key) */
    {14041283u, {{AP_EFFECT_TRAP, 4, 0}, {AP_EFFECT_NONE, 0, 0}}}, /* Disable C Up Trap */
    {14041284u, {{AP_EFFECT_TRAP, 4, 0}, {AP_EFFECT_NONE, 0, 0}}}, /* Ice Trap (Disable C Up - Bean) */
    {14041285u, {{AP_EFFECT_TRAP, 4, 0}, {AP_EFFECT_NONE, 0, 0}}}, /* Ice Trap (Disable C Up - Key) */
    {14041314u, {{AP_EFFECT_FLAG, 0, 186}, {AP_EFFECT_NONE, 0, 0}}}, /* Treasure Chest Far Left Clam */
    {14041315u, {{AP_EFFECT_FLAG, 0, 187}, {AP_EFFECT_NONE, 0, 0}}}, /* Treasure Chest Center Clam */
    {14041316u, {{AP_EFFECT_FLAG, 0, 188}, {AP_EFFECT_NONE, 0, 0}}}, /* Treasure Chest Far Right Clam */
    {14041317u, {{AP_EFFECT_FLAG, 0, 189}, {AP_EFFECT_NONE, 0, 0}}}, /* Treasure Chest Close Right Clam */
    {14041318u, {{AP_EFFECT_FLAG, 0, 190}, {AP_EFFECT_NONE, 0, 0}}}, /* Treasure Chest Close Left Clam */
    {14041323u, {{AP_EFFECT_TRAP, 9, 0}, {AP_EFFECT_NONE, 0, 0}}}, /* Get Out Trap */
    {14041326u, {{AP_EFFECT_TRAP, 7, 0}, {AP_EFFECT_NONE, 0, 0}}}, /* Dry Trap */
    {14041329u, {{AP_EFFECT_TRAP, 8, 0}, {AP_EFFECT_NONE, 0, 0}}}, /* Flip Trap */
    {14041342u, {{AP_EFFECT_TRAP, 10, 0}, {AP_EFFECT_NONE, 0, 0}}}, /* Ice Floor Trap */
    {14041346u, {{AP_EFFECT_TRAP, 11, 0}, {AP_EFFECT_NONE, 0, 0}}}, /* Paper Trap */
    {14041350u, {{AP_EFFECT_TRAP, 14, 0}, {AP_EFFECT_NONE, 0, 0}}}, /* Slip Trap */
    {14041353u, {{AP_EFFECT_SPECIAL, 4, 0}, {AP_EFFECT_NONE, 0, 0}}}, /* Forest Second Anthill Reward */
    {14041354u, {{AP_EFFECT_RAINBOW, 0, 0}, {AP_EFFECT_NONE, 0, 0}}}, /* Rainbow Coin */
    {14041358u, {{AP_EFFECT_BLUEPRINT, 0, 0}, {AP_EFFECT_NONE, 0, 0}}}, /* Donkey Blueprint */
    {14041359u, {{AP_EFFECT_BLUEPRINT, 1, 0}, {AP_EFFECT_NONE, 0, 0}}}, /* Diddy Blueprint */
    {14041360u, {{AP_EFFECT_BLUEPRINT, 2, 0}, {AP_EFFECT_NONE, 0, 0}}}, /* Lanky Blueprint */
    {14041361u, {{AP_EFFECT_BLUEPRINT, 3, 0}, {AP_EFFECT_NONE, 0, 0}}}, /* Tiny Blueprint */
    {14041362u, {{AP_EFFECT_BLUEPRINT, 4, 0}, {AP_EFFECT_NONE, 0, 0}}}, /* Chunky Blueprint */
    {14041369u, {{AP_EFFECT_TRAP, 0, 0}, {AP_EFFECT_NONE, 0, 0}}}, /* Animal Trap */
    {14041373u, {{AP_EFFECT_TRAP, 13, 0}, {AP_EFFECT_NONE, 0, 0}}}, /* Rockfall Trap */
    {14041377u, {{AP_EFFECT_TRAP, 6, 0}, {AP_EFFECT_NONE, 0, 0}}}, /* Disable Tag Trap */
    {14041689u, {{AP_EFFECT_FLAG, 0, 940}, {AP_EFFECT_NONE, 0, 0}}}, /* Japes Crate: Behind the Mountain */
    {14041690u, {{AP_EFFECT_FLAG, 0, 941}, {AP_EFFECT_NONE, 0, 0}}}, /* Japes Crate: In the Rambi Cave */
    {14041691u, {{AP_EFFECT_FLAG, 0, 942}, {AP_EFFECT_NONE, 0, 0}}}, /* Aztec Crate: Llama Temple Entrance */
    {14041692u, {{AP_EFFECT_FLAG, 0, 943}, {AP_EFFECT_NONE, 0, 0}}}, /* Factory Crate: Near Funky */
    {14041693u, {{AP_EFFECT_FLAG, 0, 944}, {AP_EFFECT_NONE, 0, 0}}}, /* Factory Crate: Near Candy */
    {14041694u, {{AP_EFFECT_FLAG, 0, 945}, {AP_EFFECT_NONE, 0, 0}}}, /* Galleon Crate: Near Cactus */
    {14041695u, {{AP_EFFECT_FLAG, 0, 946}, {AP_EFFECT_NONE, 0, 0}}}, /* Aztec Crate: On Llama Temple */
    {14041696u, {{AP_EFFECT_FLAG, 0, 947}, {AP_EFFECT_NONE, 0, 0}}}, /* Aztec Crate: Near Gong Tower */
    {14041697u, {{AP_EFFECT_FLAG, 0, 948}, {AP_EFFECT_NONE, 0, 0}}}, /* Forest Crate: Near Owl Tree */
    {14041698u, {{AP_EFFECT_FLAG, 0, 949}, {AP_EFFECT_NONE, 0, 0}}}, /* Forest Crate: Near Thornvine Barn */
    {14041699u, {{AP_EFFECT_FLAG, 0, 950}, {AP_EFFECT_NONE, 0, 0}}}, /* Forest Crate: Behind Dark Attic */
    {14041700u, {{AP_EFFECT_FLAG, 0, 951}, {AP_EFFECT_NONE, 0, 0}}}, /* Forest Crate: In Thornvine Barn */
    {14041701u, {{AP_EFFECT_FLAG, 0, 952}, {AP_EFFECT_NONE, 0, 0}}}, /* Castle Crate: Behind Mausoleum Entrance */
};
#define AP_SHOP_COUNT 120
#define AP_SHOP_SHARED 5
typedef struct { unsigned int id; unsigned char vendor, level, kong; } ap_shop_location_def;
static const ap_shop_location_def ap_shop_locations[AP_SHOP_COUNT] = {
    {14041442u, 0, 7, 5}, /* Isles Cranky Shared */
    {14041443u, 0, 0, 0}, /* Japes Cranky Donkey */
    {14041444u, 0, 0, 1}, /* Japes Cranky Diddy */
    {14041445u, 0, 0, 2}, /* Japes Cranky Lanky */
    {14041446u, 0, 0, 3}, /* Japes Cranky Tiny */
    {14041447u, 0, 0, 4}, /* Japes Cranky Chunky */
    {14041448u, 1, 0, 0}, /* Japes Funky Donkey */
    {14041449u, 1, 0, 1}, /* Japes Funky Diddy */
    {14041450u, 1, 0, 2}, /* Japes Funky Lanky */
    {14041451u, 1, 0, 3}, /* Japes Funky Tiny */
    {14041452u, 1, 0, 4}, /* Japes Funky Chunky */
    {14041453u, 0, 1, 0}, /* Aztec Cranky Donkey */
    {14041454u, 0, 1, 1}, /* Aztec Cranky Diddy */
    {14041455u, 2, 1, 0}, /* Aztec Candy Donkey */
    {14041456u, 2, 1, 1}, /* Aztec Candy Diddy */
    {14041457u, 2, 1, 2}, /* Aztec Candy Lanky */
    {14041458u, 2, 1, 3}, /* Aztec Candy Tiny */
    {14041459u, 2, 1, 4}, /* Aztec Candy Chunky */
    {14041460u, 0, 2, 0}, /* Factory Cranky Donkey */
    {14041461u, 0, 2, 1}, /* Factory Cranky Diddy */
    {14041462u, 0, 2, 2}, /* Factory Cranky Lanky */
    {14041463u, 0, 2, 3}, /* Factory Cranky Tiny */
    {14041464u, 0, 2, 4}, /* Factory Cranky Chunky */
    {14041465u, 1, 2, 5}, /* Factory Funky Shared */
    {14041466u, 2, 3, 5}, /* Galleon Candy Shared */
    {14041467u, 0, 4, 5}, /* Forest Cranky Shared */
    {14041468u, 1, 4, 5}, /* Forest Funky Shared */
    {14041469u, 0, 5, 2}, /* Caves Cranky Lanky */
    {14041470u, 0, 5, 3}, /* Caves Cranky Tiny */
    {14041471u, 0, 5, 4}, /* Caves Cranky Chunky */
    {14041472u, 1, 5, 5}, /* Caves Funky Shared */
    {14041473u, 2, 5, 5}, /* Caves Candy Shared */
    {14041474u, 0, 6, 5}, /* Castle Cranky Shared */
    {14041475u, 1, 6, 5}, /* Castle Funky Shared */
    {14041476u, 2, 6, 5}, /* Castle Candy Shared */
    {14041478u, 0, 0, 5}, /* Japes Cranky Shared */
    {14041479u, 1, 0, 5}, /* Japes Funky Shared */
    {14041480u, 0, 1, 5}, /* Aztec Cranky Shared */
    {14041481u, 0, 1, 2}, /* Aztec Cranky Lanky */
    {14041482u, 0, 1, 3}, /* Aztec Cranky Tiny */
    {14041483u, 0, 1, 4}, /* Aztec Cranky Chunky */
    {14041484u, 1, 1, 5}, /* Aztec Funky Shared */
    {14041485u, 1, 1, 0}, /* Aztec Funky Donkey */
    {14041486u, 1, 1, 1}, /* Aztec Funky Diddy */
    {14041487u, 1, 1, 2}, /* Aztec Funky Lanky */
    {14041488u, 1, 1, 3}, /* Aztec Funky Tiny */
    {14041489u, 1, 1, 4}, /* Aztec Funky Chunky */
    {14041490u, 2, 1, 5}, /* Aztec Candy Shared */
    {14041491u, 0, 2, 5}, /* Factory Cranky Shared */
    {14041492u, 1, 2, 0}, /* Factory Funky Donkey */
    {14041493u, 1, 2, 1}, /* Factory Funky Diddy */
    {14041494u, 1, 2, 2}, /* Factory Funky Lanky */
    {14041495u, 1, 2, 3}, /* Factory Funky Tiny */
    {14041496u, 1, 2, 4}, /* Factory Funky Chunky */
    {14041497u, 2, 2, 5}, /* Factory Candy Shared */
    {14041498u, 2, 2, 0}, /* Factory Candy Donkey */
    {14041499u, 2, 2, 1}, /* Factory Candy Diddy */
    {14041500u, 2, 2, 2}, /* Factory Candy Lanky */
    {14041501u, 2, 2, 3}, /* Factory Candy Tiny */
    {14041502u, 2, 2, 4}, /* Factory Candy Chunky */
    {14041503u, 0, 3, 5}, /* Galleon Cranky Shared */
    {14041504u, 0, 3, 0}, /* Galleon Cranky Donkey */
    {14041505u, 0, 3, 1}, /* Galleon Cranky Diddy */
    {14041506u, 0, 3, 2}, /* Galleon Cranky Lanky */
    {14041507u, 0, 3, 3}, /* Galleon Cranky Tiny */
    {14041508u, 0, 3, 4}, /* Galleon Cranky Chunky */
    {14041509u, 1, 3, 5}, /* Galleon Funky Shared */
    {14041510u, 1, 3, 0}, /* Galleon Funky Donkey */
    {14041511u, 1, 3, 1}, /* Galleon Funky Diddy */
    {14041512u, 1, 3, 2}, /* Galleon Funky Lanky */
    {14041513u, 1, 3, 3}, /* Galleon Funky Tiny */
    {14041514u, 1, 3, 4}, /* Galleon Funky Chunky */
    {14041515u, 2, 3, 0}, /* Galleon Candy Donkey */
    {14041516u, 2, 3, 1}, /* Galleon Candy Diddy */
    {14041517u, 2, 3, 2}, /* Galleon Candy Lanky */
    {14041518u, 2, 3, 3}, /* Galleon Candy Tiny */
    {14041519u, 2, 3, 4}, /* Galleon Candy Chunky */
    {14041520u, 0, 4, 0}, /* Forest Cranky Donkey */
    {14041521u, 0, 4, 1}, /* Forest Cranky Diddy */
    {14041522u, 0, 4, 2}, /* Forest Cranky Lanky */
    {14041523u, 0, 4, 3}, /* Forest Cranky Tiny */
    {14041524u, 0, 4, 4}, /* Forest Cranky Chunky */
    {14041525u, 1, 4, 0}, /* Forest Funky Donkey */
    {14041526u, 1, 4, 1}, /* Forest Funky Diddy */
    {14041527u, 1, 4, 2}, /* Forest Funky Lanky */
    {14041528u, 1, 4, 3}, /* Forest Funky Tiny */
    {14041529u, 1, 4, 4}, /* Forest Funky Chunky */
    {14041530u, 0, 5, 5}, /* Caves Cranky Shared */
    {14041531u, 0, 5, 0}, /* Caves Cranky Donkey */
    {14041532u, 0, 5, 1}, /* Caves Cranky Diddy */
    {14041533u, 1, 5, 0}, /* Caves Funky Donkey */
    {14041534u, 1, 5, 1}, /* Caves Funky Diddy */
    {14041535u, 1, 5, 2}, /* Caves Funky Lanky */
    {14041536u, 1, 5, 3}, /* Caves Funky Tiny */
    {14041537u, 1, 5, 4}, /* Caves Funky Chunky */
    {14041538u, 2, 5, 0}, /* Caves Candy Donkey */
    {14041539u, 2, 5, 1}, /* Caves Candy Diddy */
    {14041540u, 2, 5, 2}, /* Caves Candy Lanky */
    {14041541u, 2, 5, 3}, /* Caves Candy Tiny */
    {14041542u, 2, 5, 4}, /* Caves Candy Chunky */
    {14041543u, 0, 6, 0}, /* Castle Cranky Donkey */
    {14041544u, 0, 6, 1}, /* Castle Cranky Diddy */
    {14041545u, 0, 6, 2}, /* Castle Cranky Lanky */
    {14041546u, 0, 6, 3}, /* Castle Cranky Tiny */
    {14041547u, 0, 6, 4}, /* Castle Cranky Chunky */
    {14041548u, 1, 6, 0}, /* Castle Funky Donkey */
    {14041549u, 1, 6, 1}, /* Castle Funky Diddy */
    {14041550u, 1, 6, 2}, /* Castle Funky Lanky */
    {14041551u, 1, 6, 3}, /* Castle Funky Tiny */
    {14041552u, 1, 6, 4}, /* Castle Funky Chunky */
    {14041553u, 2, 6, 0}, /* Castle Candy Donkey */
    {14041554u, 2, 6, 1}, /* Castle Candy Diddy */
    {14041555u, 2, 6, 2}, /* Castle Candy Lanky */
    {14041556u, 2, 6, 3}, /* Castle Candy Tiny */
    {14041557u, 2, 6, 4}, /* Castle Candy Chunky */
    {14041558u, 0, 7, 0}, /* Isles Cranky Donkey */
    {14041559u, 0, 7, 1}, /* Isles Cranky Diddy */
    {14041560u, 0, 7, 2}, /* Isles Cranky Lanky */
    {14041561u, 0, 7, 3}, /* Isles Cranky Tiny */
    {14041562u, 0, 7, 4}, /* Isles Cranky Chunky */
};
