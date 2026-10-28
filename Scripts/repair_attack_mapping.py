"""누락된 공격 키 매핑만 복구함. 기존 조작과 에셋은 유지함"""
import unreal

assets = unreal.EditorAssetLibrary
paths = ['/Game/CXMultiplayer/Input/IA_Attack', '/Game/CXMultiplayer/IA_Attack']
action = next((assets.load_asset(path) for path in paths if assets.does_asset_exist(path)), None)
if not action:
    raise RuntimeError('Existing IA_Attack could not be found')
context = assets.load_asset('/Game/VehicleTemplate/Input/IMC_Default')
if not context:
    raise RuntimeError('IMC_Default could not be found')
context.modify()
key = unreal.Key()
key.import_text('LeftMouseButton')
assert key.export_text() == 'LeftMouseButton'
for mapping in list(context.get_editor_property('mappings')):
    if mapping.get_editor_property('action') == action and mapping.get_editor_property('key').export_text() == '(':
        context.unmap_key(action, mapping.get_editor_property('key'))
already_mapped = any(m.get_editor_property('action') == action and m.get_editor_property('key').export_text() == 'LeftMouseButton' for m in context.get_editor_property('mappings'))
if not already_mapped:
    context.map_key(action, key)
if not assets.save_loaded_asset(context, only_if_is_dirty=False):
    raise RuntimeError('Failed to save IMC_Default')
assert any(m.get_editor_property('action') == action and m.get_editor_property('key').export_text() == 'LeftMouseButton' for m in context.get_editor_property('mappings'))
unreal.log('CX_ATTACK_REPAIR: saved LeftMouseButton -> %s in %s' % (action.get_path_name(), context.get_path_name()))
