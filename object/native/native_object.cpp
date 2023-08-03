void nativeObjectRef::n_remove(ServerActiveObject *sao) {
    sao->clearChildAttachments();
    sao->clearParentAttachment();

    verbosestream << "ObjectRef::n_remove(): id=" << sao->getId() << std::endl;
    sao->markForRemoval();
}

v3f nativeObjectRef::n_get_pos(ServerActiveObject *sao) {
    return sao->getBasePosition() / BS;
}

void nativeObjectRef::n_set_pos(ServerActiveObject *sao, const v3f &pos) {
    sao->setPos(pos);
}

void nativeObjectRef::n_move_to(ServerActiveObject *sao, const v3f &pos, bool continuous) {
    sao->moveTo(pos, continuous);
}

float nativeObjectRef::n_punch(ServerActiveObject *sao, ServerActiveObject *puncher, float time_from_last_punch, const ToolCapabilities &toolcap, const v3f &dir) {
    dir.normalize();
    u16 src_original_hp = sao->getHP();
    u16 dst_origin_hp = puncher->getHP();

    u16 wear = sao->punch(dir, &toolcap, puncher, time_from_last_punch);

    // If the punched is a player, and its HP changed
    if (src_original_hp != sao->getHP() && sao->getType() == ACTIVEOBJECT_TYPE_PLAYER) {
        getServer(L)->SendPlayerHPOrDie((PlayerSAO *)sao, PlayerHPChangeReason(PlayerHPChangeReason::PLAYER_PUNCH, puncher));
    }

    // If the puncher is a player, and its HP changed
    if (dst_origin_hp != puncher->getHP() && puncher->getType() == ACTIVEOBJECT_TYPE_PLAYER) {
        getServer(L)->SendPlayerHPOrDie((PlayerSAO *)puncher, PlayerHPChangeReason(PlayerHPChangeReason::PLAYER_PUNCH, sao));
    }

    return wear;
}

//Might not be changable
void nativeObjectRef::n_right_click(ServerActiveObject *sao, ServerActiveObject *sao2) {
    sao->rightClick(sao2);
}

void nativeObjectRef::n_set_hp(ServerActiveObject *sao, int hp, const PlayerHPChangeReason& reason) {
    sao->setHP(hp, reason);
    if (sao->getType() == ACTIVEOBJECT_TYPE_PLAYER)
        getServer(L)->SendPlayerHPOrDie((PlayerSAO *)sao, reason);
    if (reason.hasLuaReference())
        luaL_unref(L, LUA_REGISTRYINDEX, reason.lua_reference);
}

int nativeObjectRef::n_get_hp(ServerActiveObject *sao) {
    if (sao == nullptr) {
        // Default hp is 1
        return 1;
    }

    return sao->getHP();
}

InvRef nativeObjectRef::n_get_inventory(ServerActiveObject *sao)
{
	InventoryLocation loc = sao->getInventoryLocation();
	if (getServerInventoryMgr(sao->getEnv())->getInventory(loc) != nullptr)
		return InvRef::create(sao->getEnv(), loc);
	else
		return InvRef();
}

std::string nativeObjectRef::n_get_wield_list(ServerActiveObject *sao) {
    if (sao == nullptr)
        return "";

    return sao->getWieldList();
}

int nativeObjectRef::n_get_wield_index(ServerActiveObject *sao)
{
    if (sao == nullptr)
        return -1;

    int wieldIndex = sao->getWieldIndex() + 1;
    return wieldIndex;
}

ItemStack nativeObjectRef::n_get_wielded_item(ServerActiveObject *sao) {
    ItemStack selected_item;
    sao->getWieldedItem(&selected_item, nullptr);
    return selected_item;
}

bool nativeObjectRef::n_set_wielded_item(ServerActiveObject *sao, const ItemStack &item)
{
    return sao->setWieldedItem(item);
}

std::pair<bool, ItemGroupList> nativeObjectRef::n_set_armor_groups(ServerActiveObject *sao, const ItemGroupList &groups)
{
    if (sao == nullptr)
        return std::make_pair(false, ItemGroupList());

    if (sao->getType() == ACTIVEOBJECT_TYPE_PLAYER) {
        if (!g_settings->getBool("enable_damage") && !itemgroup_get(groups, "immortal")) {
            groups["immortal"] = 1;
        }
    }

    sao->setArmorGroups(groups);
    return std::make_pair(true, groups);
}

std::map<std::string, int> nativeObjectRef::native_get_armor_groups(ServerActiveObject *sao)
{
	// Assuming ServerActiveObject has a function getArmorGroups() that returns the armor groups.
	// Replace 'getArmorGroups()' with the actual function name if different.
	return sao->getArmorGroups();
}

void ObjectRef::n_set_animation(ServerActiveObject *sao, v2f frame_range, float frame_speed, float frame_blend, bool frame_loop)
{
    sao->setAnimation(frame_range, frame_speed, frame_blend, frame_loop);
}

void nativeObjectRef::n_get_animation(ServerActiveObject *sao, v2f *frames, float *frame_speed, float *frame_blend, bool *frame_loop)
{
	sao->getAnimation(frames, frame_speed, frame_blend, frame_loop);
}

void nativeObjectRef::n_set_local_animation(RemotePlayer *player, v2s32 frames[4], float frame_speed)
{
	getServer(L)->setLocalPlayerAnimations(player, frames, frame_speed);
}

std::pair<std::array<v2s32, 4>, float> nativeObjectRef::n_get_local_animation(RemotePlayer *player)
{
    std::array<v2s32, 4> frames;
    float frame_speed;
    player->getLocalAnimations(frames.data(), &frame_speed);
    return std::make_pair(frames, frame_speed);
}

//Might not be changable
void nativeModApiObjectRef::n_set_eye_offset(RemotePlayer *player, const v3f &offset_first, const v3f &offset_third)
{
	getServer(L)->setPlayerEyeOffset(player, offset_first, offset_third);
}

std::pair<v3f, v3f> nativeObjectRef::n_get_eye_offset(RemotePlayer *player)
{
	return std::make_pair(player->eye_offset_first, player->eye_offset_third);
}

bool nativeObjectRef::n_send_mapblock(ObjectRef *ref, v3s16 pos)
{
	RemotePlayer *player = getplayer(ref);
	if (player == nullptr)
		return false;

	session_t peer_id = player->getPeerId();
	return getServer(ref)->SendBlock(peer_id, pos);
}

void nativeObjectRef::n_set_animation_frame_speed(ServerActiveObject *sao, float frame_speed)
{
    sao->setAnimationSpeed(frame_speed);
}

void nativeObjectRef::n_set_bone_position(ServerActiveObject *sao, const std::string &bone, const v3f &position, const v3f &rotation)
{
    sao->setBonePosition(bone, position, rotation);
}

void nativeObjectRef::n_get_bone_position(ServerActiveObject *sao, const std::string &bone, v3f *position, v3f *rotation)
{
	sao->getBonePosition(bone, position, rotation);
}

//double check this one later
std::tuple<bool, std::string, v3f, v3f, bool> nativeObjectRef::n_set_attach(ServerActiveObject *sao, ServerActiveObject *parent, const std::string &bone, const v3f &position, const v3f &rotation, bool force_visible)
{
	if (sao == nullptr || parent == nullptr)
		return std::make_tuple(false, "", v3f(0, 0, 0), v3f(0, 0, 0), false);
	if (sao == parent)
		throw LuaError("ObjectRef::set_attach: attaching object to itself is not allowed.");

	int parent_id;
	std::string current_bone;
	v3f current_position;
	v3f current_rotation;
	bool current_force_visible;

	sao->getAttachment(&parent_id, &current_bone, &current_position, &current_rotation, &current_force_visible);
	if (parent_id) {
		ServerActiveObject *old_parent = sao->getEnv()->getActiveObject(parent_id);
		old_parent->removeAttachmentChild(sao->getId());
	}

	sao->setAttachment(parent->getId(), bone, position, rotation, force_visible);
	parent->addAttachmentChild(sao->getId());

	return std::make_tuple(true, current_bone, current_position, current_rotation, current_force_visible);
}

int nativeObjectRef::n_get_attach(ServerActiveObject *sao, int *parent_id, std::string *bone, v3f *position, v3f *rotation, bool *force_visible)
{
    sao->getAttachment(parent_id, bone, position, rotation, force_visible);
    return (*parent_id != 0) ? 1 : 0;
}

std::unordered_set<int> nativeObjectRef::n_get_children(ServerActiveObject* sao)
{
    if (sao == nullptr)
        return std::unordered_set<int>();

    return sao->getAttachmentChildIds();
}

void nativeModApiObject::n_set_detach(ServerActiveObject *sao)
{
    sao->clearParentAttachment();
}

void nativeObjectRef::n_set_properties(ServerActiveObject *sao, const ObjectProperties &prop)
{
    // No map lock required for this function, as it should be called from other C++ functions
    if (sao == nullptr)
        return;

    ObjectProperties *objProp = sao->accessObjectProperties();
    if (objProp == nullptr)
        return;

    *objProp = prop;
    sao->notifyObjectPropertiesModified();
}

std::pair<bool, ObjectProperties*> nativeObjectRef::n_get_properties(ServerActiveObject *sao)
{
	if (sao == nullptr)
		return std::make_pair(false, nullptr);

	ObjectProperties *prop = sao->accessObjectProperties();
	if (prop == nullptr)
		return std::make_pair(false, nullptr);

	return std::make_pair(true, prop);
}

bool nativeObjectRef::n_is_player(ObjectRef *ref)
{
    return (getplayer(ref) != nullptr);
}

bool nativeObjectRef::n_set_nametag_attributes(ServerActiveObject *sao, video::SColor nametag_color, video::SColor nametag_bgcolor, const std::string &nametag)
{
	if (!sao)
		return false;

	ObjectProperties *prop = sao->accessObjectProperties();
	if (!prop)
		return false;

	prop->nametag_color = nametag_color;
	prop->nametag_bgcolor = (nametag_bgcolor.getAlpha() == 0) ? nullopt : nametag_bgcolor;
	prop->nametag = nametag;

	return true;
}

std::tuple<bool, Color, bool, Color, std::string> nativeObjectRef::n_get_nametag_attributes(ServerActiveObject *sao)
{
	NO_MAP_LOCK_REQUIRED;
	if (sao == nullptr)
		return std::make_tuple(false, Color(255, 255, 255, 255), false, Color(0, 0, 0, 0), "");

	ObjectProperties *prop = sao->accessObjectProperties();
	if (!prop)
		return std::make_tuple(false, Color(255, 255, 255, 255), false, Color(0, 0, 0, 0), "");

	Color color = prop->nametag_color;

	bool hasBgColor = (prop->nametag_bgcolor != boost::none);
	Color bgcolor = hasBgColor ? prop->nametag_bgcolor.get() : Color(0, 0, 0, 0);

	std::string text = prop->nametag;
	return std::make_tuple(true, color, hasBgColor, bgcolor, text);
}

void nativeObjectRef::n_set_velocity(ObjectRef *ref, v3f vel)
{
    LuaEntitySAO *sao = getluaobject(ref);
    if (sao == nullptr)
        return;

    sao->setVelocity(vel);
}


// l_native_add_vel:
void nativeObjectRef::n_add_velocity_lua_entity(LuaEntitySAO *entitysao, v3f vel)
{
    entitysao->addVelocity(vel);
}

void nativeObjectRef::n_add_velocity_player_sao(PlayerSAO *playersao, v3f vel)
{
    playersao->setMaxSpeedOverride(vel);
    getServer(playersao->getLua())->SendPlayerSpeed(playersao->getPeerID(), vel);
}
///////////////////////


//Double check this one
v3f nativeObjectRef::n_get_velocity(ServerActiveObject *sao)
{
    if (sao == nullptr)
        return v3f(0, 0, 0);

    if (sao->getType() == ACTIVEOBJECT_TYPE_LUAENTITY)
    {
        LuaEntitySAO *entitysao = dynamic_cast<LuaEntitySAO*>(sao);
        return entitysao->getVelocity();
    }
    else if (sao->getType() == ACTIVEOBJECT_TYPE_PLAYER)
    {
        RemotePlayer *player = dynamic_cast<PlayerSAO*>(sao)->getPlayer();
        return player->getSpeed() / BS;
    }

    return v3f(0, 0, 0);
}

void nativeObjectRef::n_set_acceleration(ObjectRef *ref, v3f acceleration)
{
    LuaEntitySAO *entitysao = getluaobject(ref);
    if (entitysao == nullptr)
        return;

    entitysao->setAcceleration(acceleration);
}

v3f nativeObjectRef::n_get_acceleration(LuaEntitySAO *entitysao)
{
    if (entitysao == nullptr)
        return v3f(0, 0, 0);

    return entitysao->getAcceleration();
}

void nativeObjectRef::n_set_rotation(ObjectRef *ref, v3f rotationDegrees)
{
    LuaEntitySAO *entitysao = getluaobject(ref);
    if (entitysao == nullptr)
        return;

    v3f rotationRadians = rotationDegrees * core::DEGTORAD;

    entitysao->setRotation(rotationRadians);
}

