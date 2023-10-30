import bpy 
from mathutils import Vector
C = bpy.context

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
    taper = bpy.data.objects.new('Taper', crv)
    collection.objects.link(taper)
    return taper


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
    collection = bpy.data.collections.new(name)
    bpy.context.scene.collection.children.link(collection)
    collection.color_tag = 'COLOR_01'
    # Select the bones associated of the muscle if possible:
    # if a joint is selected
    if C.selected_objects[0].type == 'ARMATURE':
        armature = C.selected_objects[0]
        print("armature selected: ", armature.name)
        if C.active_object.mode == 'EDIT':
            print("select the bone")
            if len(C.selected_bones) < 2:
                bpy.ops.armature.select_more()
            else:
                print("Warning: Too musch bones selected. Only the joint have to be selected")
            start_bone = C.selected_bones[0]
            end_bone   = C.selected_bones[1]
            print("The bones of the muscle are: ", C.selected_bones)
            print(start_bone.name)
            print(end_bone.name)
        elif C.active_object.mode == 'POSE':
            print("POSE Mode to do")
        else:
            print("Do the default case where empty aren't connected to the armature")
            return
    
    # Create the 3 empties
    bpy.ops.object.mode_set(mode='OBJECT')
    start_target = bpy.data.objects.new("start_target", None)
    middle_target = bpy.data.objects.new("middle_target", None)
    end_target    = bpy.data.objects.new("end_target", None)
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
    start_target.parent      = armature
    start_target.parent_type = 'BONE'
    start_target.parent_bone = start_bone.name
    end_target.parent        = armature
    end_target.parent_type   = 'BONE'
    end_target.parent_bone   = end_bone.name
    # For the middle bone, create constraint to be at the avg location of the other target
    const_start = middle_target.constraints.new(type='COPY_LOCATION')
    const_start.target = start_target
    const_end = middle_target.constraints.new(type='COPY_LOCATION')
    const_end.target     = end_target
    const_end.influence  = 0.5
    bpy.context.view_layer.update() #Update to compute correct matrix_world translation
    
    # Create the muscle geometry from a path nurbs
    # make a new curve of 3 point
    crv = bpy.data.curves.new('crv', 'CURVE')
    crv.dimensions = '3D'
    spline = crv.splines.new(type='NURBS')
    spline.points.add(2) # theres already one point by default
    # assign the point coordinates to the spline points

    spline.points[0].co = start_target.matrix_world.translation.to_4d()
    spline.points[1].co = middle_target.matrix_world.translation.to_4d()
    spline.points[2].co = end_target.matrix_world.translation.to_4d()
    spline.use_endpoint_u = True
    # make a new object with the curve
    muscle = bpy.data.objects.new('Muscle_name', crv)
    collection.objects.link(muscle)
    muscle.data.bevel_depth = 1 
    taper = create_taper(collection)
    muscle.data.taper_object = taper
    # bpy.context.view_layer.update()
    print("Taper linked")

    # Place the taper
    bpy.context.view_layer.update()
    taper.location = start_target.matrix_world.translation
    musc_vec = end_target.matrix_world.translation-start_target.matrix_world.translation
    taper.scale = (musc_vec.length, 1, 1)
    taper.rotation_mode = 'QUATERNION'
    q_rot = Vector((1, 0, 0)).rotation_difference(musc_vec)
    taper.rotation_quaternion = taper.rotation_quaternion.cross(q_rot)
    # other parameter
    taper.show_in_front = True
    
    # Create hook to attach curve to empties
    start_modif  = muscle.modifiers.new('start', type='HOOK')
    middle_modif = muscle.modifiers.new('middle', type='HOOK')
    end_modif    = muscle.modifiers.new('end', type='HOOK')
    start_modif.object  = start_target
    middle_modif.object = middle_target
    end_modif.object    = end_target
    # Select the geom
    bpy.ops.object.mode_set(mode = 'OBJECT')
    bpy.ops.object.select_all(action='DESELECT')
    bpy.context.view_layer.objects.active = muscle
    # points = muscle.data.splines[0].points
    
    # assign the start 
    muscle.data.splines[0].points[0].select = True
    bpy.ops.object.mode_set(mode = 'EDIT')
    bpy.ops.object.hook_assign(modifier=start_modif.name)
    muscle.data.splines[0].points[0].select = False
    bpy.ops.object.mode_set(mode = 'OBJECT')
    # assign the middle
    muscle.data.splines[0].points[1].select = True
    bpy.ops.object.mode_set(mode = 'EDIT')
    bpy.ops.object.hook_assign(modifier=middle_modif.name)
    bpy.ops.object.mode_set(mode = 'OBJECT')
    muscle.data.splines[0].points[1].select = False
    # assign the end
    muscle.data.splines[0].points[2].select = True
    bpy.ops.object.mode_set(mode = 'EDIT')
    bpy.ops.object.hook_assign(modifier=end_modif.name)
    bpy.ops.object.mode_set(mode = 'OBJECT')
    muscle.data.splines[0].points[1].select = True

    # Create taper controler
    taper_controler_shape = bpy.data.objects.new("taper_shape", None)
    taper_controler_shape.location = taper.matrix_world @ taper.data.splines[0].bezier_points[1].co # world pt location
    taper_controler_shape.empty_display_type = 'SPHERE'
    taper_controler_shape.empty_display_size = 0.3
    taper_controler_thickness = bpy.data.objects.new("taper_thickness", None)
    taper_controler_thickness.location = taper.matrix_world @ (taper.data.splines[0].bezier_points[0].co + Vector((0, 0.0, 0)))# world pt location
    taper_controler_thickness.empty_display_type = 'SPHERE'
    taper_controler_thickness.empty_display_size = 0.3
    collection.objects.link(taper_controler_shape)
    collection.objects.link(taper_controler_thickness)

    # create modifier for taper
    print("Create taper hook")
    taper_shape_modif  = taper.modifiers.new('Shape', type='HOOK')
    taper_thick_modif  = taper.modifiers.new('Thick', type='HOOK')
    taper_shape_modif.object  = taper_controler_shape
    taper_thick_modif.object  = taper_controler_thickness
    # select geom
    print("Selected object = ", C.selected_objects)
    bpy.ops.object.select_all(action='DESELECT')
    bpy.context.view_layer.objects.active = taper
    print("Selected object = ", C.selected_objects)
    # assign the start 
    taper.data.splines[0].bezier_points[1].select_control_point = True
    taper.data.splines[0].bezier_points[1].select_left_handle   = True
    taper.data.splines[0].bezier_points[1].select_right_handle  = True
    bpy.ops.object.mode_set(mode = 'EDIT')
    bpy.ops.object.hook_assign(modifier=taper_shape_modif.name)
    bpy.ops.object.hook_reset()
    bpy.ops.curve.select_all(action='DESELECT')
    # taper.data.splines[0].bezier_points[1].select_control_point = False
    bpy.ops.object.mode_set(mode = 'OBJECT')
    taper.data.splines[0].bezier_points[0].select_control_point = True
    taper.data.splines[0].bezier_points[0].select_left_handle   = True
    taper.data.splines[0].bezier_points[0].select_right_handle  = True
    taper.data.splines[0].bezier_points[2].select_control_point = True
    taper.data.splines[0].bezier_points[2].select_left_handle   = True
    taper.data.splines[0].bezier_points[2].select_right_handle  = True
    bpy.ops.object.mode_set(mode = 'EDIT')
    bpy.ops.object.modifier_move_up(modifier=taper_thick_modif.name)
    bpy.ops.object.hook_assign(modifier=taper_thick_modif.name)
    bpy.ops.object.hook_reset()
    bpy.ops.object.mode_set(mode = 'OBJECT')
    taper_controler_thickness.location += taper.matrix_world @ Vector((0, 0.2, 0))

    # build Constraint
    limit_loc_thick = taper_controler_thickness.constraints.new('LIMIT_LOCATION')
    limit_loc_thick.use_min_x = True
    limit_loc_thick.use_max_x = True
    limit_loc_thick.use_min_z = True
    limit_loc_thick.use_max_z = True
    limit_loc_thick.owner_space = 'CUSTOM'
    limit_loc_thick.space_object = taper

    track = taper.constraints.new('LOCKED_TRACK')
    track.target = taper_controler_shape
    track.lock_axis = 'LOCK_X'
    
class AddMuscleOperator(bpy.types.Operator):
    """Select the two armature bones that the muscle you want to create is linked to"""
    bl_idname = "object.add_muscle"
    bl_label = "Add Muscle Operator"

    @classmethod
    def poll(cls, context):
        return context.active_object is not None

    def execute(self, context):
        name = "Biceps_test"
        create_muscle(name)
        return {'FINISHED'}
