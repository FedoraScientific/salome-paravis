# Copyright (C) 2010-2013  CEA/DEN, EDF R&D
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
#
# See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
#

"""
This module is intended to provide Python API for building presentations
typical for Post-Pro module (Scalar Map, Deformed Shape, Vectors, etc.)
"""


from __future__ import division
##from __future__ import print_function

import os
import re
import warnings
from math import sqrt, sin, cos, radians
from string import upper

# Do not use pv as a short name.
# It is a name of function from numpy and may be redefined implicitly by 'from numpy import *' call.
# import pvsimple as pv
import pvsimple as pvs
#try:
#    # TODO(MZN): to be removed (issue with Point Sprite texture)
#    #import paravisSM as sm
#except:
#    import paraview.simple as pvs
#    import paraview.servermanager as sm


# Constants
EPS = 1E-3
FLT_MIN = 1E-37
VTK_LARGE_FLOAT = 1E+38
GAP_COEFFICIENT = 0.0001


# Globals
_current_bar = None
_med_field_sep = '@@][@@'


# Enumerations
class PrsTypeEnum:
    """
    Post-Pro presentation types.
    """
    MESH = 0
    SCALARMAP = 1
    ISOSURFACES = 2
    CUTPLANES = 3
    CUTLINES = 4
    DEFORMEDSHAPE = 5
    DEFORMEDSHAPESCALARMAP = 6
    VECTORS = 7
    PLOT3D = 8
    STREAMLINES = 9
    GAUSSPOINTS = 10

    _type2name = {MESH: 'Mesh',
                  SCALARMAP: 'Scalar Map',
                  ISOSURFACES: 'Iso Surfaces',
                  CUTPLANES: 'Cut Planes',
                  CUTLINES: 'Cut Lines',
                  DEFORMEDSHAPE: 'Deformed Shape',
                  DEFORMEDSHAPESCALARMAP: 'Deformed Shape And Scalar Map',
                  VECTORS: 'Vectors',
                  PLOT3D: 'Plot3D',
                  STREAMLINES: 'Stream Lines',
                  GAUSSPOINTS: 'Gauss Points'}

    @classmethod
    def get_name(cls, type):
        """Return presentaion name by its type."""
        return cls._type2name[type]


class EntityType:
    """
    Entity types.
    """
    NODE = 0
    CELL = 1

    _type2name = {NODE: 'P1',
                  CELL: 'P0'}

    _name2type = {'P1': NODE,
                  'P0': CELL}

    _type2pvtype = {NODE: 'POINT_DATA',
                    CELL: 'CELL_DATA'}

    @classmethod
    def get_name(cls, type):
        """Return entity name (used in full group names) by its type."""
        return cls._type2name[type]

    @classmethod
    def get_type(cls, name):
        """Return entity type by its name (used in full group names)."""
        return cls._name2type[name]

    @classmethod
    def get_pvtype(cls, type):
        """Return entity type from ['CELL_DATA', 'POINT_DATA']"""
        return cls._type2pvtype[type]


class Orientation:
    """Orientation types.

    Defines a set of plane orientation possibilities:
      AUTO: plane orientation should be calculated.
      XY: plane formed by X and Y axis.
      YZ: plane formed by Y and Z axis.
      ZX: plane formed by Z and X axis

    """
    AUTO = 0
    XY = 1
    YZ = 2
    ZX = 3


class GlyphPos:
    """Glyph positions.

    Set of elements defining the position of the vector head:
      CENTER: in the center of the vector
      TAIL: in the tail of the vector
      HEAD: in the head of the vector

    """
    CENTER = 0
    TAIL = 1
    HEAD = 2


class GaussType:
    """
    Gauss Points primitive types.
    """
    SPRITE = 0
    POINT = 1
    SPHERE = 2

    _type2mode = {SPRITE: 'Texture',
                  POINT: 'SimplePoint',
                  SPHERE: 'Sphere (Texture)'}

    @classmethod
    def get_mode(cls, type):
        """Return paraview point sprite mode by the primitive type."""
        return cls._type2mode[type]


# Auxiliary functions

def get_field_mesh_name(full_field_name):
    """Return mesh name of the field by its full name."""
    aList = full_field_name.split('/')
    if len(aList) >= 2 :
        field_name = full_field_name.split('/')[1]
        return field_name


def get_field_entity(full_field_name):
    """Return entity type of the field by its full name."""
    aList = full_field_name.split(_med_field_sep)
    if len(aList) == 2 :
        entity_name = full_field_name.split(_med_field_sep)[-1]
        entity = EntityType.get_type(entity_name)
        return entity


def get_field_short_name(full_field_name):
    """Return short name of the field by its full name."""
    aList = full_field_name.split('/')
    if len(aList) == 4 :
        short_name_with_type = full_field_name.split('/')[-1]
        short_name = short_name_with_type.split(_med_field_sep)[0]
        return short_name


def find_mesh_full_name(proxy, short_mesh_name):
    """Return full mesh path by short mesh name, if found"""
    proxy.UpdatePipeline()
    all_mesh_names = get_mesh_full_names(proxy)
    for name in all_mesh_names:
        if short_mesh_name == get_field_short_name(name):
            return name


def process_prs_for_test(prs, view, picture_name, show_bar=True):
    """Show presentation and record snapshot image.

    Arguments:
      prs: the presentation to show
      view: the render view
      picture_name: the full name of the graphics file to save
      show_bar: to show scalar bar or not

    """
    # Show the presentation only
    display_only(prs, view)

    # Show scalar bar
    if show_bar and _current_bar:
        _current_bar.Visibility = 1

    # Reset the view
    reset_view(view)

    # Create a directory for screenshot if necessary
    file_name = re.sub("\s+", "_", picture_name)
    pic_dir = os.path.dirname(picture_name)
    if not os.path.exists(pic_dir):
        os.makedirs(pic_dir)

    # Save picture
    pvs.WriteImage(file_name, view=view, Magnification=1)


def reset_view(view=None):
    """Reset the view.

    Set predefined (taken from Post-Pro) camera settings.
    If the view is not passed, the active view is used.

    """
    if not view:
        view = pvs.GetRenderView()

    # Camera preferences
    view.CameraFocalPoint = [0.0, 0.0, 0.0]
    view.CameraViewUp = [0.0, 0.0, 1.0]
    view.CameraPosition = [738.946, -738.946, 738.946]

    # Turn on the headligth
    view.LightSwitch = 1
    view.LightIntensity = 0.5

    # Use parallel projection
    view.CameraParallelProjection = 1

    view.ResetCamera()
    pvs.Render(view=view)


def hide_all(view, to_remove=False):
    """Hide all representations in the view."""
    if not view:
        view = pvs.GetRenderView()

    rep_list = view.Representations
    for rep in rep_list:
        if hasattr(rep, 'Visibility') and rep.Visibility != 0:
            rep.Visibility = 0
        if to_remove:
            view.Representations.remove(rep)
    pvs.Render(view=view)


def display_only(prs, view=None):
    """Display only the given presentation in the view."""
    if not view:
        view = pvs.GetRenderView()

    rep_list = view.Representations
    for rep in rep_list:
        if hasattr(rep, 'Visibility'):
            rep.Visibility = (rep == prs)
    pvs.Render(view=view)


def set_visible_lines(xy_prs, lines):
    """Set visible only the given lines for XYChartRepresentation."""
    sv = xy_prs.GetProperty("SeriesVisibilityInfo").GetData()
    visible = '0'

    for i in xrange(0, len(sv)):
        if i % 2 == 0:
            line_name = sv[i]
            if line_name in lines:
                visible = '1'
            else:
                visible = '0'
        else:
            sv[i] = visible

    xy_prs.SeriesVisibility = sv


def check_vector_mode(vector_mode, nb_components):
    """Check vector mode.

    Check if vector mode is correct for the data array with the
    given number of components.

    Arguments:
      vector_mode: 'Magnitude', 'X', 'Y' or 'Z'
      nb_components: number of component in the data array

    Raises:
      ValueError: in case of the vector mode is unexistent
      or nonapplicable.

    """
    if vector_mode not in ('Magnitude', 'X', 'Y', 'Z'):
        raise ValueError("Unexistent vector mode: " + vector_mode)

    if ((nb_components == 1 and (vector_mode == 'Y' or vector_mode == 'Z')) or
        (nb_components == 2 and  vector_mode == 'Z')):
        raise ValueError("Incorrect vector mode " + vector_mode + " for " +
                         nb_components + "-component field")


def get_vector_component(vector_mode):
    """Get vector component as ineger.

    Translate vector component notation from string
    to integer:
      'Magnitude': -1
      'X': 0
      'Y': 1
      'Z': 2

    """
    vcomponent = -1

    if vector_mode == 'X':
        vcomponent = 0
    elif vector_mode == 'Y':
        vcomponent = 1
    elif vector_mode == 'Z':
        vcomponent = 2

    return vcomponent


def get_data_range(proxy, entity, field_name, vector_mode='Magnitude',
                   cut_off=False):
    """Get data range for the field.

    Arguments:
      proxy: the pipeline object, containig data array for the field
      entity: the field entity
      field_name: the field name
      vector_mode: the vector mode ('Magnitude', 'X', 'Y' or 'Z')

    Returns:
      Data range as [min, max]

    """
    proxy.UpdatePipeline()
    entity_data_info = None
    field_data = proxy.GetFieldDataInformation()

    if field_name in field_data.keys():
        entity_data_info = field_data
    elif entity == EntityType.CELL:
        entity_data_info = proxy.GetCellDataInformation()
    elif entity == EntityType.NODE:
        entity_data_info = proxy.GetPointDataInformation()

    data_range = []

    if field_name in entity_data_info.keys():
        vcomp = get_vector_component(vector_mode)
        data_range = entity_data_info[field_name].GetComponentRange(vcomp)
    else:
        pv_entity = EntityType.get_pvtype(entity)
        warnings.warn("Field " + field_name +
                      " is unknown for " + pv_entity + "!")

    # Cut off the range
    if cut_off and (data_range[0] <= data_range[1]):
        data_range = list(data_range)
        delta = abs(data_range[1] - data_range[0]) * GAP_COEFFICIENT
        data_range[0] += delta
        data_range[1] -= delta

    return data_range


def get_bounds(proxy):
    """Get bounds of the proxy in 3D."""
    proxy.UpdatePipeline()
    dataInfo = proxy.GetDataInformation()
    bounds_info = dataInfo.GetBounds()
    return bounds_info


def get_x_range(proxy):
    """Get X range of the proxy bounds in 3D."""
    proxy.UpdatePipeline()
    bounds_info = get_bounds(proxy)
    return bounds_info[0:2]


def get_y_range(proxy):
    """Get Y range of the proxy bounds in 3D."""
    proxy.UpdatePipeline()
    bounds_info = get_bounds(proxy)
    return bounds_info[2:4]


def get_z_range(proxy):
    """Get Z range of the proxy bounds in 3D."""
    proxy.UpdatePipeline()
    bounds_info = get_bounds(proxy)
    return bounds_info[4:6]


def is_planar_input(proxy):
    """Check if the given input is planar."""
    proxy.UpdatePipeline()
    bounds_info = get_bounds(proxy)

    if (abs(bounds_info[0] - bounds_info[1]) <= FLT_MIN or
        abs(bounds_info[2] - bounds_info[3]) <= FLT_MIN or
        abs(bounds_info[4] - bounds_info[5]) <= FLT_MIN):
        return True

    return False


def is_data_on_cells(proxy, field_name):
    """Check the existence of a field on cells with the given name."""
    proxy.UpdatePipeline()
    cell_data_info = proxy.GetCellDataInformation()
    return (field_name in cell_data_info.keys())


def is_empty(proxy):
    """Check if the object contains any points or cells.

    Returns:
      True: if the given proxy doesn't contain any points or cells
      False: otherwise

    """
    proxy.UpdatePipeline()
    data_info = proxy.GetDataInformation()

    nb_cells = data_info.GetNumberOfCells()
    nb_points = data_info.GetNumberOfPoints()

    return not(nb_cells + nb_points)


def get_orientation(proxy):
    """Get the optimum cutting plane orientation for Plot 3D."""
    proxy.UpdatePipeline()
    orientation = Orientation.XY

    bounds = get_bounds(proxy)
    delta = [bounds[1] - bounds[0],
             bounds[3] - bounds[2],
             bounds[5] - bounds[4]]

    if (delta[0] >= delta[1] and delta[0] >= delta[2]):
        if (delta[1] >= delta[2]):
            orientation = Orientation.XY
        else:
            orientation = Orientation.ZX
    elif (delta[1] >= delta[0] and delta[1] >= delta[2]):
        if (delta[0] >= delta[2]):
            orientation = Orientation.XY
        else:
            orientation = Orientation.YZ
    elif (delta[2] >= delta[0] and delta[2] >= delta[1]):
        if (delta[0] >= delta[1]):
            orientation = Orientation.ZX
        else:
            orientation = Orientation.YZ

    return orientation


def dot_product(a, b):
    """Dot product of two 3-vectors."""
    dot = a[0] * b[0] + a[1] * b[1] + a[2] * b[2]
    return dot


def multiply3x3(a, b):
    """Mutltiply one 3x3 matrix by another."""
    c = [[0, 0, 0],
         [0, 0, 0],
         [0, 0, 0]]

    for i in xrange(3):
        c[0][i] = a[0][0] * b[0][i] + a[0][1] * b[1][i] + a[0][2] * b[2][i]
        c[1][i] = a[1][0] * b[0][i] + a[1][1] * b[1][i] + a[1][2] * b[2][i]
        c[2][i] = a[2][0] * b[0][i] + a[2][1] * b[1][i] + a[2][2] * b[2][i]

    return c


def get_rx(ang):
    """Get X rotation matrix by angle."""
    rx = [[1.0, 0.0,      0.0],
          [0.0, cos(ang), -sin(ang)],
          [0.0, sin(ang), cos(ang)]]

    return rx


def get_ry(ang):
    """Get Y rotation matrix by angle."""
    ry = [[cos(ang),  0.0, sin(ang)],
          [0.0,       1.0, 0.0],
          [-sin(ang), 0.0, cos(ang)]]

    return ry


def get_rz(ang):
    """Get Z rotation matrix by angle."""
    rz = [[cos(ang), -sin(ang), 0.0],
          [sin(ang), cos(ang),  0.0],
          [0.0,      0.0,       1.0]]

    return rz


def get_normal_by_orientation(orientation, ang1=0, ang2=0):
    """Get normal for the plane by its orientation."""
    i_plane = 0
    rotation = [[], [], []]
    rx = ry = rz = [[1.0, 0.0, 0.0],
                    [0.0, 1.0, 0.0],
                    [0.0, 0.0, 1.0]]

    normal = [0.0, 0.0, 0.0]
    if orientation == Orientation.XY:
        if abs(ang1) > EPS:
            rx = get_rx(ang1)
        if abs(ang2) > EPS:
            ry = get_ry(ang2)
        rotation = multiply3x3(rx, ry)
        i_plane = 2
    elif orientation == Orientation.ZX:
        if abs(ang1) > EPS:
            rz = get_rz(ang1)
        if abs(ang2) > EPS:
            rx = get_rx(ang2)
        rotation = multiply3x3(rz, rx)
        i_plane = 1
    elif orientation == Orientation.YZ:
        if abs(ang1) > EPS:
            ry = get_ry(ang1)
        if abs(ang2) > EPS:
            rz = get_rz(ang2)
        rotation = multiply3x3(ry, rz)
        i_plane = 0

    for i in xrange(0, 3):
        normal[i] = rotation[i][i_plane]

    return normal


def get_bound_project(bound_box, dir):
    """Get bounds projection"""
    bound_points = [[bound_box[0], bound_box[2], bound_box[4]],
                    [bound_box[1], bound_box[2], bound_box[4]],
                    [bound_box[0], bound_box[3], bound_box[4]],
                    [bound_box[1], bound_box[3], bound_box[4]],
                    [bound_box[0], bound_box[2], bound_box[5]],
                    [bound_box[1], bound_box[2], bound_box[5]],
                    [bound_box[0], bound_box[3], bound_box[5]],
                    [bound_box[1], bound_box[3], bound_box[5]]]

    bound_prj = [0, 0, 0]
    bound_prj[0] = dot_product(dir, bound_points[0])
    bound_prj[1] = bound_prj[0]

    for i in xrange(1, 8):
        tmp = dot_product(dir, bound_points[i])
        if bound_prj[1] < tmp:
            bound_prj[1] = tmp
        if bound_prj[0] > tmp:
            bound_prj[0] = tmp

    bound_prj[2] = bound_prj[1] - bound_prj[0]
    bound_prj[1] = bound_prj[0] + (1.0 - EPS) * bound_prj[2]
    bound_prj[0] = bound_prj[0] + EPS * bound_prj[2]
    bound_prj[2] = bound_prj[1] - bound_prj[0]

    return bound_prj


def get_positions(nb_planes, dir, bounds, displacement):
    """Compute plane positions."""
    positions = []
    bound_prj = get_bound_project(bounds, dir)
    if nb_planes > 1:
        step = bound_prj[2] / (nb_planes - 1)
        abs_displacement = step * displacement
        start_pos = bound_prj[0] - 0.5 * step + abs_displacement
        for i in xrange(nb_planes):
            pos = start_pos + i * step
            positions.append(pos)
    else:
        pos = bound_prj[0] + bound_prj[2] * displacement
        positions.append(pos)

    return positions


def get_contours(scalar_range, nb_contours):
    """Generate contour values."""
    contours = []
    for i in xrange(nb_contours):
        pos = scalar_range[0] + i * (
            scalar_range[1] - scalar_range[0]) / (nb_contours - 1)
        contours.append(pos)

    return contours


def get_nb_components(proxy, entity, field_name):
    """Return number of components for the field."""
    proxy.UpdatePipeline()
    entity_data_info = None
    field_data = proxy.GetFieldDataInformation()

    if field_name in field_data.keys():
        entity_data_info = field_data
    elif entity == EntityType.CELL:
        entity_data_info = proxy.GetCellDataInformation()
    elif entity == EntityType.NODE:
        entity_data_info = proxy.GetPointDataInformation()

    nb_comp = None
    if field_name in entity_data_info.keys():
        nb_comp = entity_data_info[field_name].GetNumberOfComponents()
    else:
        pv_entity = EntityType.get_pvtype(entity)
        raise ValueError("Field " + field_name +
                         " is unknown for " + pv_entity + "!")

    return nb_comp


def get_scale_factor(proxy):
    """Compute scale factor."""
    if not proxy:
        return 0.0

    proxy.UpdatePipeline()
    data_info = proxy.GetDataInformation()

    nb_cells = data_info.GetNumberOfCells()
    nb_points = data_info.GetNumberOfPoints()
    nb_elements = nb_cells if nb_cells > 0  else nb_points
    bounds = get_bounds(proxy)

    volume = 1
    vol = dim = 0

    for i in xrange(0, 6, 2):
        vol = abs(bounds[i + 1] - bounds[i])
        if vol > 0:
            dim += 1
            volume *= vol

    if nb_elements == 0 or dim < 1 / VTK_LARGE_FLOAT:
        return 0

    volume /= nb_elements

    return pow(volume, 1 / dim)


def get_default_scale(prs_type, proxy, entity, field_name):
    """Get default scale factor."""
    proxy.UpdatePipeline()
    data_range = get_data_range(proxy, entity, field_name)

    if prs_type == PrsTypeEnum.DEFORMEDSHAPE:
        EPS = 1.0 / VTK_LARGE_FLOAT
        if abs(data_range[1]) > EPS:
            scale_factor = get_scale_factor(proxy)
            return scale_factor / data_range[1]
    elif prs_type == PrsTypeEnum.PLOT3D:
        bounds = get_bounds(proxy)
        length = sqrt((bounds[1] - bounds[0]) ** 2 +
                      (bounds[3] - bounds[2]) ** 2 +
                      (bounds[5] - bounds[4]) ** 2)

        EPS = 0.3
        if data_range[1] > 0:
            return length / data_range[1] * EPS

    return 0


def get_calc_magnitude(proxy, array_entity, array_name):
    """Compute magnitude for the given vector array via Calculator.

    Returns:
      the calculator object.

    """
    proxy.UpdatePipeline()
    calculator = None

    # Transform vector array to scalar array if possible
    nb_components = get_nb_components(proxy, array_entity, array_name)
    if (nb_components > 1):
        calculator = pvs.Calculator(proxy)
        attribute_mode = "Point Data"
        if array_entity != EntityType.NODE:
            attribute_mode = "Cell Data"
        calculator.AttributeMode = attribute_mode
        if (nb_components == 2):
            # Workaroud: calculator unable to compute magnitude
            # if number of components equal to 2
            func = "sqrt(" + array_name + "_X^2+" + array_name + "_Y^2)"
            calculator.Function = func
        else:
            calculator.Function = "mag(" + array_name + ")"
        calculator.ResultArrayName = array_name + "_magnitude"
        calculator.UpdatePipeline()

    return calculator


def get_add_component_calc(proxy, array_entity, array_name):
    """Creates 3-component array from 2-component.

    The first two components is from the original array. The 3rd component
    is zero.
    If the number of components is not equal to 2 - return original array name.

    Returns:
      the calculator object.

    """
    proxy.UpdatePipeline()
    calculator = None

    nb_components = get_nb_components(proxy, array_entity, array_name)
    if nb_components == 2:
        calculator = pvs.Calculator(proxy)
        attribute_mode = "Point Data"
        if array_entity != EntityType.NODE:
            attribute_mode = "Cell Data"
        calculator.AttributeMode = attribute_mode
        expression = "iHat * " + array_name + "_X + jHat * " + array_name + "_Y + kHat * 0"
        calculator.Function = expression
        calculator.ResultArrayName = array_name + "_3c"
        calculator.UpdatePipeline()

    return calculator


def select_all_cells(proxy):
    """Select all cell types.

    Used in creation of mesh/submesh presentation.

    """
    proxy.UpdatePipeline()
    extractCT = pvs.ExtractCellType()
    extractCT.AllGeoTypes = extractCT.GetProperty("GeoTypesInfo")[::2]
    extractCT.UpdatePipelineInformation()


def select_cells_with_data(proxy, on_points=[], on_cells=[], on_gauss=[]):
    """Select cell types with data.

    Only cell types with data for the given fields will be selected.
    If no fields defined (neither on points nor on cells) only cell
    types with data for even one field (from available) will be selected.

    """
    proxy.UpdatePipeline()
    if not hasattr(proxy, 'Entity'):
        separator = proxy.GetProperty("Separator").GetData()

        fields_info = proxy.GetProperty("FieldsTreeInfo")[::2]
        arr_name_with_dis=[elt.split("/")[-1] for elt in fields_info]

        proxy.AllArrays = []
        
        fields = []
        for name in on_gauss:
            fields.append(name+separator+'GAUSS')
        for name in on_cells:
            fields.append(name+separator+'P0')
        for name in on_points:
            fields.append(name+separator+'P1')

        field_list = []
        for name in fields:
            if arr_name_with_dis.count(name) > 0:
                index = arr_name_with_dis.index(name)
                field_list.append(fields_info[index])
                
        proxy.AllArrays = field_list
        proxy.UpdatePipeline()
        return len(field_list) != 0

    # TODO: VTN. Looks like this code is out of date.
    
    #all_cell_types = proxy.CellTypes.Available
    all_cell_types = proxy.Entity.Available
    all_arrays = list(proxy.CellArrays.GetData())
    all_arrays.extend(proxy.PointArrays.GetData())

    if not all_arrays:
        file_name = proxy.FileName.split(os.sep)[-1]
        print "Warning: " + file_name + " doesn't contain any data array."

    # List of cell types to be selected
    cell_types_on = []

    for cell_type in all_cell_types:
        #proxy.CellTypes = [cell_type]
        proxy.Entity = [cell_type]
        proxy.UpdatePipeline()

        cell_arrays = proxy.GetCellDataInformation().keys()
        point_arrays = proxy.GetPointDataInformation().keys()

        if on_points or on_cells:
            if on_points is None:
                on_points = []
            if on_cells is None:
                on_cells = []

            if (all(array in cell_arrays for array in on_cells) and
                all(array in point_arrays for array in on_points)):
                # Add cell type to the list
                cell_types_on.append(cell_type)
        else:
            in_arrays = lambda array: ((array in cell_arrays) or
                                       (array in point_arrays))
            if any(in_arrays(array) for array in all_arrays):
                cell_types_on.append(cell_type)

    # Select cell types
    #proxy.CellTypes = cell_types_on
    proxy.Entity = cell_types_on
    proxy.UpdatePipeline()

def if_possible(proxy, field_name, entity, prs_type, extrGrps=None):
    """Check if the presentation creation is possible on the given field."""
    proxy.UpdatePipeline()
    result = True
    if (prs_type == PrsTypeEnum.DEFORMEDSHAPE or
        prs_type == PrsTypeEnum.DEFORMEDSHAPESCALARMAP or
        prs_type == PrsTypeEnum.VECTORS or
        prs_type == PrsTypeEnum.STREAMLINES):
        nb_comp = get_nb_components(proxy, entity, field_name)
        result = (nb_comp > 1)
    elif (prs_type == PrsTypeEnum.GAUSSPOINTS):
        result = (entity == EntityType.CELL or
                  field_name in proxy.QuadraturePointArrays.Available)
    elif (prs_type == PrsTypeEnum.MESH):
        result = len(get_group_names(extrGrps)) > 0

    return result


def add_scalar_bar(field_name, nb_components,
                   vector_mode, lookup_table, time_value):
    """Add scalar bar with predefined properties."""
    global _current_bar

    # Construct bar title
    title = "\n".join([field_name, str(time_value)])
    if nb_components > 1:
        title = "\n".join([title, vector_mode])

    # Create scalar bar
    scalar_bar = pvs.CreateScalarBar(Enabled=1)
    scalar_bar.Orientation = 'Vertical'
    scalar_bar.Title = title
    scalar_bar.LookupTable = lookup_table

    # Set default properties same as in Post-Pro
    scalar_bar.NumberOfLabels = 5
    scalar_bar.AutomaticLabelFormat = 0
    scalar_bar.LabelFormat = '%-#6.6g'
    # Title
    scalar_bar.TitleFontFamily = 'Arial'
    scalar_bar.TitleFontSize = 8
    scalar_bar.TitleBold = 1
    scalar_bar.TitleItalic = 1
    scalar_bar.TitleShadow = 1
    # Labels
    scalar_bar.LabelFontFamily = 'Arial'
    scalar_bar.LabelFontSize = 8
    scalar_bar.LabelBold = 1
    scalar_bar.LabelItalic = 1
    scalar_bar.LabelShadow = 1

    # Add the scalar bar to the view
    pvs.GetRenderView().Representations.append(scalar_bar)

    # Reassign the current bar
    _current_bar = scalar_bar

    return scalar_bar


def get_bar():
    """Get current scalar bar."""
    global _current_bar

    return _current_bar


def get_lookup_table(field_name, nb_components, vector_mode='Magnitude'):
    """Get lookup table for the given field."""
    lookup_table = pvs.GetLookupTableForArray(field_name, nb_components)

    if vector_mode == 'Magnitude':
        lookup_table.VectorMode = vector_mode
    elif vector_mode == 'X':
        lookup_table.VectorMode = 'Component'
        lookup_table.VectorComponent = 0
    elif vector_mode == 'Y':
        lookup_table.VectorMode = 'Component'
        lookup_table.VectorComponent = 1
    elif vector_mode == 'Z':
        lookup_table.VectorMode = 'Component'
        lookup_table.VectorComponent = 2
    else:
        raise ValueError("Incorrect vector mode: " + vector_mode)

    lookup_table.Discretize = 0
    lookup_table.ColorSpace = 'HSV'
    lookup_table.LockScalarRange = 0

    return lookup_table


def get_group_mesh_name(full_group_name):
    """Return mesh name of the group by its full name."""
    aList = full_group_name.split('/')
    if len(aList) >= 2 :
        group_name = full_group_name.split('/')[1]
        return group_name


def get_group_entity(full_group_name):
    """Return entity type of the group by its full name."""
    aList = full_group_name.split('/')
    if len(aList) >= 3 :
        entity_name = full_group_name.split('/')[2]
        entity = EntityType.get_type(entity_name)
        return entity


def get_group_short_name(full_group_name):
    """Return short name of the group by its full name."""
    short_name = re.sub('^GRP_', '', full_group_name)
    return short_name


def get_mesh_full_names(proxy):
    """Return all mesh names in the given proxy as a set."""
    proxy.UpdatePipeline()
    fields = proxy.GetProperty("FieldsTreeInfo")[::2]
    mesh_full_names = set([item for item in fields if get_field_mesh_name(item) == get_field_short_name(item)])
    return mesh_full_names


def get_group_names(extrGrps):
    """Return full names of all groups of the given 'ExtractGroup' filter object.
    """
    group_names = filter(lambda x:x[:4]=="GRP_",list(extrGrps.GetProperty("GroupsFlagsInfo")[::2]))
    return group_names


def get_time(proxy, timestamp_nb):
    """Get time value by timestamp number."""
    proxy.UpdatePipeline()
    # Check timestamp number
    timestamps = []
    
    if (hasattr(proxy, 'TimestepValues')):
        timestamps = proxy.TimestepValues.GetData()
    elif (hasattr(proxy.Input, 'TimestepValues')):
        timestamps = proxy.Input.TimestepValues.GetData()

    length = len(timestamps)
    if (timestamp_nb > 0 and (timestamp_nb - 1) not in xrange(length) ) or (timestamp_nb < 0 and -timestamp_nb > length):
        raise ValueError("Timestamp number is out of range: " + str(timestamp_nb))

    # Return time value
    if timestamp_nb > 0:
        return timestamps[timestamp_nb - 1]
    else:
        return timestamps[timestamp_nb]

def create_prs(prs_type, proxy, field_entity, field_name, timestamp_nb):
    """Auxiliary function.

    Build presentation of the given type on the given field and
    timestamp number.
    Set the presentation properties like visu.CreatePrsForResult() do.

    """
    proxy.UpdatePipeline()
    prs = None

    if prs_type == PrsTypeEnum.SCALARMAP:
        prs = ScalarMapOnField(proxy, field_entity, field_name, timestamp_nb)
    elif prs_type == PrsTypeEnum.CUTPLANES:
        prs = CutPlanesOnField(proxy, field_entity, field_name, timestamp_nb,
                               orientation=Orientation.ZX)
    elif prs_type == PrsTypeEnum.CUTLINES:
        prs = CutLinesOnField(proxy, field_entity, field_name, timestamp_nb,
                              orientation1=Orientation.XY,
                              orientation2=Orientation.ZX)
    elif prs_type == PrsTypeEnum.DEFORMEDSHAPE:
        prs = DeformedShapeOnField(proxy, field_entity,
                                   field_name, timestamp_nb)
    elif prs_type == PrsTypeEnum.DEFORMEDSHAPESCALARMAP:
        prs = DeformedShapeAndScalarMapOnField(proxy, field_entity,
                                               field_name, timestamp_nb)
    elif prs_type == PrsTypeEnum.VECTORS:
        prs = VectorsOnField(proxy, field_entity, field_name, timestamp_nb)
    elif prs_type == PrsTypeEnum.PLOT3D:
        prs = Plot3DOnField(proxy, field_entity, field_name, timestamp_nb)
    elif prs_type == PrsTypeEnum.ISOSURFACES:
        prs = IsoSurfacesOnField(proxy, field_entity, field_name, timestamp_nb)
    elif prs_type == PrsTypeEnum.GAUSSPOINTS:
        prs = GaussPointsOnField(proxy, field_entity, field_name, timestamp_nb)
    elif prs_type == PrsTypeEnum.STREAMLINES:
        prs = StreamLinesOnField(proxy, field_entity, field_name, timestamp_nb)
    else:
        raise ValueError("Unexistent presentation type.")

    return prs


# Functions for building Post-Pro presentations
def ScalarMapOnField(proxy, entity, field_name, timestamp_nb,
                     vector_mode='Magnitude'):
    """Creates Scalar Map presentation on the given field.

    Arguments:
      proxy: the pipeline object, containig data
      entity: the entity type from PrsTypeEnum
      field_name: the field name
      timestamp_nb: the number of time step (1, 2, ...)
      vector_mode: the mode of transformation of vector values
      into scalar values, applicable only if the field contains vector values.
      Possible modes: 'Magnitude', 'X', 'Y' or 'Z'.

    Returns:
      Scalar Map as representation object.

    """
    proxy.UpdatePipeline()
    # We don't need mesh parts with no data on them
    if entity == EntityType.NODE:
        select_cells_with_data(proxy, on_points=[field_name])
    else:
        select_cells_with_data(proxy, on_cells=[field_name])

    # Check vector mode
    nb_components = get_nb_components(proxy, entity, field_name)
    check_vector_mode(vector_mode, nb_components)

    # Get time value
    time_value = get_time(proxy, timestamp_nb)

    # Set timestamp
    pvs.GetRenderView().ViewTime = time_value
    pvs.UpdatePipeline(time_value, proxy)

    # Get Scalar Map representation object
    scalarmap = pvs.GetRepresentation(proxy)

    # Get lookup table
    lookup_table = get_lookup_table(field_name, nb_components, vector_mode)

    # Set field range if necessary
    data_range = get_data_range(proxy, entity,
                                field_name, vector_mode)
    lookup_table.LockScalarRange = 1
    lookup_table.RGBPoints = [data_range[0], 0, 0, 1, data_range[1], 1, 0, 0]
    # Set properties
    scalarmap.ColorAttributeType = EntityType.get_pvtype(entity)
    scalarmap.ColorArrayName = field_name
    scalarmap.LookupTable = lookup_table

    # Add scalar bar
    bar_title = field_name + ", " + str(time_value)
    if (nb_components > 1):
        bar_title += "\n" + vector_mode
    add_scalar_bar(field_name, nb_components, vector_mode,
                   lookup_table, time_value)

    return scalarmap


def CutPlanesOnField(proxy, entity, field_name, timestamp_nb,
                     nb_planes=10, orientation=Orientation.YZ,
                     angle1=0, angle2=0,
                     displacement=0.5, vector_mode='Magnitude'):
    """Creates Cut Planes presentation on the given field.

    Arguments:
      proxy: the pipeline object, containig data
      entity: the entity type from PrsTypeEnum
      field_name: the field name
      timestamp_nb: the number of time step (1, 2, ...)
      nb_planes: number of cutting planes
      orientation: cutting planes orientation in 3D space
      angle1: rotation of the planes in 3d space around the first axis of the
      selected orientation (X axis for XY, Y axis for YZ, Z axis for ZX).
      The angle of rotation is set in degrees. Acceptable range: [-45, 45].
      angle2: rotation of the planes in 3d space around the second axis of the
      selected orientation. Acceptable range: [-45, 45].
      displacement: the displacement of the planes into one or another side
      vector_mode: the mode of transformation of vector values
      into scalar values, applicable only if the field contains vector values.
      Possible modes: 'Magnitude', 'X', 'Y' or 'Z'.

    Returns:
      Cut Planes as representation object.

    """
    proxy.UpdatePipeline()
    if entity == EntityType.NODE:
        select_cells_with_data(proxy, on_points=[field_name])
    else:
        select_cells_with_data(proxy, on_cells=[field_name])

    # Check vector mode
    nb_components = get_nb_components(proxy, entity, field_name)
    check_vector_mode(vector_mode, nb_components)

    # Get time value
    time_value = get_time(proxy, timestamp_nb)

    # Set timestamp
    pvs.GetRenderView().ViewTime = time_value
    pvs.UpdatePipeline(time_value, proxy)

    # Create slice filter
    slice_filter = pvs.Slice(proxy)
    slice_filter.SliceType = "Plane"

    # Set cut planes normal
    normal = get_normal_by_orientation(orientation,
                                       radians(angle1), radians(angle2))
    slice_filter.SliceType.Normal = normal

    # Set cut planes positions
    positions = get_positions(nb_planes, normal,
                              get_bounds(proxy), displacement)
    slice_filter.SliceOffsetValues = positions

    # Get Cut Planes representation object
    cut_planes = pvs.GetRepresentation(slice_filter)

    # Get lookup table
    lookup_table = get_lookup_table(field_name, nb_components, vector_mode)

    # Set field range if necessary
    data_range = get_data_range(proxy, entity,
                                field_name, vector_mode)
    lookup_table.LockScalarRange = 1
    lookup_table.RGBPoints = [data_range[0], 0, 0, 1, data_range[1], 1, 0, 0]

    # Set properties
    cut_planes.ColorAttributeType = EntityType.get_pvtype(entity)
    cut_planes.ColorArrayName = field_name
    cut_planes.LookupTable = lookup_table

    # Add scalar bar
    add_scalar_bar(field_name, nb_components,
                   vector_mode, lookup_table, time_value)

    return cut_planes


def CutLinesOnField(proxy, entity, field_name, timestamp_nb,
                    nb_lines=10,
                    orientation1=Orientation.XY,
                    base_angle1=0, base_angle2=0,
                    orientation2=Orientation.YZ,
                    cut_angle1=0, cut_angle2=0,
                    displacement1=0.5, displacement2=0.5,
                    generate_curves=False,
                    vector_mode='Magnitude'):
    """Creates Cut Lines presentation on the given field.

    Arguments:
      proxy: the pipeline object, containig data
      entity: the entity type from PrsTypeEnum
      field_name: the field name
      timestamp_nb: the number of time step (1, 2, ...)
      nb_lines: number of lines
      orientation1: base plane orientation in 3D space
      base_angle1: rotation of the base plane in 3d space around the first
      axis of the orientation1 (X axis for XY, Y axis for YZ, Z axis for ZX).
      The angle of rotation is set in degrees. Acceptable range: [-45, 45].
      base_angle2: rotation of the base plane in 3d space around the second
      axis of the orientation1. Acceptable range: [-45, 45].
      orientation2: cutting planes orientation in 3D space
      cut_angle1: rotation of the cut planes in 3d space around the first
      axis of the orientation2. Acceptable range: [-45, 45].
      cut_angle2: rotation of the cuting planes in 3d space around the second
      axis of the orientation2. Acceptable range: [-45, 45].
      displacement1: base plane displacement
      displacement2: cutting planes displacement
      generate_curves: if true, 'PlotOverLine' filter will be created
      for each cut line
      vector_mode: the mode of transformation of vector values
      into scalar values, applicable only if the field contains vector values.
      Possible modes: 'Magnitude', 'X', 'Y' or 'Z'.

    Returns:
      Cut Lines as representation object if generate_curves == False,
      (Cut Lines as representation object, list of 'PlotOverLine') otherwise

    """
    proxy.UpdatePipeline()
    if entity == EntityType.NODE:
        select_cells_with_data(proxy, on_points=[field_name])
    else:
        select_cells_with_data(proxy, on_cells=[field_name])

    # Check vector mode
    nb_components = get_nb_components(proxy, entity, field_name)
    check_vector_mode(vector_mode, nb_components)

    # Get time value
    time_value = get_time(proxy, timestamp_nb)

    # Set timestamp
    pvs.GetRenderView().ViewTime = time_value
    pvs.UpdatePipeline(time_value, proxy)

    # Create base plane
    base_plane = pvs.Slice(proxy)
    base_plane.SliceType = "Plane"

    # Set base plane normal
    base_normal = get_normal_by_orientation(orientation1,
                                            radians(base_angle1),
                                            radians(base_angle2))
    base_plane.SliceType.Normal = base_normal

    # Set base plane position
    base_position = get_positions(1, base_normal,
                                  get_bounds(proxy), displacement1)
    base_plane.SliceOffsetValues = base_position

    # Check base plane
    base_plane.UpdatePipeline()
    if (base_plane.GetDataInformation().GetNumberOfCells() == 0):
        base_plane = proxy

    # Create cutting planes
    cut_planes = pvs.Slice(base_plane)
    cut_planes.SliceType = "Plane"

    # Set cutting planes normal and get positions
    cut_normal = get_normal_by_orientation(orientation2,
                                           radians(cut_angle1),
                                           radians(cut_angle2))
    cut_planes.SliceType.Normal = cut_normal

    # Set cutting planes position
    cut_positions = get_positions(nb_lines, cut_normal,
                                  get_bounds(base_plane), displacement2)

    # Generate curves
    curves = []
    if generate_curves:
        index = 0
        for pos in cut_positions:
            # Get points for plot over line objects
            cut_planes.SliceOffsetValues = pos
            cut_planes.UpdatePipeline()
            bounds = get_bounds(cut_planes)
            point1 = [bounds[0], bounds[2], bounds[4]]
            point2 = [bounds[1], bounds[3], bounds[5]]

            # Create plot over line filter
            pol = pvs.PlotOverLine(cut_planes,
                                  Source="High Resolution Line Source")
            pvs.RenameSource('Y' + str(index), pol)
            pol.Source.Point1 = point1
            pol.Source.Point2 = point2
            pol.UpdatePipeline()
            curves.append(pol)

            index += 1

    cut_planes.SliceOffsetValues = cut_positions
    cut_planes.UpdatePipeline()

    # Get Cut Lines representation object
    cut_lines = pvs.GetRepresentation(cut_planes)

    # Get lookup table
    lookup_table = get_lookup_table(field_name, nb_components, vector_mode)

    # Set field range if necessary
    data_range = get_data_range(proxy, entity,
                                field_name, vector_mode)
    lookup_table.LockScalarRange = 1
    lookup_table.RGBPoints = [data_range[0], 0, 0, 1, data_range[1], 1, 0, 0]

    # Set properties
    cut_lines.ColorAttributeType = EntityType.get_pvtype(entity)
    cut_lines.ColorArrayName = field_name
    cut_lines.LookupTable = lookup_table

    # Set wireframe represenatation mode
    cut_lines.Representation = 'Wireframe'

    # Add scalar bar
    add_scalar_bar(field_name, nb_components,
                   vector_mode, lookup_table, time_value)

    result = cut_lines
    # If curves were generated return tuple (cut lines, list of curves)
    if curves:
        result = cut_lines, curves

    return result


def CutSegmentOnField(proxy, entity, field_name, timestamp_nb,
                      point1, point2, vector_mode='Magnitude'):
    """Creates Cut Segment presentation on the given field.

    Arguments:
      proxy: the pipeline object, containig data
      entity: the entity type from PrsTypeEnum
      field_name: the field name
      timestamp_nb: the number of time step (1, 2, ...)
      point1: set the first point of the segment (as [x, y, z])
      point1: set the second point of the segment (as [x, y, z])
      vector_mode: the mode of transformation of vector values
      into scalar values, applicable only if the field contains vector values.
      Possible modes: 'Magnitude', 'X', 'Y' or 'Z'.

    Returns:
      Cut Segment as 3D representation object.

    """
    proxy.UpdatePipeline()
    if entity == EntityType.NODE:
        select_cells_with_data(proxy, on_points=[field_name])
    else:
        select_cells_with_data(proxy, on_cells=[field_name])

    # Check vector mode
    nb_components = get_nb_components(proxy, entity, field_name)
    check_vector_mode(vector_mode, nb_components)

    # Get time value
    time_value = get_time(proxy, timestamp_nb)

    # Set timestamp
    pvs.GetRenderView().ViewTime = time_value
    pvs.UpdatePipeline(time_value, proxy)

    # Create plot over line filter
    pol = pvs.PlotOverLine(proxy, Source="High Resolution Line Source")
    pol.Source.Point1 = point1
    pol.Source.Point2 = point2
    pol.UpdatePipeline()

    # Get Cut Segment representation object
    cut_segment = pvs.GetRepresentation(pol)

    # Get lookup table
    lookup_table = get_lookup_table(field_name, nb_components, vector_mode)

    # Set field range if necessary
    data_range = get_data_range(proxy, entity,
                                field_name, vector_mode)
    lookup_table.LockScalarRange = 1
    lookup_table.RGBPoints = [data_range[0], 0, 0, 1, data_range[1], 1, 0, 0]

    # Set properties
    cut_segment.ColorAttributeType = EntityType.get_pvtype(entity)
    cut_segment.ColorArrayName = field_name
    cut_segment.LookupTable = lookup_table

    # Set wireframe represenatation mode
    cut_segment.Representation = 'Wireframe'

    # Add scalar bar
    add_scalar_bar(field_name, nb_components,
                   vector_mode, lookup_table, time_value)

    return cut_segment


def VectorsOnField(proxy, entity, field_name, timestamp_nb,
                   scale_factor=None,
                   glyph_pos=GlyphPos.TAIL, glyph_type='2D Glyph',
                   is_colored=False, vector_mode='Magnitude'):
    """Creates Vectors presentation on the given field.

    Arguments:
      proxy: the pipeline object, containig data
      entity: the entity type from PrsTypeEnum
      field_name: the field name
      timestamp_nb: the number of time step (1, 2, ...)
      scale_factor: scale factor
      glyph_pos: the position of glyphs
      glyph_type: the type of glyphs
      is_colored: this option allows to color the presentation according to
      the corresponding data array values
      vector_mode: the mode of transformation of vector values
      into scalar values, applicable only if the field contains vector values.
      Possible modes: 'Magnitude', 'X', 'Y' or 'Z'.

    Returns:
      Vectors as representation object.

    """
    proxy.UpdatePipeline()
    if entity == EntityType.NODE:
        select_cells_with_data(proxy, on_points=[field_name])
    else:
        select_cells_with_data(proxy, on_cells=[field_name])

    # Check vector mode
    nb_components = get_nb_components(proxy, entity, field_name)
    check_vector_mode(vector_mode, nb_components)

    # Get time value
    time_value = get_time(proxy, timestamp_nb)

    # Set timestamp
    pvs.GetRenderView().ViewTime = time_value
    pvs.UpdatePipeline(time_value, proxy)

    # Extract only groups with data for the field
    source = proxy

    # Cell centers
    if is_data_on_cells(proxy, field_name):
        cell_centers = pvs.CellCenters(source)
        cell_centers.VertexCells = 1
        source = cell_centers

    vector_array = field_name
    # If the given vector array has only 2 components, add the third one
    if nb_components == 2:
        calc = get_add_component_calc(source, EntityType.NODE, field_name)
        vector_array = calc.ResultArrayName
        source = calc

    # Glyph
    glyph = pvs.Glyph(source)
    glyph.Vectors = vector_array
    glyph.ScaleMode = 'vector'
    glyph.MaskPoints = 0

    # Set glyph type
    glyph.GlyphType = glyph_type
    if glyph_type == '2D Glyph':
        glyph.GlyphType.GlyphType = 'Arrow'
    elif glyph_type == 'Cone':
        glyph.GlyphType.Resolution = 7
        glyph.GlyphType.Height = 2
        glyph.GlyphType.Radius = 0.2

    # Set glyph position if possible
    if glyph.GlyphType.GetProperty("Center"):
        if (glyph_pos == GlyphPos.TAIL):
            glyph.GlyphType.Center = [0.5, 0.0, 0.0]
        elif (glyph_pos == GlyphPos.HEAD):
            glyph.GlyphType.Center = [-0.5, 0.0, 0.0]
        elif (glyph_pos == GlyphPos.CENTER):
            glyph.GlyphType.Center = [0.0, 0.0, 0.0]

    if scale_factor is not None:
        glyph.SetScaleFactor = scale_factor
    else:
        def_scale = get_default_scale(PrsTypeEnum.DEFORMEDSHAPE,
                                      proxy, entity, field_name)
        glyph.SetScaleFactor = def_scale

    glyph.UpdatePipeline()

    # Get Vectors representation object
    vectors = pvs.GetRepresentation(glyph)

    # Get lookup table
    lookup_table = get_lookup_table(field_name, nb_components, vector_mode)

    # Set field range if necessary
    data_range = get_data_range(proxy, entity,
                                field_name, vector_mode)
    lookup_table.LockScalarRange = 1
    lookup_table.RGBPoints = [data_range[0], 0, 0, 1, data_range[1], 1, 0, 0]

    # Set properties
    if (is_colored):
        vectors.ColorArrayName = 'GlyphVector'
    else:
        vectors.ColorArrayName = ''
    vectors.LookupTable = lookup_table

    vectors.LineWidth = 1.0

    # Set wireframe represenatation mode
    vectors.Representation = 'Wireframe'

    # Add scalar bar
    add_scalar_bar(field_name, nb_components,
                   vector_mode, lookup_table, time_value)

    return vectors


def DeformedShapeOnField(proxy, entity, field_name,
                         timestamp_nb,
                         scale_factor=None, is_colored=False,
                         vector_mode='Magnitude'):
    """Creates Defromed Shape presentation on the given field.

    Arguments:
      proxy: the pipeline object, containig data
      entity: the entity type from PrsTypeEnum
      field_name: the field name
      timestamp_nb: the number of time step (1, 2, ...)
      scale_factor: scale factor of the deformation
      is_colored: this option allows to color the presentation according to
      the corresponding data array values
      vector_mode: the mode of transformation of vector values
      into scalar values, applicable only if the field contains vector values.
      Possible modes: 'Magnitude', 'X', 'Y' or 'Z'.

    Returns:
      Defromed Shape as representation object.

    """
    proxy.UpdatePipeline()
    # We don't need mesh parts with no data on them
    if entity == EntityType.NODE:
        select_cells_with_data(proxy, on_points=[field_name])
    else:
        select_cells_with_data(proxy, on_cells=[field_name])

    # Check vector mode
    nb_components = get_nb_components(proxy, entity, field_name)
    check_vector_mode(vector_mode, nb_components)

    # Get time value
    time_value = get_time(proxy, timestamp_nb)

    # Set timestamp
    pvs.GetRenderView().ViewTime = time_value
    pvs.UpdatePipeline(time_value, proxy)

    # Do merge
    source = pvs.MergeBlocks(proxy)

    # Cell data to point data
    if is_data_on_cells(proxy, field_name):
        cell_to_point = pvs.CellDatatoPointData()
        cell_to_point.PassCellData = 1
        source = cell_to_point

    vector_array = field_name
    # If the given vector array has only 2 components, add the third one
    if nb_components == 2:
        calc = get_add_component_calc(source, EntityType.NODE, field_name)
        vector_array = calc.ResultArrayName
        source = calc

    # Warp by vector
    warp_vector = pvs.WarpByVector(source)
    warp_vector.Vectors = [vector_array]
    if scale_factor is not None:
        warp_vector.ScaleFactor = scale_factor
    else:
        def_scale = get_default_scale(PrsTypeEnum.DEFORMEDSHAPE,
                                      proxy, entity, field_name)
        warp_vector.ScaleFactor = def_scale

    # Get Deformed Shape representation object
    defshape = pvs.GetRepresentation(warp_vector)

    # Get lookup table
    lookup_table = get_lookup_table(field_name, nb_components, vector_mode)

    # Set field range if necessary
    data_range = get_data_range(proxy, entity,
                                field_name, vector_mode)
    lookup_table.LockScalarRange = 1
    lookup_table.RGBPoints = [data_range[0], 0, 0, 1, data_range[1], 1, 0, 0]

    # Set properties
    if is_colored:
        defshape.ColorAttributeType = EntityType.get_pvtype(entity)
        defshape.ColorArrayName = field_name
    else:
        defshape.ColorArrayName = ''
    defshape.LookupTable = lookup_table

    # Set wireframe represenatation mode
    defshape.Representation = 'Wireframe'

    # Add scalar bar
    add_scalar_bar(field_name, nb_components,
                   vector_mode, lookup_table, time_value)

    return defshape


def DeformedShapeAndScalarMapOnField(proxy, entity, field_name,
                                     timestamp_nb,
                                     scale_factor=None,
                                     scalar_entity=None,
                                     scalar_field_name=None,
                                     vector_mode='Magnitude'):
    """Creates Defromed Shape And Scalar Map presentation on the given field.

    Arguments:
      proxy: the pipeline object, containig data
      entity: the entity type from PrsTypeEnum
      field_name: the field name
      timestamp_nb: the number of time step (1, 2, ...)
      scale_factor: scale factor of the deformation
      scalar_entity: scalar field entity
      scalar_field_name: scalar field, i.e. the field for coloring
      vector_mode: the mode of transformation of vector values
      into scalar values, applicable only if the field contains vector values.
      Possible modes: 'Magnitude', 'X', 'Y' or 'Z'.

    Returns:
      Defromed Shape And Scalar Map as representation object.

    """
    proxy.UpdatePipeline()
    # We don't need mesh parts with no data on them
    on_points = []
    on_cells = []

    if entity == EntityType.NODE:
        on_points.append(field_name)
    else:
        on_cells.append(field_name)

    if scalar_entity and scalar_field_name:
        if scalar_entity == EntityType.NODE:
            on_points.append(scalar_field_name)
        else:
            on_cells.append(scalar_field_name)

    select_cells_with_data(proxy, on_points, on_cells)

    # Check vector mode
    nb_components = get_nb_components(proxy, entity, field_name)
    check_vector_mode(vector_mode, nb_components)

    # Get time value
    time_value = get_time(proxy, timestamp_nb)

    # Set timestamp
    pvs.GetRenderView().ViewTime = time_value
    pvs.UpdatePipeline(time_value, proxy)

    # Set scalar field by default
    scalar_field_entity = scalar_entity
    scalar_field = scalar_field_name
    if (scalar_field_entity is None) or (scalar_field is None):
        scalar_field_entity = entity
        scalar_field = field_name

    # Do merge
    source = pvs.MergeBlocks(proxy)

    # Cell data to point data
    if is_data_on_cells(proxy, field_name):
        cell_to_point = pvs.CellDatatoPointData(source)
        cell_to_point.PassCellData = 1
        source = cell_to_point

    vector_array = field_name
    # If the given vector array has only 2 components, add the third one
    if nb_components == 2:
        calc = get_add_component_calc(source, EntityType.NODE, field_name)
        vector_array = calc.ResultArrayName
        source = calc

    # Warp by vector
    warp_vector = pvs.WarpByVector(source)
    warp_vector.Vectors = [vector_array]
    if scale_factor is not None:
        warp_vector.ScaleFactor = scale_factor
    else:
        def_scale = get_default_scale(PrsTypeEnum.DEFORMEDSHAPE,
                                      proxy, entity, field_name)
        warp_vector.ScaleFactor = def_scale

    # Get Defromed Shape And Scalar Map representation object
    defshapemap = pvs.GetRepresentation(warp_vector)

    # Get lookup table
    lookup_table = get_lookup_table(scalar_field, nb_components, vector_mode)

    # Set field range if necessary
    data_range = get_data_range(proxy, scalar_field_entity,
                                scalar_field, vector_mode)
    lookup_table.LockScalarRange = 1
    lookup_table.RGBPoints = [data_range[0], 0, 0, 1, data_range[1], 1, 0, 0]

    # Set properties
    defshapemap.ColorArrayName = scalar_field
    defshapemap.LookupTable = lookup_table
    defshapemap.ColorAttributeType = EntityType.get_pvtype(scalar_field_entity)

    # Add scalar bar
    add_scalar_bar(field_name, nb_components,
                   vector_mode, lookup_table, time_value)

    return defshapemap


def Plot3DOnField(proxy, entity, field_name, timestamp_nb,
                  orientation=Orientation.AUTO,
                  angle1=0, angle2=0,
                  position=0.5, is_relative=True,
                  scale_factor=None,
                  is_contour=False, nb_contours=32,
                  vector_mode='Magnitude'):
    """Creates Plot 3D presentation on the given field.

    Arguments:
      proxy: the pipeline object, containig data
      entity: the entity type from PrsTypeEnum
      field_name: the field name
      timestamp_nb: the number of time step (1, 2, ...)
      orientation: the cut plane plane orientation in 3D space, if
      the input is planar - will not be taken into account
      angle1: rotation of the cut plane in 3d space around the first axis
      of the selected orientation (X axis for XY, Y axis for YZ,
      Z axis for ZX).
      The angle of rotation is set in degrees. Acceptable range: [-45, 45].
      angle2: rotation of the cut plane in 3d space around the second axis
      of the selected orientation. Acceptable range: [-45, 45].
      position: position of the cut plane in the object (ranging from 0 to 1).
      The value 0.5 corresponds to cutting by halves.
      is_relative: defines if the cut plane position is relative or absolute
      scale_factor: deformation scale factor
      is_contour: if True - Plot 3D will be represented with a set of contours,
      otherwise - Plot 3D will be represented with a smooth surface
      nb_contours: number of contours, applied if is_contour is True
      vector_mode: the mode of transformation of vector values
      into scalar values, applicable only if the field contains vector values.
      Possible modes: 'Magnitude', 'X', 'Y' or 'Z'.

    Returns:
      Plot 3D as representation object.

    """
    proxy.UpdatePipeline()
    # We don't need mesh parts with no data on them
    if entity == EntityType.NODE:
        select_cells_with_data(proxy, on_points=[field_name])
    else:
        select_cells_with_data(proxy, on_cells=[field_name])

    # Check vector mode
    nb_components = get_nb_components(proxy, entity, field_name)
    check_vector_mode(vector_mode, nb_components)

    # Get time value
    time_value = get_time(proxy, timestamp_nb)

    # Set timestamp
    pvs.GetRenderView().ViewTime = time_value
    pvs.UpdatePipeline(time_value, proxy)

    # Do merge
    merge_blocks = pvs.MergeBlocks(proxy)
    merge_blocks.UpdatePipeline()

    poly_data = None

    # Cutting plane

    # Define orientation if necessary (auto mode)
    plane_orientation = orientation
    if (orientation == Orientation.AUTO):
        plane_orientation = get_orientation(proxy)

    # Get cutting plane normal
    normal = None

    if (not is_planar_input(proxy)):
        normal = get_normal_by_orientation(plane_orientation,
                                           radians(angle1), radians(angle2))

        # Create slice filter
        slice_filter = pvs.Slice(merge_blocks)
        slice_filter.SliceType = "Plane"

        # Set cutting plane normal
        slice_filter.SliceType.Normal = normal

        # Set cutting plane position
        if (is_relative):
            base_position = get_positions(1, normal,
                                          get_bounds(proxy), position)
            slice_filter.SliceOffsetValues = base_position
        else:
            slice_filter.SliceOffsetValues = position

        slice_filter.UpdatePipeline()
        poly_data = slice_filter
    else:
        normal = get_normal_by_orientation(plane_orientation, 0, 0)

    use_normal = 0
    # Geometry filter
    if not poly_data or poly_data.GetDataInformation().GetNumberOfCells() == 0:
        geometry_filter = pvs.GeometryFilter(merge_blocks)
        poly_data = geometry_filter
        use_normal = 1  # TODO(MZN): workaround

    warp_scalar = None
    plot3d = None
    source = poly_data

    if is_data_on_cells(poly_data, field_name):
        # Cell data to point data
        cell_to_point = pvs.CellDatatoPointData(poly_data)
        cell_to_point.PassCellData = 1
        source = cell_to_point

    scalars = ['POINTS', field_name]

    # Transform vector array to scalar array if necessary
    if (nb_components > 1):
        calc = get_calc_magnitude(source, EntityType.NODE, field_name)
        scalars = ['POINTS', calc.ResultArrayName]
        source = calc

    # Warp by scalar
    warp_scalar = pvs.WarpByScalar(source)
    warp_scalar.Scalars = scalars
    warp_scalar.Normal = normal
    warp_scalar.UseNormal = use_normal
    if scale_factor is not None:
        warp_scalar.ScaleFactor = scale_factor
    else:
        def_scale = get_default_scale(PrsTypeEnum.PLOT3D,
                                      proxy, entity, field_name)
        warp_scalar.ScaleFactor = def_scale

    warp_scalar.UpdatePipeline()
    source = warp_scalar

    if (is_contour):
        # Contours
        contour = pvs.Contour(warp_scalar)
        contour.PointMergeMethod = "Uniform Binning"
        contour.ContourBy = ['POINTS', field_name]
        scalar_range = get_data_range(proxy, entity,
                                      field_name, vector_mode)
        contour.Isosurfaces = get_contours(scalar_range, nb_contours)
        contour.UpdatePipeline()
        source = contour

    # Get Plot 3D representation object
    plot3d = pvs.GetRepresentation(source)

    # Get lookup table
    lookup_table = get_lookup_table(field_name, nb_components, vector_mode)

    # Set field range if necessary
    data_range = get_data_range(proxy, entity,
                                field_name, vector_mode)
    lookup_table.LockScalarRange = 1
    lookup_table.RGBPoints = [data_range[0], 0, 0, 1, data_range[1], 1, 0, 0]

    # Set properties
    plot3d.ColorAttributeType = EntityType.get_pvtype(entity)
    plot3d.ColorArrayName = field_name
    plot3d.LookupTable = lookup_table

    # Add scalar bar
    add_scalar_bar(field_name, nb_components,
                   vector_mode, lookup_table, time_value)

    return plot3d


def IsoSurfacesOnField(proxy, entity, field_name, timestamp_nb,
                       custom_range=None, nb_surfaces=10,
                       is_colored=True, color=None, vector_mode='Magnitude'):
    """Creates Iso Surfaces presentation on the given field.

    Arguments:
      proxy: the pipeline object, containig data
      entity: the entity type from PrsTypeEnum
      field_name: the field name
      timestamp_nb: the number of time step (1, 2, ...)
      custom_range: scalar range, if undefined the source range will be applied
      nb_surfaces: number of surfaces, which will be generated
      is_colored: this option allows to color the presentation according to
      the corresponding data array values. If False - the presentation will
      be one-coloured.
      color: defines the presentation color as [R, G, B] triple. Taken into
      account only if is_colored is False.
      vector_mode: the mode of transformation of vector values
      into scalar values, applicable only if the field contains vector values.
      Possible modes: 'Magnitude', 'X', 'Y' or 'Z'.

    Returns:
      Iso Surfaces as representation object.

    """
    proxy.UpdatePipeline()
    # We don't need mesh parts with no data on them
    if entity == EntityType.NODE:
        select_cells_with_data(proxy, on_points=[field_name])
    else:
        select_cells_with_data(proxy, on_cells=[field_name])

    # Check vector mode
    nb_components = get_nb_components(proxy, entity, field_name)
    check_vector_mode(vector_mode, nb_components)

    # Get time value
    time_value = get_time(proxy, timestamp_nb)

    # Set timestamp
    pvs.GetRenderView().ViewTime = time_value
    pvs.UpdatePipeline(time_value, proxy)

    # Do merge
    source = pvs.MergeBlocks(proxy)

    # Transform cell data into point data if necessary
    if is_data_on_cells(proxy, field_name):
        cell_to_point = pvs.CellDatatoPointData(source)
        cell_to_point.PassCellData = 1
        source = cell_to_point

    contour_by = ['POINTS', field_name]

    # Transform vector array to scalar array if necessary
    if (nb_components > 1):
        calc = get_calc_magnitude(source, EntityType.NODE, field_name)
        contour_by = ['POINTS', calc.ResultArrayName]
        source = calc

    # Contour filter settings
    contour = pvs.Contour(source)
    contour.ComputeScalars = 1
    contour.ContourBy = contour_by

    # Specify the range
    scalar_range = custom_range
    if (scalar_range is None):
        scalar_range = get_data_range(proxy, entity,
                                      field_name, cut_off=True)

    # Get contour values for the range
    surfaces = get_contours(scalar_range, nb_surfaces)

    # Set contour values
    contour.Isosurfaces = surfaces

    # Get Iso Surfaces representation object
    isosurfaces = pvs.GetRepresentation(contour)

    # Get lookup table
    lookup_table = get_lookup_table(field_name, nb_components, vector_mode)

    # Set field range if necessary
    data_range = get_data_range(proxy, entity,
                                field_name, vector_mode)
    lookup_table.LockScalarRange = 1
    lookup_table.RGBPoints = [data_range[0], 0, 0, 1, data_range[1], 1, 0, 0]

    # Set display properties
    if (is_colored):
        isosurfaces.ColorAttributeType = EntityType.get_pvtype(entity)
        isosurfaces.ColorArrayName = field_name
    else:
        isosurfaces.ColorArrayName = ''
        if color:
            isosurfaces.DiffuseColor = color
    isosurfaces.LookupTable = lookup_table

    # Add scalar bar
    add_scalar_bar(field_name, nb_components,
                   vector_mode, lookup_table, time_value)

    return isosurfaces


def GaussPointsOnField(proxy, entity, field_name,
                       timestamp_nb,
                       is_deformed=True, scale_factor=None,
                       is_colored=True, color=None,
                       primitive=GaussType.SPRITE,
                       is_proportional=True,
                       max_pixel_size=256,
                       multiplier=None, vector_mode='Magnitude'):
    """Creates Gauss Points on the given field.

    Arguments:

    proxy: the pipeline object, containig data
    entity: the field entity type from PrsTypeEnum
    field_name: the field name
    timestamp_nb: the number of time step (1, 2, ...)
    is_deformed: defines whether the Gauss Points will be deformed or not
    scale_factor -- the scale factor for deformation. Will be taken into
    account only if is_deformed is True.
    If not passed by user, default scale will be computed.
    is_colored -- defines whether the Gauss Points will be multicolored,
    using the corresponding data values
    color: defines the presentation color as [R, G, B] triple. Taken into
    account only if is_colored is False.
    primitive: primitive type from GaussType
    is_proportional: if True, the size of primitives will depends on
    the gauss point value
    max_pixel_size: the maximum sizr of the Gauss Points primitive in pixels
    multiplier: coefficient between data values and the size of primitives
    If not passed by user, default scale will be computed.
    vector_mode: the mode of transformation of vector values into
    scalar values, applicable only if the field contains vector values.
    Possible modes: 'Magnitude' - vector module;
    'X', 'Y', 'Z' - vector components.

    Returns:
      Gauss Points as representation object.

    """
    proxy.UpdatePipeline()
    # We don't need mesh parts with no data on them
    on_gauss = select_cells_with_data(proxy, on_gauss=[field_name])
    if not on_gauss:
        if entity == EntityType.NODE:
            select_cells_with_data(proxy, on_points=[field_name])
        else:
            select_cells_with_data(proxy, on_cells=[field_name])

    # Check vector mode
    nb_components = get_nb_components(proxy, entity, field_name)
    check_vector_mode(vector_mode, nb_components)

    # Get time value
    time_value = get_time(proxy, timestamp_nb)

    # Set timestamp
    pvs.GetRenderView().ViewTime = time_value
    proxy.UpdatePipeline(time=time_value)

    source = proxy

    # If no quadrature point array is passed, use cell centers
    if on_gauss:
        generate_qp = pvs.GenerateQuadraturePoints(source)
        generate_qp.QuadratureSchemeDef = ['CELLS', 'ELGA@0']
        source = generate_qp
    else:
        # Cell centers
        cell_centers = pvs.CellCenters(source)
        cell_centers.VertexCells = 1
        source = cell_centers

    source.UpdatePipeline()

    # Check if deformation enabled
    if is_deformed and nb_components > 1:
        vector_array = field_name
        # If the given vector array has only 2 components, add the third one
        if nb_components == 2:
            calc = get_add_component_calc(source,
                                          EntityType.NODE, field_name)
            vector_array = calc.ResultArrayName
            source = calc

        # Warp by vector
        warp_vector = pvs.WarpByVector(source)
        warp_vector.Vectors = [vector_array]
        if scale_factor is not None:
            warp_vector.ScaleFactor = scale_factor
        else:
            def_scale = get_default_scale(PrsTypeEnum.DEFORMEDSHAPE, proxy,
                                          entity, field_name)
            warp_vector.ScaleFactor = def_scale
        warp_vector.UpdatePipeline()
        source = warp_vector

    # Get Gauss Points representation object
    gausspnt = pvs.GetRepresentation(source)

    # Get lookup table
    lookup_table = get_lookup_table(field_name, nb_components, vector_mode)

    # Set field range if necessary
    data_range = get_data_range(proxy, entity,
                                field_name, vector_mode)
    lookup_table.LockScalarRange = 1
    lookup_table.RGBPoints = [data_range[0], 0, 0, 1, data_range[1], 1, 0, 0]

    # Set display properties
    if is_colored:
        gausspnt.ColorAttributeType = EntityType.get_pvtype(entity)
        gausspnt.ColorArrayName = field_name
    else:
        gausspnt.ColorArrayName = ''
        if color:
            gausspnt.DiffuseColor = color

    gausspnt.LookupTable = lookup_table

    # Add scalar bar
    add_scalar_bar(field_name, nb_components,
                   vector_mode, lookup_table, time_value)

    # Set point sprite representation
    gausspnt.Representation = 'Point Sprite'

    # Point sprite settings
    gausspnt.InterpolateScalarsBeforeMapping = 0
    gausspnt.MaxPixelSize = max_pixel_size

    # Render mode
    gausspnt.RenderMode = GaussType.get_mode(primitive)

    #if primitive == GaussType.SPRITE:
        # Set texture
        # TODO(MZN): replace with pvsimple high-level interface
    #    texture = sm.CreateProxy("textures", "SpriteTexture")
    #    alphamprop = texture.GetProperty("AlphaMethod")
    #    alphamprop.SetElement(0, 2)  # Clamp
    #    alphatprop = texture.GetProperty("AlphaThreshold")
    #    alphatprop.SetElement(0, 63)
    #    maxprop = texture.GetProperty("Maximum")
    #    maxprop.SetElement(0, 255)
    #    texture.UpdateVTKObjects()

    #    gausspnt.Texture = texture
        #gausspnt.Texture.AlphaMethod = 'Clamp'
        #gausspnt.Texture.AlphaThreshold = 63
        #gausspnt.Texture.Maximum= 255

    # Proportional radius
    gausspnt.RadiusUseScalarRange = 0
    gausspnt.RadiusIsProportional = 0

    if is_proportional:
        mult = multiplier
        if mult is None:
            mult = abs(0.1 / data_range[1])

        gausspnt.RadiusScalarRange = data_range
        gausspnt.RadiusTransferFunctionEnabled = 1
        gausspnt.RadiusMode = 'Scalar'
        gausspnt.RadiusArray = ['POINTS', field_name]
        if nb_components > 1:
            v_comp = get_vector_component(vector_mode)
            gausspnt.RadiusVectorComponent = v_comp
        gausspnt.RadiusTransferFunctionMode = 'Table'
        gausspnt.RadiusScalarRange = data_range
        gausspnt.RadiusUseScalarRange = 1
        gausspnt.RadiusIsProportional = 1
        gausspnt.RadiusProportionalFactor = mult
    else:
        gausspnt.RadiusTransferFunctionEnabled = 0
        gausspnt.RadiusMode = 'Constant'
        gausspnt.RadiusArray = ['POINTS', 'Constant Radius']

    return gausspnt

def GaussPointsOnField1(proxy, entity, field_name,
                        timestamp_nb,
                        is_colored=True, color=None,
                        primitive=GaussType.SPHERE,
                        is_proportional=True,
                        max_pixel_size=256,
                        multiplier=None,
                        vector_mode='Magnitude'):
    """Creates Gauss Points on the given field. Use GaussPoints() Paraview interface.

    Arguments:
    proxy: the pipeline object, containig data
    entity: the field entity type from PrsTypeEnum
    field_name: the field name
    timestamp_nb: the number of time step (1, 2, ...)
    is_colored -- defines whether the Gauss Points will be multicolored,
    using the corresponding data values
    color: defines the presentation color as [R, G, B] triple. Taken into
    account only if is_colored is False.
    primitive: primitive type from GaussType
    is_proportional: if True, the size of primitives will depends on
    the gauss point value
    max_pixel_size: the maximum sizr of the Gauss Points primitive in pixels
    multiplier: coefficient between data values and the size of primitives
    If not passed by user, default scale will be computed.
    vector_mode: the mode of transformation of vector values into
    scalar values, applicable only if the field contains vector values.
    Possible modes: 'Magnitude' - vector module;
    'X', 'Y', 'Z' - vector components.

    Returns:
      Gauss Points as representation object.

    """
    proxy.UpdatePipeline()
    select_cells_with_data(proxy, on_gauss=[field_name])

    nb_components = get_nb_components(proxy, entity, field_name)

    # Get time value
    time_value = get_time(proxy, timestamp_nb)

    # Set timestamp
    pvs.GetRenderView().ViewTime = time_value
    proxy.UpdatePipeline(time=time_value)

    # Create Gauss Points object
    source = pvs.GaussPoints(proxy)
    source.UpdatePipeline()
  
    # Get Gauss Points representation object
    gausspnt = pvs.GetRepresentation(source)

    # Get lookup table
    lookup_table = get_lookup_table(field_name, nb_components, vector_mode)

    # Set field range if necessary
    data_range = get_data_range(proxy, entity,
                                field_name, vector_mode)
    lookup_table.LockScalarRange = 1
    lookup_table.RGBPoints = [data_range[0], 0, 0, 1, data_range[1], 1, 0, 0]

    # Set display properties
    if is_colored:
        gausspnt.ColorAttributeType = EntityType.get_pvtype(entity)
        gausspnt.ColorArrayName = field_name
    else:
        gausspnt.ColorArrayName = ''
        if color:
            gausspnt.DiffuseColor = color

    gausspnt.LookupTable = lookup_table

    # Add scalar bar
    add_scalar_bar(field_name, nb_components,
                   vector_mode, lookup_table, time_value)

    # Set point sprite representation
    gausspnt.Representation = 'Point Sprite'

    # Point sprite settings
    gausspnt.InterpolateScalarsBeforeMapping = 0
    gausspnt.MaxPixelSize = max_pixel_size

    # Render mode
    gausspnt.RenderMode = GaussType.get_mode(primitive)

    #if primitive == GaussType.SPRITE:
        # Set texture
        # TODO(MZN): replace with pvsimple high-level interface
    #    texture = sm.CreateProxy("textures", "SpriteTexture")
    #    alphamprop = texture.GetProperty("AlphaMethod")
    #    alphamprop.SetElement(0, 2)  # Clamp
    #    alphatprop = texture.GetProperty("AlphaThreshold")
    #    alphatprop.SetElement(0, 63)
    #    maxprop = texture.GetProperty("Maximum")
    #    maxprop.SetElement(0, 255)
    #    texture.UpdateVTKObjects()

    #    gausspnt.Texture = texture
        #gausspnt.Texture.AlphaMethod = 'Clamp'
        #gausspnt.Texture.AlphaThreshold = 63
        #gausspnt.Texture.Maximum= 255

    # Proportional radius
    gausspnt.RadiusUseScalarRange = 0
    gausspnt.RadiusIsProportional = 0

    if is_proportional:
        mult = multiplier
        if mult is None:
            mult = abs(0.1 / data_range[1])

        gausspnt.RadiusScalarRange = data_range
        gausspnt.RadiusTransferFunctionEnabled = 1
        gausspnt.RadiusMode = 'Scalar'
        gausspnt.RadiusArray = ['POINTS', field_name]
        if nb_components > 1:
            v_comp = get_vector_component(vector_mode)
            gausspnt.RadiusVectorComponent = v_comp
        gausspnt.RadiusTransferFunctionMode = 'Table'
        gausspnt.RadiusScalarRange = data_range
        gausspnt.RadiusUseScalarRange = 1
        gausspnt.RadiusIsProportional = 1
        gausspnt.RadiusProportionalFactor = mult
    else:
        gausspnt.RadiusTransferFunctionEnabled = 0
        gausspnt.RadiusMode = 'Constant'
        gausspnt.RadiusArray = ['POINTS', 'Constant Radius']

    return gausspnt

def StreamLinesOnField(proxy, entity, field_name, timestamp_nb,
                       direction='BOTH', is_colored=False, color=None,
                       vector_mode='Magnitude'):
    """Creates Stream Lines presentation on the given field.

    Arguments:
      proxy: the pipeline object, containig data
      entity: the entity type from PrsTypeEnum
      field_name: the field name
      timestamp_nb: the number of time step (1, 2, ...)
      direction: the stream lines direction ('FORWARD', 'BACKWARD' or 'BOTH')
      is_colored: this option allows to color the presentation according to
      the corresponding data values. If False - the presentation will
      be one-coloured.
      color: defines the presentation color as [R, G, B] triple. Taken into
      account only if is_colored is False.
      vector_mode: the mode of transformation of vector values
      into scalar values, applicable only if the field contains vector values.
      Possible modes: 'Magnitude', 'X', 'Y' or 'Z'.

    Returns:
      Stream Lines as representation object.

    """
    proxy.UpdatePipeline()
    # We don't need mesh parts with no data on them
    if entity == EntityType.NODE:
        select_cells_with_data(proxy, on_points=[field_name])
    else:
        select_cells_with_data(proxy, on_cells=[field_name])

    # Check vector mode
    nb_components = get_nb_components(proxy, entity, field_name)
    check_vector_mode(vector_mode, nb_components)

    # Get time value
    time_value = get_time(proxy, timestamp_nb)

    # Set timestamp
    pvs.GetRenderView().ViewTime = time_value
    pvs.UpdatePipeline(time_value, proxy)

    # Do merge
    source = pvs.MergeBlocks(proxy)

    # Cell data to point data
    if is_data_on_cells(proxy, field_name):
        cell_to_point = pvs.CellDatatoPointData(source)
        cell_to_point.PassCellData = 1
        cell_to_point.UpdatePipeline()
        source = cell_to_point

    vector_array = field_name
    # If the given vector array has only 2 components, add the third one
    if nb_components == 2:
        calc = get_add_component_calc(source, EntityType.NODE, field_name)
        vector_array = calc.ResultArrayName
        calc.UpdatePipeline()
        source = calc

    # Stream Tracer
    stream = pvs.StreamTracer(source)
    stream.SeedType = "Point Source"
    stream.Vectors = ['POINTS', vector_array]
    stream.SeedType = "Point Source"
    stream.IntegrationDirection = direction
    stream.IntegratorType = 'Runge-Kutta 2'
    stream.UpdatePipeline()

    # Get Stream Lines representation object
    if is_empty(stream):
        return None
    streamlines = pvs.GetRepresentation(stream)

    # Get lookup table
    lookup_table = get_lookup_table(field_name, nb_components, vector_mode)

    # Set field range if necessary
    data_range = get_data_range(proxy, entity,
                                field_name, vector_mode)
    lookup_table.LockScalarRange = 1
    lookup_table.RGBPoints = [data_range[0], 0, 0, 1, data_range[1], 1, 0, 0]

    # Set properties
    if is_colored:
        streamlines.ColorAttributeType = EntityType.get_pvtype(entity)
        streamlines.ColorArrayName = field_name
    else:
        streamlines.ColorArrayName = ''
        if color:
            streamlines.DiffuseColor = color

    streamlines.LookupTable = lookup_table

    # Add scalar bar
    add_scalar_bar(field_name, nb_components,
                   vector_mode, lookup_table, time_value)

    return streamlines


def MeshOnEntity(proxy, mesh_name, entity):
    """Creates submesh of the entity type for the mesh.

    Arguments:
      proxy -- the pipeline object, containig data
      mesh_name -- the full or short name of mesh field

    Returns:
      Submesh as representation object of the given source.

    """
    proxy.UpdatePipeline()
    mesh_full_name = None
    aList = mesh_name.split('/')
    if len(aList) >= 2:
        mesh_full_name = mesh_name
    else:
        mesh_full_name = find_mesh_full_name(proxy, mesh_name)
    if not mesh_full_name:
        raise RuntimeError, "The given mesh name was not found"
    # Select only the given mesh
    proxy.AllArrays = []
    proxy.UpdatePipeline()
    proxy.AllArrays = [mesh_full_name]
    proxy.UpdatePipeline()

    # Get representation object if the submesh is not empty
    prs = None
    if (proxy.GetDataInformation().GetNumberOfPoints() or
        proxy.GetDataInformation().GetNumberOfCells()):
        prs = pvs.GetRepresentation(proxy)
        prs.ColorArrayName = ''

    return prs


def MeshOnGroup(proxy, extrGroups, group_name):
    """Creates submesh on the group.

    Arguments:
      proxy -- the pipeline object, containig data
      group_name -- the full group name
      extrGroups -- all extracted groups object

    Returns:
      Representation object of the given source with single group
      selected.

    """
    proxy.UpdatePipeline()
    # Deselect all groups
    extrGroups.AllGroups = []
    extrGroups.UpdatePipelineInformation()
    # Select only the group with the given name
    extrGroups.AllGroups = [group_name]
    extrGroups.UpdatePipelineInformation()

    # Get representation object if the submesh is not empty
    prs = None

    # Check if the group was set
    if len(extrGroups.AllGroups) == 1 and \
       extrGroups.AllGroups[0] == group_name:
        # Check if the submesh is not empty
        nb_points = proxy.GetDataInformation().GetNumberOfPoints()
        nb_cells = proxy.GetDataInformation().GetNumberOfCells()

        if nb_points or nb_cells:
#            prs = pvs.GetRepresentation(proxy)
            prs = pvs.Show()
            prs.ColorArrayName = ''
            display_only(prs)

    return prs


def CreatePrsForFile(paravis_instance, file_name, prs_types,
                     picture_dir, picture_ext):
    """Build presentations of the given types for the file.

    Build presentations for all fields on all timestamps.

    Arguments:
      paravis_instance: ParaVis module instance object
      file_name: full path to the MED file
      prs_types: the list of presentation types to build
      picture_dir: the directory path for saving snapshots
      picture_ext: graphics files extension (determines file type)

    """
    # Import MED file
    print "Import " + file_name.split(os.sep)[-1] + "..."

    try:
        proxy = pvs.MEDReader(FileName=file_name)
        if proxy is None:
            print "FAILED"
        else:
            proxy.UpdatePipeline()
            _med_field_sep = proxy.GetProperty("Separator")
            print "OK"
    except:
        print "FAILED"
    else:
        # Get view
        view = pvs.GetRenderView()

        # Create required presentations for the proxy
        CreatePrsForProxy(proxy, view, prs_types,
                          picture_dir, picture_ext)

def CreatePrsForProxy(proxy, view, prs_types, picture_dir, picture_ext):
    """Build presentations of the given types for all fields of the proxy.

    Save snapshots in graphics files (type depends on the given extension).
    Stores the files in the given directory.

    Arguments:
      proxy: the pipeline object, containig data
      view: the render view
      prs_types: the list of presentation types to build
      picture_dir: the directory path for saving snapshots
      picture_ext: graphics files extension (determines file type)

    """
    proxy.UpdatePipeline()
    # List of the field names
    fields_info = proxy.GetProperty("FieldsTreeInfo")[::2]
    print fields_info

    # Add path separator to the end of picture path if necessery
    if not picture_dir.endswith(os.sep):
        picture_dir += os.sep

    # Mesh Presentation
    if PrsTypeEnum.MESH in prs_types:
        # Iterate on meshes
        mesh_names = get_mesh_full_names(proxy)
        for mesh_name in mesh_names:
            # Build mesh field presentation
            print "Creating submesh for '" + get_field_short_name(mesh_name) + "' mesh... "
            prs = MeshOnEntity(proxy, mesh_name, None)
            if prs is None:
                print "FAILED"
                continue
            else:
                print "OK"
            # Construct image file name
            pic_name = picture_dir + get_field_short_name(mesh_name) + "." + picture_ext
            
            # Show and dump the presentation into a graphics file
            process_prs_for_test(prs, view, pic_name, False)

        # Create Mesh presentation. Build all groups.
        extGrp = pvs.ExtractGroup()
        extGrp.UpdatePipelineInformation()
        if if_possible(proxy, None, None, PrsTypeEnum.MESH, extGrp):
            for group in get_group_names(extGrp):
                print "Creating submesh on group " + get_group_short_name(group) + "... "
                prs = MeshOnGroup(proxy, extGrp, group)
                if prs is None:
                    print "FAILED"
                    continue
                else:
                    print "OK"
                # Construct image file name
                pic_name = picture_dir + get_group_short_name(group) + "." + picture_ext
                    
                # Show and dump the presentation into a graphics file
                process_prs_for_test(prs, view, pic_name, False)

    # Presentations on fields
    for field in fields_info:
        field_name = get_field_short_name(field)
        # Ignore mesh presentation
        if field_name == get_field_mesh_name(field):
            continue
        field_entity = get_field_entity(field)
        # Clear fields selection state
        proxy.AllArrays = []
        proxy.UpdatePipeline()
        # Select only the current field:
        # necessary for getting the right timestamps
        proxy.AllArrays = field
        proxy.UpdatePipeline()

        # Get timestamps
        entity_data_info = proxy.GetCellDataInformation()
        timestamps = proxy.TimestepValues.GetData()

        for prs_type in prs_types:
            # Ignore mesh presentation
            if prs_type == PrsTypeEnum.MESH:
                continue

            # Get name of presentation type
            prs_name = PrsTypeEnum.get_name(prs_type)

            # Build the presentation if possible
            possible = if_possible(proxy, field_name,
                                   field_entity, prs_type)
            if possible:
                # Presentation type for graphics file name
                f_prs_type = prs_name.replace(' ', '').upper()

                for timestamp_nb in xrange(1, len(timestamps) + 1):
                    time = timestamps[timestamp_nb - 1]
                    print "Creating " + prs_name + " on " + field_name + ", time = " + str(time) + "... "
                    prs = create_prs(prs_type, proxy,
                                     field_entity, field_name, timestamp_nb)
                    if prs is None:
                        print "FAILED"
                        continue
                    else:
                        print "OK"

                    # Construct image file name
                    pic_name = picture_dir + field_name + "_" + str(time) + "_" + f_prs_type + "." + picture_ext

                    # Show and dump the presentation into a graphics file
                    process_prs_for_test(prs, view, pic_name)
    return
