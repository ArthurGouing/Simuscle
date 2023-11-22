import bpy
from mathutils import Vector, Matrix
from bpy.types import Menu


def main(context):
    for ob in context.scene.objects:
        print(ob)
        return


class FrontTargetOperator(bpy.types.Operator):
    """
    How to use:
        Select the vertex you want to pin to the front target of the muscle.
        You will have to parent the empty to the bone where the muscle is attached.
    What happen:
        An empty is create at the middle of the vertex. Create a vertex group from it
        Then the center of the muscle is place at the front_target_empty. 
        And add the copy location constraint.
    TODO:
        add security, like check if other constraint exist, if other modifier that could brake the code are 
        on the object. Check if the front_target already exist, the vertex groupe already exist and clean it
        before appmying the operator.
    """
    bl_idname = "muscle.add_front_target"
    bl_label = "Add Front Target to Muscle"

    @classmethod
    def poll(cls, context):
        return context.active_object is not None

    def execute(self, context):
        print("Create front target")
        # Select Vertices
        muscle = bpy.context.object
        bpy.ops.object.mode_set(mode='OBJECT')
        s_vert = [v for v in bpy.context.object.data.vertices if v.select]
        # Create vertex groupe
        ft_vert_groupe = bpy.context.object.vertex_groups.new(name="Front_Target")
        ft_vert_groupe.add([v.index for v in  s_vert], 1.0, 'ADD')
        print("The vertex group is assigned")
        # Find the middle of selected point
        ft_pos = Vector((0, 0, 0))
        ft_pos_loc = Vector((0, 0, 0))
        for v in s_vert:
            ft_pos += muscle.matrix_world @ v.co
            ft_pos_loc += v.co
        print("ft_pos tot sum :", ft_pos)
        ft_pos = ft_pos/len(s_vert)
        ft_pos_loc = ft_pos_loc/len(s_vert)
        print("Len s_vert :", len(s_vert))
        print("ft_pos_loc :", ft_pos_loc)
        print("The target will be located at :", ft_pos)
        # Change the object origin
        muscle.data.transform(Matrix.Translation(-ft_pos_loc))
        # muscle.location += ft_pos #Not needed as I make a copy location later
        bpy.context.view_layer.update() #Update to compute correct mesh translation
        # Create the empty  
        start_target = bpy.data.objects.new("start_target_"+muscle.name, None)
        start_target.location = ft_pos
        start_target.hide_set(True)
        bpy.context.collection.objects.link(start_target)
        print("Empty created")
        # Create constraint
        ft_constraint = muscle.constraints.new('COPY_LOCATION')
        ft_constraint.target = start_target
        print("Constraint added")
        return {'FINISHED'}

class EndTargetOperator(bpy.types.Operator):
    """
    How to use:
        Select the vertex you want to pin to the end target of the muscle.
        You will have to parent the empty to the bone where the muscle is attached.
    What happen:
        An empty is create at the middle of the vertex. Create a vertex group from it,
        Then add the "Stretch to" constraint.
        That would be greate (in fact obligatory) to align the local Y axis to the 
        front_target/end_target axis. (doable with mesh.transform)
    """
    bl_idname = "muscle.add_end_target"
    bl_label = "Add End Target to Muscle"

    @classmethod
    def poll(cls, context):
        return context.active_object is not None

    def execute(self, context):
        # Select Vertices
        muscle = bpy.context.object
        bpy.ops.object.mode_set(mode='OBJECT')
        s_vert = [v for v in bpy.context.object.data.vertices if v.select]
        # Create vertex groupe
        ft_vert_groupe = bpy.context.object.vertex_groups.new(name="End_Target")
        ft_vert_groupe.add([v.index for v in  s_vert], 1.0, 'ADD')
        print("The vertex group is assigned")
        # Find the middle of selected point
        ft_pos = Vector((0, 0, 0))
        for v in s_vert:
            ft_pos += muscle.matrix_world @ v.co
        ft_pos = ft_pos/len(s_vert)
        print("The target will be located at :", ft_pos)
        # Create the empty  
        end_target = bpy.data.objects.new("end_target_"+muscle.name, None)
        end_target.location = ft_pos
        end_target.hide_set(True)
        bpy.context.collection.objects.link(end_target)
        print("Empty created")
        # Align Y object axis with targets
        start_target = bpy.data.objects.get("start_target_"+muscle.name)
        target_vec = end_target.location - start_target.location
        print("target vec :", target_vec)
        y_axis = muscle.matrix_world.to_quaternion() @ Vector((0, 1, 0))
        q_rot = y_axis.rotation_difference(target_vec)
        print("y_axis :", y_axis," and q_rot :", q_rot)
        muscle.rotation_mode = 'QUATERNION'
        muscle.rotation_quaternion = muscle.rotation_quaternion.cross(q_rot)
        muscle.data.transform(q_rot.conjugated().to_matrix().to_4x4())
        # Create constraint
        et_constraint = muscle.constraints.new('STRETCH_TO')
        et_constraint.target = end_target
        print("constraint created")
        return {'FINISHED'}
    

class VIEW3D_MT_muscle(Menu):
    bl_label = "Muscle_Op"
    bl_idname= "VIEW3D_MT_muscle"
    
    def draw(self, _context):
        layout = self.layout
        
        layout.operator("muscle.add_front_target")
        layout.operator("muscle.add_end_target")

def menu_func(self, context):
    self.layout.operator(FrontTargetOperator.bl_idname, text=FrontTargetOperator.bl_label)

addon_keymaps = list()

# Register and add to the "object" menu (required to also use F3 search "Simple Object Operator" for quick access).
def register():
    bpy.utils.register_class(FrontTargetOperator)
    bpy.utils.register_class(EndTargetOperator)
    bpy.utils.register_class(VIEW3D_MT_muscle)
    bpy.types.VIEW3D_MT_object.append(menu_func)
    
    # Set the addon keymap: Open Simuscle menu
    wm = bpy.context.window_manager
    km = wm.keyconfigs.addon.keymaps.new(name='3D View', space_type='VIEW_3D')

    kmi = km.keymap_items.new('wm.call_menu', 'D', 'PRESS', ctrl=True, shift=False)
    kmi.properties.name = 'VIEW3D_MT_muscle'

    addon_keymaps.append((km, kmi))


def unregister():
    # reset keymap
    for km, kmi in addon_keymaps:
        km.keymap_items.remove(kmi)
    addon_keymaps.clear()
    
    bpy.utils.unregister_class(FrontTargetOperator)
    bpy.utils.unregister_class(EndTargetOperator)
    bpy.types.VIEW3D_MT_object.remove(menu_func)


if __name__ == "__main__":
    register()

