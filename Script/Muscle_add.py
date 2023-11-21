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
    taper = bpy.data.objects.new('taper_'+collection.muscle_name, crv)

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
    start_target = bpy.data.objects.new("start_target_"+name, None)
    end_target    = bpy.data.objects.new("end_target_"+name, None)
    middle_target = create_middle_empty(start_target, end_target, "middle_target_"+name)
    start_target.empty_display_type = 'SPHERE'
    middle_target.empty_display_type = 'CIRCLE'
    end_target.empty_display_type = 'SPHERE'
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
    muscle_vec = end_target.matrix_world.translation - start_target.matrix_world.translation
    muscle_length = muscle_vec.length
    print(muscle_length)
    print("linked")
    
    # Create the muscle curve from a path nurbs
    print("create curve")
    crv = bpy.data.curves.new('crv_'+name, 'CURVE')
    crv.dimensions = '3D'
    spline = crv.splines.new(type='NURBS')
    spline.points.add(2) # theres already one point by default
    # assign the point coordinates to the spline points
    spline.points[0].co = (0, 0, 0, 1)               # start_target.matrix_world.translation.to_4d()
    spline.points[1].co = (0, muscle_length/2, 0, 1) # middle_target.matrix_world.translation.to_4d()
    spline.points[2].co = (0, muscle_length, 0, 1)   # end_target.matrix_world.translation.to_4d()
    crv.taper_radius_mode = 'ADD'
    spline.points[0].radius = 0.
    spline.points[1].radius = 0.
    spline.points[2].radius = 0.
    spline.use_endpoint_u = True
    # make a new object with the curve
    print("create obj")
    muscle_curve = bpy.data.objects.new("curve_"+name, crv)
    q_rot = Vector((0, 1, 0)).rotation_difference(muscle_vec)
    muscle_curve.rotation_mode = 'QUATERNION'
    muscle_curve.rotation_quaternion = muscle_curve.rotation_quaternion.cross(q_rot)
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
    # driver

    # Create the taper
    taper = create_taper(collection)
    muscle_curve.data.bevel_depth = 1 
    muscle_curve.data.taper_object = taper
    muscle_curve.data.use_fill_caps = True
    taper.show_in_front = True
    taper.scale = (1, 1, 1)

    # Create shape controller
    # need to create an intermediaire empty
    middle_taper = create_middle_empty(start_target, end_target, "middle_taper_"+name)
    taper_controler_shape = bpy.data.objects.new("taper_shape_"+name, None)
    taper_controler_shape.parent = middle_taper
    taper_controler_shape.empty_display_type = 'SPHERE'
    taper_controler_shape.empty_display_size = 0.3
    taper_controler_shape.location = (0.7, 0, 0)
    taper_controler_shape.show_in_front = True
    # done
    collection.objects.link(taper_controler_shape)

    # Create thickness controller
    taper_controler_thickness = bpy.data.objects.new("taper_thickness_"+name, None)
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
    # Driver
    driver_scale = taper_controler_thickness.driver_add('scale', 0)
    var = driver_scale.driver.variables.new()
    var.name = "length"
    var.type = "LOC_DIFF"
    var.targets[0].id = start_target
    var.targets[1].id = end_target
    driver_scale.driver.expression = "1/length"
    # done
    collection.objects.link(taper_controler_thickness)

    # Add taper constraint and modifiers
    # Constraints
    const_taper_loc = taper.constraints.new(type="COPY_LOCATION")
    const_taper_loc.target = start_target
    taper_track_end = taper.constraints.new(type="DAMPED_TRACK")
    taper_track_end.target = end_target
    taper_track_end.track_axis = 'TRACK_X'
    taper_track_shape = taper.constraints.new(type="LOCKED_TRACK")
    taper_track_shape.target = taper_controler_shape
    taper_track_shape.track_axis = 'TRACK_Y'
    taper_track_shape.lock_axis = 'LOCK_X'
    # Drivers
    driver = taper.driver_add("scale", 0)
    var = driver.driver.variables.new()
    var.name = "length"
    var.type = 'LOC_DIFF'
    var.targets[0].id = start_target
    var.targets[1].id = end_target
    driver.driver.expression = "length"

    # Add thick_controle driver for thick an counter size
    driver_p0 = taper.data.splines[0].bezier_points[0].driver_add("co", 1)
    driver_hl0 = taper.data.splines[0].bezier_points[0].driver_add("handle_left", 1)
    driver_hr0 = taper.data.splines[0].bezier_points[0].driver_add("handle_right", 1)
    driver_p2 = taper.data.splines[0].bezier_points[2].driver_add("co", 1)
    driver_hl2 = taper.data.splines[0].bezier_points[2].driver_add("handle_left", 1)
    driver_hr2 = taper.data.splines[0].bezier_points[2].driver_add("handle_right", 1)
    drivers = [driver_p0, driver_hl0, driver_hr0, driver_p2, driver_hl2, driver_hr2]
    for driver in drivers:
        var = driver.driver.variables.new()
        var.name = "thick"
        var.type = "LOC_DIFF"
        var.targets[0].id = taper_controler_thickness
        var.targets[1].id = start_target
        driver.driver.expression = "thick"
    # var_p2 = driver_p2.driver.variables.new()
    # var_p2.name = "thick"
    # var_p2.type = "LOC_DIFF"
    # var_p2.targets[0].id = taper_controler_thickness
    # var_p2.targets[1].id = start_target
    # driver_p2.driver.expression = "thick"
    
    # start and end target size driver
    driver = start_target.driver_add("empty_display_size")
    var = driver.driver.variables.new()
    var.name = "size"
    var.type = 'LOC_DIFF'
    var.targets[0].id = start_target
    var.targets[1].id = taper_controler_thickness
    driver.driver.expression = "size + 0.1 if size>0.2 else 0.3"
    driver = end_target.driver_add("empty_display_size")
    var = driver.driver.variables.new()
    var.name = "size"
    var.type = 'LOC_DIFF'
    var.targets[0].id = start_target
    var.targets[1].id = taper_controler_thickness
    driver.driver.expression = "size + 0.1 if size>0.2 else 0.3"
    # Add middle_target size driver
    driver_scale_all = middle_target.driver_add("scale")
    print(driver_scale_all)
    for driver in driver_scale_all:
        print(driver)
        print(driver.data_path)
        print(driver.array_index)
        print(driver.driver)
        var = driver.driver.variables.new()
        var.name = "width"
        var.type = 'LOC_DIFF'
        var.targets[0].id = taper_controler_shape
        var.targets[1].id = middle_taper
        driver.driver.expression = "width"
    # Create hooks
    shape_hook = taper.modifiers.new("shape", type="HOOK")
    shape_hook.object = taper_controler_shape
    bpy.ops.object.mode_set(mode = 'OBJECT')
    bpy.ops.object.select_all(action='DESELECT')
    bpy.context.view_layer.objects.active = taper
    # Assigne shape hook hook
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

    # Place shaper controller at top
    taper_controler_shape.location = (0, 0, 0.7)
    collection.muscle_name = name

    # Set the material
    mat = bpy.data.materials.get("Muscle")
    muscle_curve.data.materials.append(mat)



def apply_muscle():
    print("")
    print("Apply Muscle")
    print("Working on...")
    # Select the muscle curve
    print("Select the muscle curve")
    bpy.ops.object.mode_set(mode = 'OBJECT')
    coll = bpy.context.object.users_collection[0]
    muscle_name = coll.muscle_name
    curve_muscle = None
    for obj in coll.objects:
        if "curve" == obj.name[:5]:
            curve_muscle = obj
            break
    if not curve_muscle:
        print("Error: no curve in the muscle collection")
    bpy.ops.object.select_all(action='DESELECT')
    bpy.context.view_layer.objects.active = curve_muscle
    print(coll)
    print(curve_muscle)
    print("selected: ", [obj.name for obj in bpy.context.selected_objects])

    # Create the mesh frome the curve ( with filled hole )
    print("Create mesh")
    curve_muscle_eval = curve_muscle.evaluated_get(bpy.context.evaluated_depsgraph_get())
    muscle_mesh = bpy.data.meshes.new_from_object(curve_muscle_eval)
    muscle_geom = bpy.data.objects.new(name="geom_"+muscle_name, object_data=muscle_mesh)
    muscle_geom.location = curve_muscle.matrix_world.translation
    coll.objects.link(muscle_geom)
    print(C.object)
    bpy.ops.constraint.apply(constraint="Copy Location", owner='OBJECT', report=True)
    bpy.ops.object.modifier_apply(modifier="end", report=True)
    bpy.ops.object.modifier_apply(modifier="middle", report=True)

    # Clean the curve (rename it ?)
    print("Clean active curve")
    curve_muscle.data.bevel_depth = 0
    curve_muscle.data.taper_object = None
    curve_muscle.show_in_front = True


    print("Recrate middle hool (TODO)")
    # Recreate the middle hook (if required)
    
    # delete useless controler
    print("delete controller")
    delete_list = ["taper_"+muscle_name, "taper_shape_"+muscle_name, "taper_thickness_"+muscle_name, "start_target_"+muscle_name, "middle_target_"+muscle_name, "end_target_"+muscle_name]
    print(delete_list)
    for obj in coll.objects:
        print(obj.name)
        if obj.name in delete_list:
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

    # @classmethod
    # def poll(cls, context):
    #     # What is the purpose of this function ????
    #     return context.active_object is not None

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

def update_name(self, context):
    self.name = self.muscle_name
    objects = self.objects
    for obj in objects:
        words = obj.name.split("_")
        if (words[0] == "taper") or (words[0] == "curve"):
            if words[1] in ["shape", "thickness"]:
                obj.name = "_".join(words[:2] + [self.muscle_name])
            else:
                obj.name = words[0] + "_" + self.muscle_name
        else:
            obj.name = "_".join(words[:2] + [self.muscle_name])

def update_armature(self, context):
    start_target = self.objects.get("start_target_"+self.muscle_name)
    end_target = self.objects.get("end_target_"+self.muscle_name)
    for obj in bpy.data.objects:
        if obj.data:
            if obj.data.name == self.armature:
                print(start_target.name)
                start_target.parent = obj
                start_target.parent_type = 'BONE'
                end_target.parent = obj
                end_target.parent_type = 'BONE'
                return

def update_start_bone(self, context):
    start_target = [obj for obj in self.objects if obj.name[:12] == "start_target"][0]
    start_target.parent_bone = self.start_bone

def update_end_bone(self, context):
    end_target = [obj for obj in self.objects if obj.name[:10] == "end_target"][0]
    end_target.parent_bone = self.end_bone


def register():
    # Add muscle property in collection
    bpy.types.Collection.muscle_name = bpy.props.StringProperty(name="muscle_name", 
                                                         description="Name of the muscle",
                                                         update=update_name)
    bpy.types.Collection.armature = bpy.props.StringProperty(name="Armature", 
                                                             description="Armature on which the muscle is attached to",
                                                             update=update_armature)
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