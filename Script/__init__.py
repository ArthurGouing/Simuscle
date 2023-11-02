bl_info = {
    "name": "Simuscle",
    "author": "Arthur Gouinguenet",
    "category": "3D View",
    "description": "The Blender Addon to create muscuol-skeleton compatible with Simuscle",
    "version": (1, 0),
    "blender": (3, 6, 0),
    "location": "View3D > Add > Mesh > New Object",
    "description": "Adds a Muscle Object",
    "warning": "",
    "doc_url": "",
    "category": "Add Mesh",
 }
if __name__=="__main__":
    import sys
    dir = r"C:\Users\picol\Documents\Simuscle\Script"
    print(dir)
    print(sys.path)
    if not dir in sys.path:
        sys.path.append(dir )
        print(dir)
    import Addon_Add_muscle_object, Menu_Simuscle, View3D_Panel
else:
    # import all classes
    if "bpy" in locals():
        import importlib
        importlib.reload(Addon_Add_muscle_object)
        importlib.reload(Menu_Simuscle)
        importlib.reload(View3D_Panel)
        print("Reload files")
    else:
        from . import Addon_Add_muscle_object
        from . import Menu_Simuscle
        from . import View3D_Panel
        print("Import multifiles")

import bpy
from bpy.types import Menu

class VIEW3D_MT_editor_menus(Menu):
    bl_label = ""

    def draw(self, context):
        layout = self.layout
        obj = context.active_object
        mode_string = context.mode
        edit_object = context.edit_object
        gp_edit = obj and obj.mode in {'EDIT_GPENCIL', 'PAINT_GPENCIL', 'SCULPT_GPENCIL',
                                       'WEIGHT_GPENCIL', 'VERTEX_GPENCIL'}
        tool_settings = context.scene.tool_settings

        layout.menu("VIEW3D_MT_view")

        # Select Menu
        if gp_edit:
            if mode_string not in {'PAINT_GPENCIL', 'WEIGHT_GPENCIL'}:
                if (
                        mode_string == 'SCULPT_GPENCIL' and
                        (tool_settings.use_gpencil_select_mask_point or
                         tool_settings.use_gpencil_select_mask_stroke or
                         tool_settings.use_gpencil_select_mask_segment)
                ):
                    layout.menu("VIEW3D_MT_select_gpencil")
                elif mode_string == 'EDIT_GPENCIL':
                    layout.menu("VIEW3D_MT_select_gpencil")
                elif mode_string == 'VERTEX_GPENCIL':
                    layout.menu("VIEW3D_MT_select_gpencil")
        elif mode_string in {'PAINT_WEIGHT', 'PAINT_VERTEX', 'PAINT_TEXTURE'}:
            mesh = obj.data
            if mesh.use_paint_mask:
                layout.menu("VIEW3D_MT_select_paint_mask")
            elif mesh.use_paint_mask_vertex and mode_string in {'PAINT_WEIGHT', 'PAINT_VERTEX'}:
                layout.menu("VIEW3D_MT_select_paint_mask_vertex")
        elif mode_string not in {'SCULPT', 'SCULPT_CURVES'}:
            layout.menu("VIEW3D_MT_select_%s" % mode_string.lower())

        if gp_edit:
            pass
        elif mode_string == 'OBJECT':
            layout.menu("VIEW3D_MT_add", text="Add", text_ctxt=i18n_contexts.operator_default)
        elif mode_string == 'EDIT_MESH':
            layout.menu("VIEW3D_MT_mesh_add", text="Add", text_ctxt=i18n_contexts.operator_default)
        elif mode_string == 'EDIT_CURVE':
            layout.menu("VIEW3D_MT_curve_add", text="Add", text_ctxt=i18n_contexts.operator_default)
        elif mode_string == 'EDIT_SURFACE':
            layout.menu("VIEW3D_MT_surface_add", text="Add", text_ctxt=i18n_contexts.operator_default)
        elif mode_string == 'EDIT_METABALL':
            layout.menu("VIEW3D_MT_metaball_add", text="Add", text_ctxt=i18n_contexts.operator_default)
        elif mode_string == 'EDIT_ARMATURE':
            layout.menu("TOPBAR_MT_edit_armature_add", text="Add", text_ctxt=i18n_contexts.operator_default)

        if gp_edit:
            if obj and obj.mode == 'PAINT_GPENCIL':
                layout.menu("VIEW3D_MT_draw_gpencil")
            elif obj and obj.mode == 'EDIT_GPENCIL':
                layout.menu("VIEW3D_MT_edit_gpencil")
                layout.menu("VIEW3D_MT_edit_gpencil_stroke")
                layout.menu("VIEW3D_MT_edit_gpencil_point")
            elif obj and obj.mode == 'WEIGHT_GPENCIL':
                layout.menu("VIEW3D_MT_weight_gpencil")
            if obj and obj.mode == 'VERTEX_GPENCIL':
                layout.menu("VIEW3D_MT_paint_gpencil")

        elif edit_object:
            layout.menu("VIEW3D_MT_edit_%s" % edit_object.type.lower())

            if mode_string == 'EDIT_MESH':
                layout.menu("VIEW3D_MT_edit_mesh_vertices")
                layout.menu("VIEW3D_MT_edit_mesh_edges")
                layout.menu("VIEW3D_MT_edit_mesh_faces")
                layout.menu("VIEW3D_MT_uv_map", text="UV")
            elif mode_string in {'EDIT_CURVE', 'EDIT_SURFACE'}:
                layout.menu("VIEW3D_MT_edit_curve_ctrlpoints")
                layout.menu("VIEW3D_MT_edit_curve_segments")

        elif obj:
            if mode_string not in {'PAINT_TEXTURE', 'SCULPT_CURVES'}:
                layout.menu("VIEW3D_MT_%s" % mode_string.lower())
            if mode_string == 'SCULPT':
                layout.menu("VIEW3D_MT_mask")
                layout.menu("VIEW3D_MT_face_sets")
            if mode_string == 'SCULPT_CURVES':
                layout.menu("VIEW3D_MT_select_sculpt_curves")
                layout.menu("VIEW3D_MT_sculpt_curves")

        else:
            layout.menu("VIEW3D_MT_object")

# classes = [OBJECT_OT_add_object, VIEW3D_MT_simuscle, VIEW3D_MT_editor_menus]
addon_keymaps = list()

def register():
  print ("Registering ", __name__)
  Addon_Add_muscle_object.register()
  Menu_Simuscle.register()
  View3D_Panel.register()
  

  # Set the addon keymap: Open Simuscle menu
  wm = bpy.context.window_manager
  km = wm.keyconfigs.addon.keymaps.new(name='Object Mode', space_type='EMPTY')

  kmi = km.keymap_items.new('wm.call_menu', 'D', 'PRESS', ctrl=True, shift=False)
  kmi.properties.name = 'VIEW3D_MT_simuscle'

  addon_keymaps.append((km, kmi))

def unregister():
  print ("Unregistering ", __name__)
  # reset keymap
  for km, kmi in addon_keymaps:
      km.keymap_items.remove(kmi)
  addon_keymaps.clear()

  Addon_Add_muscle_object.unregister()
  Menu_Simuscle.unregister()
  View3D_Panel.unregister()

  # reset the original menu (peut être metre dans le Menu_Simuscle)
  bpy.utils.register_class(VIEW3D_MT_editor_menus)

if __name__ == "__main__":
    register()