#include <unordered_map>
#include <vector>

struct MOB_INDEX_DATA;
struct OBJ_INDEX_DATA;
struct ROOM_INDEX_DATA;

extern std::unordered_map<int, MOB_INDEX_DATA*> g_mobIndex;
extern std::unordered_map<int, OBJ_INDEX_DATA*> g_objectIndex;
extern std::unordered_map<int, ROOM_INDEX_DATA*> g_roomIndex;
extern std::vector<ROOM_INDEX_DATA*> g_vrooms;