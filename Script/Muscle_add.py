import bpy 
from mathutils import Vector
from math import atan

# Some conveniante global variable
C = bpy.context
pi = 4*atan(1)

def create_taper(collection):
    """
    Create a curve which will be used for the taper of the muscle object
    """
    # create the Curve Datablock
    # default_bezier_curve = bpy.ops.curve.primitive_bezier_curve_add(enter_editmode=False, align='WORLD', location=(0, 0, 0), scale=(muscle_lenth, 1, 1))
    crv = bpy.data.curves.new("taper_curve", 'CURVE')
    # crv.dimensions = '2D' # useless for 2D
    spline = crv.splines.new('BEZIER')
    spline.bezier_points.add(2)

    # map coords to spline
    p1 = spline.bezier_points[2]
    p1.co =           (1, 0, 0)
    p1.handle_left =  (0.85, 0, 0)
    p1.handle_right = (1.15, 0, 0)
    p2 = spline.bezier_points[1]
    p2.co =           (0.5,  0.7, 0)
    p2.handle_left =  (0.35, 0.7, 0)
    p2.handle_right = (0.65, 0.7, 0)
    p3 = spline.bezier_points[0]
    p3.co =           (0, 0, 0)
    p3.handle_left =  (-0.15, 0, 0)
    p3.handle_right = (0.15, 0, 0)
    
    # create Object
    taper = bpy.data.objects.new(collection.name+'_taper', crv)

    collection.objects.link(taper)
    return taper

def create_middle_empty(start_object, end_object, name):
    middle = bpy.data.objects.new(name, None)
    # Set constraint for the middle target
    const_start_loc = middle.constraints.new(type='COPY_LOCATION')
    const_start_loc.target = start_object
    const_start_loc.use_offset = True
    const_end_loc = middle.constraints.new(type='COPY_LOCATION')
    const_end_loc.target     = end_object
    const_end_loc.influence  = 0.5
    const_start_rot = middle.constraints.new(type='TRACK_TO')
    const_start_rot.target     = start_object
    const_start_rot.up_axis    = 'UP_Z'
    const_start_rot.track_axis = 'TRACK_Y'
    const_end_rot = middle.constraints.new(type='TRACK_TO')
    const_end_rot.target     = end_object
    const_end_rot.up_axis    = 'UP_Z'
    const_end_rot.track_axis = 'TRACK_NEGATIVE_Y'
    const_end_rot.influence  = 0.5
    return middle



def create_muscle(name):
    """
    Create a Muscle object.
    A muscle has:
        - to extremity, attach to 2 bones
    of the muscle joint movement.
        - A active line which is the median line
    of the muscle geometry.
        - A geometry which move with the active
    line deformation.
    """
    # Create muscle collection
    print("Create the muscle: "+name)
    collection = bpy.data.collections.new(name)
    collection.color_tag = 'COLOR_01'
    bpy.context.scene.collection.children.link(collection)
    # Select the bones associated of the muscle if possible:
    # if a joint is selected
    if C.selected_objects:
        if C.selected_objects[0].type == 'ARMATURE' and C.active_object.mode == 'EDIT':
            armature = C.selected_objects[0]
            print("armature selected: ", armature)
            print("select the bone")
            if len(C.selected_bones) < 2:
                bpy.ops.armature.select_more()
            else:
                print("Warning: Too musch bones selected. Only the joint have to be selected")
            start_bone = C.selected_bones[0]
            end_bone   = C.selected_bones[1]
            print("The bones of the muscle are: ")
            print(start_bone.name)
            print(end_bone.name)
            collection.armature   = armature.data.name
            collection.start_bone = start_bone.name
            collection.end_bone   = end_bone.name
            print(collection.armature)
        elif C.selected_objects[0].type == 'ARMATURE' and C.active_object.mode == 'POSE':
            print("POSE Mode to do")
            start_bone = None
            end_bone = None
        else:
            start_bone = None
            end_bone = None
            print("Do the default case where empty aren't connected to the armature")
    else: 
        print("No object selected")
        start_bone = None
        end_bone = None
    
    # Create the 3 empties
    bpy.ops.object.mode_set(mode='OBJECT')
    start_target = bpy.data.objects.new(name+"_start_target", None)
    end_target    = bpy.data.objects.new(name+"_end_target", None)
    # middle_target = bpy.data.objects.new(name+"_middle_target", None)
    middle_target = create_middle_empty(start_target, end_target, name+"_middle_target")
    middle_target.empty_display_type = 'CIRCLE'
    if (start_bone and end_bone):
        start_target.location.y = -start_bone.length / 2
        end_target.location.y   = -end_bone.length / 2
    else:
        start_target.location.y = -2
        end_target.location.y   = 2
    collection.objects.link(start_target)
    collection.objects.link(middle_target)
    collection.objects.link(end_target)
    
    # Set bones parrent of each target
    if start_bone:
        start_target.parent      = armature
        start_target.parent_type = 'BONE'
        start_target.parent_bone = start_bone.name
    if end_bone:
        end_target.parent        = armature
        end_target.parent_type   = 'BONE'
        end_target.parent_bone   = end_bone.name
    bpy.context.view_layer.update() #Update to compute correct matrix_world translation
    muscle_length = (end_target.matrix_world.translation - start_target.matrix_world.translation).length
    print("linked")
    # Set constraint for the middle target
    # const_start_loc = middle_target.constraints.new(type='COPY_LOCATION')
    # const_start_loc.target = start_target
    # const_start_loc.use_offset = True
    # const_end_loc = middle_target.constraints.new(type='COPY_LOCATION')
    # const_end_loc.target     = end_target
    # const_end_loc.influence  = 0.5
    # const_start_rot = middle_target.constraints.new(type='TRACK_TO')
    # const_start_rot.target     = start_target
    # const_start_rot.up_axis    = 'UP_Z'
    # const_start_rot.track_axis = 'TRACK_Y'
    # const_end_rot = middle_target.constraints.new(type='TRACK_TO')
    # const_end_rot.target     = end_target
    # const_end_rot.up_axis    = 'UP_Z'
    # const_end_rot.track_axis = 'TRACK_NEGATIVE_Y'
    # const_end_rot.influence  = 0.5
    # bpy.context.view_layer.update() #Update to compute correct matrix_world translation
    
    # Create the muscle curve from a path nurbs
    print("create curve")
    crv = bpy.data.curves.new(name+'_crv', 'CURVE')
    crv.dimensions = '3D'
    spline = crv.splines.new(type='NURBS')
    spline.points.add(2) # theres already one point by default
    # assign the point coordinates to the spline points
    spline.points[0].co = (0, 0, 0, 1)# start_target.matrix_world.translation.to_4d()
    spline.points[1].co = (0, muscle_length/2, 0, 1)# middle_target.matrix_world.translation.to_4d()
    spline.points[2].co = (0, muscle_length, 0, 1)# end_target.matrix_world.translation.to_4d()
    spline.use_endpoint_u = True
    # make a new object with the curve
    print("create obj")
    muscle_curve = bpy.data.objects.new(name+"_curve", crv)
    collection.objects.link(muscle_curve)

    # Add muscle_curv constraints and modifiers
    const_curve_loc = muscle_curve.constraints.new(type='COPY_LOCATION')
    const_curve_loc.target = start_target
    end_modif = muscle_curve.modifiers.new('end', type='HOOK')
    end_modif.object = end_target
    middle_modif = muscle_curve.modifiers.new('middle', type='HOOK')
    middle_modif.object = middle_target
    bpy.ops.object.mode_set(mode = 'OBJECT')
    bpy.ops.object.select_all(action='DESELECT')
    bpy.context.view_layer.objects.active = muscle_curve
    # hook end
    muscle_curve.data.splines[0].points[2].select = True
    bpy.ops.object.mode_set(mode = 'EDIT')
    bpy.ops.object.hook_assign(modifier=end_modif.name)
    bpy.ops.object.hook_reset()
    bpy.ops.object.mode_set(mode = 'OBJECT')
    muscle_curve.data.splines[0].points[2].select = False
    # hook middle
    muscle_curve.data.splines[0].points[1].select = True
    bpy.ops.object.mode_set(mode = 'EDIT')
    bpy.ops.object.modifier_move_up(modifier=middle_modif.name)
    bpy.ops.object.hook_assign(modifier=middle_modif.name)
    bpy.ops.object.hook_reset()
    bpy.ops.object.mode_set(mode = 'OBJECT')

    # Create the taper
    taper = create_taper(collection)
    muscle_curve.data.bevel_depth = 1 
    muscle_curve.data.taper_object = taper
    muscle_curve.data.use_fill_caps = True
    taper.show_in_front = True
    taper.scale = (muscle_length, 1, 1) # transform it by a driver

    # Create shape controller
    # need to create an intermediaire empty
    middle_taper = create_middle_empty(start_target, end_target, name+"_middle_taper")
    taper_controler_shape = bpy.data.objects.new(name+"_taper_shape", None)
    taper_controler_shape.parent = middle_taper
    taper_controler_shape.empty_display_type = 'SPHERE'
    taper_controler_shape.empty_display_size = 0.3
    taper_controler_shape.location = (1, 0, 0)
    # done
    collection.objects.link(taper_controler_shape)

    # Create thickness controller
    taper_controler_thickness = bpy.data.objects.new(name+"_taper_thickness")
    taper_controler_thickness.parent = taper
    taper_controler_thickness.empty_display_type = 'SINGLE_ARROW'
    taper_controler_thickness.empty_display_size = 1
    taper_controler_thickness.rotation_euler[0] = -pi/2
    # Taper controller constraint
    limit_loc_thick = taper_controler_thickness.constraints.new('LIMIT_LOCATION')
    limit_loc_thick.use_min_x = True
    limit_loc_thick.use_max_x = True
    limit_loc_thick.use_min_z = True
    limit_loc_thick.use_max_z = True
    limit_loc_thick.owner_space = 'CUSTOM'
    limit_loc_thick.space_object = taper
    # Create the drivers (2 same on curve vert)
    # ...
    # done
    collection.objects.link(taper_controler_thickness)

    # Add taper constraint and modifiers
    const_taper_loc = taper.constraints.new(type="COPY_LOCATION")
    const_taper_loc.target = start_target
    # Create hooks
    shape_hook = taper.modifiers.new("shape", type="HOOK")
    shape_hook.object = taper_controler_shape
    # base_hook = taper.modifiers.new("thick", type="HOOK")
    # base_hook.object = taper_controler_thickness
    # Select the geom
    bpy.ops.object.mode_set(mode = 'OBJECT')
    bpy.ops.object.select_all(action='DESELECT')
    bpy.context.view_layer.objects.active = taper
    # Assigne 1st hook
    taper.data.splines[0].bezier_points[1].select_control_point = True
    taper.data.splines[0].bezier_points[1].select_left_handle = True
    taper.data.splines[0].bezier_points[1].select_right_handle = True
    bpy.ops.object.mode_set(mode = 'EDIT')
    bpy.ops.object.hook_assign(modifier=shape_hook.name)
    bpy.ops.object.hook_reset()
    bpy.ops.object.mode_set(mode = 'OBJECT')
    taper.data.splines[0].bezier_points[1].select_control_point = False
    taper.data.splines[0].bezier_points[1].select_left_handle = False
    taper.data.splines[0].bezier_points[1].select_right_handle = False
    # Assign 2nd hook
    # taper.data.splines[0].bezier_points[0].select_control_point = True
    # taper.data.splines[0].bezier_points[0].select_left_handle = True
    # taper.data.splines[0].bezier_points[0].select_right_handle = True
    # taper.data.splines[0].bezier_points[2].select_control_point = True
    # taper.data.splines[0].bezier_points[2].select_left_handle = True
    # taper.data.splines[0].bezier_points[2].select_right_handle = True
    # bpy.ops.object.mode_set(mode = 'EDIT')
    # bpy.ops.object.hook_assign(modifier=base_hook.name)
    # bpy.ops.object.mode_set(mode = 'OBJECT')
    # other parameter

    # Constraints
    taper_copy_loc = taper.constraints.new(type="COPY_LOCATION")
    taper_copy_loc.target = start_target
    taper_track_end = taper.constraints.new(type="DAMPED_TRACK")
    taper_track_end.target = end_target
    taper_track_end.track_axis = 'TRACK_X'
    taper_track_shape = taper.constraints.new(type="LOCKED_TRACK")
    taper_track_shape.target = taper_controler_shape
    taper_track_shape.track_axis = 'TRACK_Y'
    taper_track_shape.lock_axis = 'LOCK_X'

    # Drivers

    # Done

    # taper.location = start_target.matrix_world.translation
    # musc_vec = end_target.matrix_world.translation-start_target.matrix_world.translation
    # taper.scale = (musc_vec.length, 1, 1)
    # taper.rotation_mode = 'QUATERNION'
    # q_rot = Vector((1, 0, 0)).rotation_difference(musc_vec)
    # taper.rotation_quaternion = taper.rotation_quaternion.cross(q_rot)
    # print("Create taper hook")
    # taper_shape_modif  = taper.modifiers.new('Shape', type='HOOK')
    # taper_thick_modif  = taper.modifiers.new('Thick', type='HOOK')
    # taper_shape_modif.object  = taper_controler_shape
    # taper_thick_modif.object  = taper_controler_thickness
    # # select geom
    # print("Selected object = ", C.selected_objects)
    # bpy.ops.object.select_all(action='DESELECT')
    # bpy.context.view_layer.objects.active = taper
    # print("Selected object = ", C.selected_objects)
    # # assign the start 
    # taper.data.splines[0].bezier_points[1].select_control_point = True
    # taper.data.splines[0].bezier_points[1].select_left_handle   = True
    # taper.data.splines[0].bezier_points[1].select_right_handle  = True
    # bpy.ops.object.mode_set(mode = 'EDIT')
    # bpy.ops.object.hook_assign(modifier=taper_shape_modif.name)
    # bpy.ops.object.hook_reset()
    # bpy.ops.curve.select_all(action='DESELECT')
    # # taper.data.splines[0].bezier_points[1].select_control_point = False
    # bpy.ops.object.mode_set(mode = 'OBJECT')
    # taper.data.splines[0].bezier_points[0].select_control_point = True
    # taper.data.splines[0].bezier_points[0].select_left_handle   = True
    # taper.data.splines[0].bezier_points[0].select_right_handle  = True
    # taper.data.splines[0].bezier_points[2].select_control_point = True
    # taper.data.splines[0].bezier_points[2].select_left_handle   = True
    # taper.data.splines[0].bezier_points[2].select_right_handle  = True
    # bpy.ops.object.mode_set(mode = 'EDIT')
    # bpy.ops.object.modifier_move_up(modifier=taper_thick_modif.name)
    # bpy.ops.object.hook_assign(modifier=taper_thick_modif.name)
    # bpy.ops.object.hook_reset()
    # bpy.ops.object.mode_set(mode = 'OBJECT')
    # taper_controler_thickness.location += taper.matrix_world @ Vector((0, 0.2, 0))

def apply_muscle():
    print("")
    print("Apply Muscle")
    print("Working on...")
    # Select the muscle curve
    print("Select the muscle curve")
    bpy.ops.object.mode_set(mode = 'OBJECT')
    coll = bpy.context.object.users_collection[0]
    muscle_name = coll.name
    curve_muscle = None
    for obj in coll.objects:
        if "_curve" in obj.name:
            curve_muscle = obj
            break
    if not curve_muscle:
        print("Error: no curve in the muscle collection")
    bpy.ops.object.select_all(action='DESELECT')
    bpy.context.view_layer.objects.active = curve_muscle
    print(coll)
    print(curve_muscle)

    # Create the mesh frome the curve ( with filled hole )
    print("Create mesh")
    curve_muscle_eval = curve_muscle.evaluated_get(bpy.context.evaluated_depsgraph_get())
    muscle_mesh = bpy.data.meshes.new_from_object(curve_muscle_eval)
    muscle_geom = bpy.data.objects.new(name="Muscle_geom", object_data=muscle_mesh)
    coll.objects.link(muscle_geom)

    # Clean the curve (rename it ?)
    print("Clean active curve")
    curve_muscle.data.bevel_depth = 0
    curve_muscle.data.taper_object = None

    print("Recrate middle hool (TODO)")
    # Recreate the middle hook (if required)
    
    # delete useless controler
    print("delete controller")
    print([muscle_name+"_taper", muscle_name+"_taper_shape", muscle_name+"_taper_thickness"])
    for obj in coll.objects:
        print(obj.name)
        if obj.name in [muscle_name+"_taper", muscle_name+"_taper_shape", muscle_name+"_taper_thickness"]:
            print("delete "+obj.name)
            bpy.data.objects.remove(obj)



class ApplyMuscleOperator(bpy.types.Operator):
    """
    Convert the muscle sketch (a curve) in a mesh ready to sculpt
    """
    bl_idname = "simuscle.apply_muscle"
    bl_label = "Apply"

    def execute(self, context):
        apply_muscle()
        return {'FINISHED'}
    
class AddMuscleOperator(bpy.types.Operator):
    """
    Select the two armature bones that the muscle you want to create is linked to. 
    It will create a bone connect to the selected bones that you will be able to 
    edit with Blender.
    """
    bl_idname = "simuscle.add_muscle"
    bl_label = "Add Muscle"

    @classmethod
    def poll(cls, context):
        # What is the purpose of this function ????
        return context.active_object is not None

    def execute(self, context):
        name = "Biceps_test"
        create_muscle(name)
        return {'FINISHED'}

class DeleteMuscleOperator(bpy.types.Operator):
    """
    Delete all the object linked to the selected muscle
    """
    bl_idname = "simuscle.delete_muscle"
    bl_label = "Delete Muscle"

    def execute(self, context):
        coll = context.object.users_collection[0]
        for obj in coll.objects:
            bpy.data.objects.remove(obj, do_unlink=True)
        bpy.data.collections.remove(coll)
        return {'FINISHED'}


def update_start_bone(self, context):
    print("TODO: update start bone on each object of the muscel")

def update_end_bone(self, context):
    print("TODO: update end bone on each object of the muscel")


def register():
    # Add muscle property in collection
    bpy.types.Collection.armature = bpy.props.StringProperty(name="Armature", 
                                                             description="Armature on which the muscle is attached to")
    bpy.types.Collection.start_bone = bpy.props.StringProperty(name="Start bone", 
                                                             description="The bone on which the muscle is insert to",
                                                             update=update_start_bone)
    bpy.types.Collection.end_bone = bpy.props.StringProperty(name="End bone", 
                                                             description="The bone on which the muscle is insert to",
                                                             update=update_end_bone) 
    bpy.utils.register_class(AddMuscleOperator)
    bpy.utils.register_class(ApplyMuscleOperator)
    bpy.utils.register_class(DeleteMuscleOperator)


def unregister():
    bpy.utils.unregister_class(AddMuscleOperator)
    bpy.utils.unregister_class(ApplyMuscleOperator)
    bpy.utils.unregister_class(DeleteMuscleOperator)

if __name__=="__main__":
    register()