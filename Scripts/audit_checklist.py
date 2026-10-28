"""언리얼 에셋을 읽기 전용으로 점검함. 결과는 Saved/Audit에만 저장함"""
import unreal
import json
from pathlib import Path

assets = unreal.EditorAssetLibrary
report = {'assets': {}, 'classes': {}, 'spawners': []}

def describe(value):
    if isinstance(value, unreal.Object):
        return value.get_path_name()
    if isinstance(value, (int, float, bool, str)) or value is None:
        return value
    return str(value)

for path in assets.list_assets('/Game/CXMultiplayer', recursive=True, include_folder=False):
    asset = assets.load_asset(path)
    report['assets'][path] = asset.get_class().get_name() if asset else None

paths = {
    'controller': '/Game/VehicleTemplate/Blueprints/BP_VehicleAdvPlayerController',
    'sports': '/Game/VehicleTemplate/Blueprints/SportsCar/BP_SportsCar_Pawn',
    'offroad': '/Game/VehicleTemplate/Blueprints/OffroadCar/BP_OffroadCar_Pawn',
    'mode': '/Game/VehicleTemplate/Blueprints/BP_VehicleAdvGameMode',
    'offroad_mode': '/Game/VehicleTemplate/Blueprints/BP_VehicleAdvOffroadGameMode',
    'coin': '/Game/CXMultiplayer/Blueprints/BP_Coin',
    'buff': '/Game/CXMultiplayer/Blueprints/BP_SpeedBuff',
    'spawner': '/Game/CXMultiplayer/Blueprints/BP_ItemSpawner',
    'chat': '/Game/CXMultiplayer/UI/WBP_Chat',
}
properties = ['chat_ui_class', 'input_mapping_context', 'attack_action', 'attack_range', 'attack_cooldown', 'hit_effect', 'player_state_class', 'game_state_class', 'player_controller_class', 'default_pawn_class', 'waiting_duration', 'playing_duration', 'replicates', 'net_update_frequency', 'speed_buff', 'pickup_effect', 'pickup_sound', 'item_class', 'respawn_delay']
for name, path in paths.items():
    cls = assets.load_blueprint_class(path)
    if not cls:
        report['classes'][name] = {'error': 'missing class', 'path': path}
        continue
    obj = unreal.get_default_object(cls)
    entry = {'path': cls.get_path_name()}
    for prop in properties:
        try:
            entry[prop] = describe(obj.get_editor_property(prop))
        except Exception:
            pass
    report['classes'][name] = entry
    if name in ['sports', 'offroad']:
        origin = obj.get_editor_property('attack_origin')
        entry['attack_origin_location'] = str(origin.get_editor_property('relative_location'))

context = assets.load_asset('/Game/VehicleTemplate/Input/IMC_Default')
report['attack_mappings'] = [mapping.export_text() for mapping in context.get_editor_property('mappings') if mapping.get_editor_property('action') and mapping.get_editor_property('action').get_name() == 'IA_Attack']

levels = unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
actors = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
levels.load_level('/Game/VehicleTemplate/Maps/VehicleAdvExampleMap')
report['map_game_mode'] = describe(unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem).get_editor_world().get_world_settings().get_editor_property('default_game_mode'))
for actor in actors.get_all_level_actors():
    if isinstance(actor, unreal.CXItemSpawner):
        report['spawners'].append({'label': actor.get_actor_label(), 'item_class': describe(actor.get_editor_property('item_class')), 'delay': actor.get_editor_property('respawn_delay'), 'location': str(actor.get_actor_location())})
output = Path(unreal.Paths.project_saved_dir()) / 'Audit' / 'checklist_assets.json'
output.parent.mkdir(parents=True, exist_ok=True)
output.write_text(json.dumps(report, ensure_ascii=False, indent=2), encoding='utf-8')
unreal.log('CX_AUDIT: %d assets, %d spawners, %d attack mappings. Evidence: %s' % (len(report['assets']), len(report['spawners']), len(report['attack_mappings']), output))
