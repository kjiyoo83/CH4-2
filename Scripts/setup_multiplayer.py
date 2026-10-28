"""언리얼 엔진 5.6에서 -run=pythonscript -script=<이 파일>로 실행함. 반복 실행해도 에셋 설정이 유지됨"""
import unreal

assets = unreal.EditorAssetLibrary
tools = unreal.AssetToolsHelpers.get_asset_tools()
root = '/Game/CXMultiplayer'
assets.make_directory(root)

def blueprint(name, parent):
    path = root + '/' + name
    bp = assets.load_asset(path) if assets.does_asset_exist(path) else None
    if not bp:
        factory = unreal.BlueprintFactory()
        factory.set_editor_property('parent_class', parent)
        bp = tools.create_asset(name, root, unreal.Blueprint, factory)
    return bp

# 빈 위젯 트리로 네이티브 채팅과 상태 표시 레이아웃을 상속함
widget_path = root + '/WBP_Chat'
widget = assets.load_asset(widget_path) if assets.does_asset_exist(widget_path) else None
if not widget:
    factory = unreal.WidgetBlueprintFactory()
    factory.set_editor_property('parent_class', unreal.ChatUI)
    widget = tools.create_asset('WBP_Chat', root, unreal.WidgetBlueprint, factory)
unreal.BlueprintEditorLibrary.compile_blueprint(widget)
widget_class = assets.load_blueprint_class(widget_path)

attack_paths = [root + '/Input/IA_Attack', root + '/IA_Attack']
attack = next((assets.load_asset(path) for path in attack_paths if assets.does_asset_exist(path)), None)
if not attack:
    attack = assets.duplicate_asset('/Game/VehicleTemplate/Input/Actions/IA_ToggleCamera', root + '/IA_Attack')
attack.set_editor_property('value_type', unreal.InputActionValueType.BOOLEAN)

context = assets.load_asset('/Game/VehicleTemplate/Input/IMC_Default')
if context:
    context.modify()
    context.unmap_all_keys_from_action(attack)
    attack_key = unreal.Key()
    attack_key.import_text('LeftMouseButton')
    assert attack_key.export_text() == 'LeftMouseButton'
    context.map_key(attack, attack_key)
    assets.save_loaded_asset(context, only_if_is_dirty=False)

for path in ['/Game/VehicleTemplate/Blueprints/SportsCar/BP_SportsCar_Pawn', '/Game/VehicleTemplate/Blueprints/OffroadCar/BP_OffroadCar_Pawn']:
    cls = assets.load_blueprint_class(path)
    if cls:
        unreal.get_default_object(cls).set_editor_property('attack_action', attack)
        assets.save_asset(path)

pc_path = '/Game/VehicleTemplate/Blueprints/BP_VehicleAdvPlayerController'
pc_cls = assets.load_blueprint_class(pc_path)
if pc_cls:
    unreal.get_default_object(pc_cls).set_editor_property('chat_ui_class', widget_class)
    assets.save_asset(pc_path)

for path in ['/Game/VehicleTemplate/Blueprints/BP_VehicleAdvGameMode', '/Game/VehicleTemplate/Blueprints/BP_VehicleAdvOffroadGameMode']:
    cls = assets.load_blueprint_class(path)
    if cls:
        defaults = unreal.get_default_object(cls)
        defaults.set_editor_property('player_state_class', unreal.CXPlayerState)
        defaults.set_editor_property('game_state_class', unreal.CXGameState)
        assets.save_asset(path)

coin = blueprint('BP_Coin', unreal.CXCoin)
buff = blueprint('BP_SpeedBuff', unreal.CXSpeedBuff)
spawner = blueprint('BP_ItemSpawner', unreal.CXItemSpawner)
for bp in [coin, buff, spawner]:
    unreal.BlueprintEditorLibrary.compile_blueprint(bp)
    assets.save_loaded_asset(bp)
coin_cls = assets.load_blueprint_class(root + '/BP_Coin')
buff_cls = assets.load_blueprint_class(root + '/BP_SpeedBuff')
spawner_cls = assets.load_blueprint_class(root + '/BP_ItemSpawner')

level = unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
actors = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
level.load_level('/Game/VehicleTemplate/Maps/VehicleAdvExampleMap')
all_actors = actors.get_all_level_actors()
existing = {actor.get_actor_label() for actor in all_actors}
starts = [actor for actor in all_actors if isinstance(actor, unreal.PlayerStart)]
if not starts:
    raise RuntimeError('No PlayerStart found: manual spawner placement required')
for i, start in enumerate(starts[:4]):
    origin = start.get_actor_location()
    forward = start.get_actor_forward_vector()
    right = start.get_actor_right_vector()
    for j, offset in enumerate([500., 1000., 1500.]):
        label = 'CX_ItemSpawner_%d_%d' % (i,j)
        if label in existing:
            continue
        location = origin + forward * offset + right * (250. if j % 2 else -250.)
        location.z = origin.z - 20.
        actor = actors.spawn_actor_from_class(spawner_cls, location)
        actor.set_actor_label(label)
        actor.set_editor_property('item_class', buff_cls if j == 2 else coin_cls)
        actor.set_editor_property('respawn_delay', 2.)
        unreal.log('CX_SETUP placed %s at %s' % (label, location))
level.save_current_level()
assets.save_directory(root, only_if_is_dirty=False, recursive=True)
unreal.log('CX_SETUP COMPLETE: chat BP, attack input, gameplay defaults, pickup BPs and level spawners')
