import bpy
from bpy.types import Menu

# I needed to keep this import from the source code too make it work, but I don't now why
from bpy.app.translations import contexts as i18n_contexts

# Create new Simuscle menu
class VIEW3D_MT_simuscle(Menu):
    bl_label = "Simuscle"
    bl_idname= "VIEW3D_MT_simuscle"
    
    def draw(self, _context):
        layout = self.layout
        
        layout.label(text="Bones")
        layout.label(text="TODO")
        layout.separator()
        layout.label(text="Muscles")
        layout.operator("simuscle.add_muscle")
        layout.operator("simuscle.apply_muscle")
        layout.operator("simuscle.delete_muscle")
        layout.operator("muscle.add_front_target")
        layout.operator("muscle.add_end_target")

# Recreate the whole menu bar
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
            
        # Add your Custom menu
        layout.menu("VIEW3D_MT_simuscle")

def register():
  bpy.utils.register_class(VIEW3D_MT_simuscle)
  bpy.utils.register_class(VIEW3D_MT_editor_menus)

def unregister():
  bpy.utils.unregister_class(VIEW3D_MT_simuscle)
  bpy.utils.unregister_class(VIEW3D_MT_editor_menus)



if __name__ == "__main__":  # only for live edit.
    register()
