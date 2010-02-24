r"""simple is a module for using paraview server manager in Python. It 
provides a simple convenience layer to functionality provided by the
C++ classes wrapped to Python as well as the servermanager module.

A simple example:
  from paraview.simple import *

  # Create a new sphere proxy on the active connection and register it
  # in the sources group.
  sphere = Sphere(ThetaResolution=16, PhiResolution=32)

  # Apply a shrink filter
  shrink = Shrink(sphere)
  
  # Turn the visiblity of the shrink object on.
  Show(shrink)
  
  # Render the scene
  Render()
"""

import paravisSM

servermanager = paravisSM

def _disconnect():
    servermanager.ProxyManager().UnRegisterProxies()
    active_objects.view = None
    active_objects.source = None
    import gc
    gc.collect()
    servermanager.Disconnect()

def Connect(ds_host=None, ds_port=11111, rs_host=None, rs_port=11111):
    """Creates a connection to a server. Example usage:
    > Connect("amber") # Connect to a single server at default port
    > Connect("amber", 12345) # Connect to a single server at port 12345
    > Connect("amber", 11111, "vis_cluster", 11111) # connect to data server, render server pair"""
    _disconnect()
    cid = servermanager.Connect(ds_host, ds_port, rs_host, rs_port)
    servermanager.ProxyManager().RegisterProxy("timekeeper", "tk", servermanager.misc.TimeKeeper())
    return cid

def ReverseConnect(port=11111):
    """Create a reverse connection to a server.  Listens on port and waits for
    an incoming connection from the server."""
    _disconnect()
    cid = servermanager.ReverseConnect(port)
    servermanager.ProxyManager().RegisterProxy("timekeeper", "tk", servermanager.misc.TimeKeeper())
    return cid

def _create_view(view_xml_name):
    "Creates and returns a 3D render view."
    view = servermanager._create_view(view_xml_name)
    servermanager.ProxyManager().RegisterProxy("views", \
      "my_view%d" % _funcs_internals.view_counter, view)
    active_objects.view = view
    _funcs_internals.view_counter += 1
    
    tk = servermanager.ProxyManager().GetProxiesInGroup("timekeeper").values()[0]
    views = tk.Views
    if not view in views:
        views.append(view)

    return view

def CreateRenderView():
    return _create_view("RenderView")

def CreateXYPlotView():
    return _create_view("XYPlotView")

def CreateBarChartView():
    return _create_view("BarChart")

def GetRenderView():
    "Returns the active view if there is one. Else creates and returns a new view."
    view = active_objects.view
    if not view: view = CreateRenderView()
    return view

def GetRenderViews():
    "Returns all render views as a list."
    return servermanager.GetRenderViews()

def GetRepresentation(proxy=None, view=None):
    """"Given a pipeline object and view, returns the corresponding representation object.
    If pipeline object and view are not specified, active objects are used."""
    if not view:
        view = active_objects.view
    if not proxy:
        proxy = active_objects.source
    rep = servermanager.GetRepresentation(proxy, view)
    if not rep:
        rep = servermanager.CreateRepresentation(proxy, view)
        servermanager.ProxyManager().RegisterProxy("representations", \
          "my_representation%d" % _funcs_internals.rep_counter, rep)
        _funcs_internals.rep_counter += 1
    return rep
    
def GetDisplayProperties(proxy=None, view=None):
    """"Given a pipeline object and view, returns the corresponding representation object.
    If pipeline object and/or view are not specified, active objects are used."""
    return GetRepresentation(proxy, view)
    
def Show(proxy=None, view=None, **params):
    """Turns the visibility of a given pipeline object on in the given view.
    If pipeline object and/or view are not specified, active objects are used."""
    if proxy == None:
        proxy = GetActiveSource()
    if proxy == None:
        raise RuntimeError, "Show() needs a proxy argument or that an active source is set."
    if not view and not active_objects.view:
        CreateRenderView()
    rep = GetDisplayProperties(proxy, view)
    if rep == None:
        raise RuntimeError, "Could not create a representation object for proxy %s" % proxy.GetXMLLabel()
    for param in params.keys():
        setattr(rep, param, params[param])
    rep.Visibility = 1
    return rep

def Hide(proxy=None, view=None):
    """Turns the visibility of a given pipeline object off in the given view.
    If pipeline object and/or view are not specified, active objects are used."""
    rep = GetDisplayProperties(proxy, view)
    rep.Visibility = 0

def Render(view=None):
    """Renders the given view (default value is active view)"""
    if not view:
        view = active_objects.view
    view.StillRender()
    if _funcs_internals.first_render:
        # Not all views have a ResetCamera method
        try:
            view.ResetCamera()
            view.StillRender()
        except AttributeError: pass
        _funcs_internals.first_render = False
    return view
        
def ResetCamera(view=None):
    """Resets the settings of the camera to preserver orientation but include
    the whole scene. If an argument is not provided, the active view is
    used."""
    if not view:
        view = active_objects.view
    view.ResetCamera()
    Render(view)

def SetProperties(proxy=None, **params):
    """Sets one or more properties of the given pipeline object. If an argument
    is not provided, the active source is used. Pass a list of property_name=value
    pairs to this function to set property values. For example:
     SetProperties(Center=[1, 2, 3], Radius=3.5)
    """
    if not proxy:
        proxy = active_objects.source
    for param in params.keys():
        if not hasattr(proxy, param):
            raise AttributeError("object has no property %s" % param)
        setattr(proxy, param, params[param])

def SetDisplayProperties(proxy=None, view=None, **params):
    """Sets one or more display properties of the given pipeline object. If an argument
    is not provided, the active source is used. Pass a list of property_name=value
    pairs to this function to set property values. For example:
     SetProperties(Color=[1, 0, 0], LineWidth=2)
    """
    rep = GetDisplayProperties(proxy, view)
    SetProperties(rep, **params)

def SetViewProperties(view=None, **params):
    """Sets one or more properties of the given view. If an argument
    is not provided, the active view is used. Pass a list of property_name=value
    pairs to this function to set property values. For example:
     SetProperties(Background=[1, 0, 0], UseImmediateMode=0)
    """
    if not view:
        view = active_objects.view
    SetProperties(view, **params)

def RenameSource(newName, proxy=None):
    """Renames the given source.  If the given proxy is not registered
    in the sources group this method will have no effect.  If no source is
    provided, the active source is used."""
    if not proxy:
        proxy = active_objects.source
    pxm = servermanager.ProxyManager()
    oldName = pxm.GetProxyName("sources", proxy)
    if oldName:
      pxm.RegisterProxy("sources", newName, proxy)
      pxm.UnRegisterProxy("sources", oldName, proxy)

def FindSource(name):
    return servermanager.ProxyManager().GetProxy("sources", name)

def GetSources():
    """Given the name of a source, return its Python object."""
    return servermanager.ProxyManager().GetProxiesInGroup("sources")

def GetRepresentations():
    """Returns all representations (display properties)."""
    return servermanager.ProxyManager().GetProxiesInGroup("representations")

def UpdatePipeline(time=None, proxy=None):
    """Updates (executes) the given pipeline object for the given time as
    necessary (i.e. if it did not already execute). If no source is provided,
    the active source is used instead."""
    if not proxy:
        proxy = active_objects.source
    if time:
        proxy.UpdatePipeline(time)
    else:
        proxy.UpdatePipeline()

def Delete(proxy=None):
    """Deletes the given pipeline object or the active source if no argument
    is specified."""
    if not proxy:
        proxy = active_objects.source
    # Unregister any helper proxies stored by a vtkSMProxyListDomain
    for prop in proxy:
        listdomain = prop.GetDomain('proxy_list')
        if listdomain:
            if listdomain.GetClassName() != 'vtkSMProxyListDomain':
                continue
            group = "pq_helper_proxies." + proxy.GetSelfIDAsString()
            for i in xrange(listdomain.GetNumberOfProxies()):
                pm = servermanager.ProxyManager()
                iproxy = listdomain.GetProxy(i)
                name = pm.GetProxyName(group, iproxy)
                if iproxy and name:
                    pm.UnRegisterProxy(group, name, iproxy)
                    
    # Remove source/view from time keeper
    tk = servermanager.ProxyManager().GetProxiesInGroup("timekeeper").values()[0]
    if isinstance(proxy, servermanager.SourceProxy):
        try:
            idx = tk.TimeSources.index(proxy)
            del tk.TimeSources[idx]
        except ValueError:
            pass
    else:
        try:
            idx = tk.Views.index(proxy)
            del tk.Views[idx]
        except ValueError:
            pass
    servermanager.UnRegister(proxy)
    
    # If this is a representation, remove it from all views.
    if proxy.SMProxy.IsA("vtkSMRepresentationProxy"):
        for view in GetRenderViews():
            view.Representations.remove(proxy)
    # If this is a source, remove the representation iff it has no consumers
    # Also change the active source if necessary
    elif proxy.SMProxy.IsA("vtkSMSourceProxy"):
        sources = servermanager.ProxyManager().GetProxiesInGroup("sources")
        for i in range(proxy.GetNumberOfConsumers()):
            if proxy.GetConsumerProxy(i) in sources:
                raise RuntimeError("Source has consumers. It cannot be deleted " +
                  "until all consumers are deleted.")
        #VSV:==
        if proxy.IsSame(GetActiveSource()):
            if hasattr(proxy, "Input") and proxy.Input:
                if isinstance(proxy.Input, servermanager.Proxy):
                    SetActiveSource(proxy.Input)
                else:
                    SetActiveSource(proxy.Input[0])
            else: SetActiveSource(None)
        for rep in GetRepresentations().values():
            #VSV:==
            if rep.Input.IsSame(proxy):
                Delete(rep)
    # Change the active view if necessary
    elif proxy.SMProxy.IsA("vtkSMRenderViewProxy"):
        ##VSV:==
        if proxy.IsSame(GetActiveView()):
            if len(GetRenderViews()) > 0:
                SetActiveView(GetRenderViews()[0])
            else:
                SetActiveView(None)

def CreateLookupTable(**params):
    """Create and return a lookup table.  Optionally, parameters can be given
    to assign to the lookup table.
    """
    lt = servermanager.rendering.PVLookupTable()
    servermanager.Register(lt)
    SetProperties(lt, **params)
    return lt

def CreatePiecewiseFunction(**params):
    """Create and return a piecewise function.  Optionally, parameters can be
    given to assign to the piecewise function.
    """
    pfunc = servermanager.piecewise_functions.PiecewiseFunction()
    servermanager.Register(pfunc)
    SetProperties(pfunc, **params)
    return pfunc

def CreateScalarBar(**params):
    """Create and return a scalar bar widget.  The returned widget may
    be added to a render view by appending it to the view's representations
    The widget must have a valid lookup table before it is added to a view.
    It is possible to pass the lookup table (and other properties) as arguments
    to this method:
    
    lt = MakeBlueToRedLt(3.5, 7.5)
    bar = CreateScalarBar(LookupTable=lt, Title="Velocity")
    GetRenderView().Representations.append(bar)
    
    By default the returned widget is selectable and resizable.
    """
    sb = servermanager.rendering.ScalarBarWidgetRepresentation()
    servermanager.Register(sb)
    sb.Selectable = 1
    sb.Resizable = 1
    sb.Enabled = 1
    sb.Title = "Scalars"
    SetProperties(sb, **params)
    return sb

# TODO: Change this to take the array name and number of components. Register 
# the lt under the name ncomp.array_name
def MakeBlueToRedLT(min, max):
    # Define RGB points. These are tuples of 4 values. First one is
    # the scalar values, the other 3 the RGB values. 
    rgbPoints = [min, 0, 0, 1, max, 1, 0, 0]
    return CreateLookupTable(RGBPoints=rgbPoints, ColorSpace="HSV")
    
def _find_writer(filename):
    "Internal function."
    extension = None
    parts = filename.split('.')
    if len(parts) > 1:
        extension = parts[-1]
    else:
        raise RuntimeError, "Filename has no extension, please specify a write"
        
    if extension == 'png':
        return 'vtkPNGWriter'
    elif extension == 'bmp':
        return 'vtkBMPWriter'
    elif extension == 'ppm':
        return 'vtkPNMWriter'
    elif extension == 'tif' or extension == 'tiff':
        return 'vtkTIFFWriter'
    elif extension == 'jpg' or extension == 'jpeg':
        return 'vtkJPEGWriter'
    else:
        raise RuntimeError, "Cannot infer filetype from extension:", extension

def AddCameraLink(viewProxy, viewProxyOther, linkName):
    """Create a camera link between two view proxies.  A name must be given
    so that the link can be referred to by name.  If a link with the given
    name already exists it will be removed first."""
    if not viewProxyOther: viewProxyOther = GetActiveView()
    link = servermanager.vtkSMCameraLink()
    link.AddLinkedProxy(viewProxy.SMProxy, 1)
    link.AddLinkedProxy(viewProxyOther.SMProxy, 2)
    link.AddLinkedProxy(viewProxyOther.SMProxy, 1)
    link.AddLinkedProxy(viewProxy.SMProxy, 2)
    RemoveCameraLink(linkName)
    servermanager.ProxyManager().RegisterLink(linkName, link)

def RemoveCameraLink(linkName):
    """Remove a camera link with the given name."""
    servermanager.ProxyManager().UnRegisterLink(linkName)

def WriteImage(filename, view=None, **params):
    """Saves the given view (or the active one if none is given) as an
    image. Optionally, you can specify the writer and the magnification
    using the Writer and Magnification named arguments. For example:
     WriteImage("foo.mypng", aview, Writer=vtkPNGWriter, Magnification=2)
    If no writer is provided, the type is determined from the file extension.
    Currently supported extensions are png, bmp, ppm, tif, tiff, jpg and jpeg.
    The writer is a VTK class that is capable of writing images.
    Magnification is used to determine the size of the written image. The size
    is obtained by multiplying the size of the view with the magnification.
    Rendering may be done using tiling to obtain the correct size without
    resizing the view."""
    if not view:
        view = active_objects.view
    writer = None
    if params.has_key('Writer'):
        writer = params['Writer']
    mag = 1
    if params.has_key('Magnification'):
        mag = int(params['Magnification'])
    if not writer:
        writer = _find_writer(filename)
    view.WriteImage(filename, writer, mag)

def AnimateReader(reader=None, view=None, filename=None):
    """This is a utility function that, given a reader and a view
    animates over all time steps of the reader. If the optional
    filename is provided, a movie is created (type depends on the
    extension of the filename."""
    if not reader:
        reader = active_objects.source
    if not view:
        view = active_objects.view
        
    return servermanager.AnimateReader(reader, view, filename)


def _create_func(key, module):
    "Internal function."

    def CreateObject(*input, **params):
        """This function creates a new proxy. For pipeline objects that accept inputs,
        all non-keyword arguments are assumed to be inputs. All keyword arguments are
        assumed to be property,value pairs and are passed to the new proxy."""

        # Instantiate the actual object from the given module.
        px = module.__dict__[key]()

        # Make sure non-keyword arguments are valid
        for inp in input:
            if inp != None and not isinstance(inp, servermanager.Proxy):
                if px.GetProperty("Input") != None:
                    raise RuntimeError, "Expecting a proxy as input."
                else:
                    raise RuntimeError, "This function does not accept non-keyword arguments."

        # Assign inputs
        if px.GetProperty("Input") != None:
            if len(input) > 0:
                px.Input = input
            else:
                # If no input is specified, try the active pipeline object
                if px.GetProperty("Input").GetRepeatable() and active_objects.get_selected_sources():
                    px.Input = active_objects.get_selected_sources()
                elif active_objects.source:
                    px.Input = active_objects.source
        else:
            if len(input) > 0:
                raise RuntimeError, "This function does not expect an input."

        registrationName = None
        for nameParam in ['registrationName', 'guiName']:
          if nameParam in params:
              registrationName = params[nameParam]
              del params[nameParam]

        # Pass all the named arguments as property,value pairs
        for param in params.keys():
            setattr(px, param, params[param])

        try:
            # Register the proxy with the proxy manager.
            if registrationName:
                group, name = servermanager.Register(px, registrationName=registrationName)
            else:
                group, name = servermanager.Register(px)


            # Register pipeline objects with the time keeper. This is used to extract time values
            # from sources. NOTE: This should really be in the servermanager controller layer.
            if group == "sources":
                tk = servermanager.ProxyManager().GetProxiesInGroup("timekeeper").values()[0]
                sources = tk.TimeSources
                if not px in sources:
                    sources.append(px)

                active_objects.source = px
        except servermanager.MissingRegistrationInformation:
            pass

        return px

    return CreateObject

def _create_doc(new, old):
    "Internal function."
    import string
    res = ""
    for doc in (new, old):
        ts = []
        strpd = doc.split('\n')
        for s in strpd:
            ts.append(s.lstrip())
        res += string.join(ts)
        res += '\n'
    return res
    
def _func_name_valid(name):
    "Internal function."
    valid = True
    for c in name:
        if c == '(' or c ==')':
            valid = False
            break
    return valid

def _add_functions(g):
    for m in [servermanager.filters, servermanager.sources, servermanager.writers]:
        dt = m.__dict__
        for key in dt.keys():
            cl = dt[key]
            if not isinstance(cl, str):
                if not key in g and _func_name_valid(key):
                    g[key] = _create_func(key, m)
                    exec "g[key].__doc__ = _create_doc(m.%s.__doc__, g[key].__doc__)" % key

def GetActiveView():
    "Returns the active view."
    return active_objects.view
    
def SetActiveView(view):
    "Sets the active view."
    active_objects.view = view
    
def GetActiveSource():
    "Returns the active source."
    return active_objects.source
    
def SetActiveSource(source):
    "Sets the active source."
    active_objects.source = source
    
def GetActiveCamera():
    """Returns the active camera for the active view. The returned object
    is an instance of vtkCamera."""
    return GetActiveView().GetActiveCamera()

def LoadXML(xmlstring, ns=None):
    """Given a server manager XML as a string, parse and process it.
    If you loaded the simple module with from paraview.simple import *,
    make sure to pass globals() as the second arguments:
    LoadXML(xmlstring, globals())
    Otherwise, the new functions will not appear in the global namespace."""
    if not ns:
        ns = globals()
    servermanager.LoadXML(xmlstring)
    _add_functions(ns)

def LoadPlugin(filename, remote=True, ns=None):
    """Loads a ParaView plugin and updates this module with new constructors
    if any. The remote argument (default to True) is to specify whether 
    the plugin will be loaded on client (remote=False) or on server (remote=True).
    If you loaded the simple module with from paraview.simple import *,
    make sure to pass globals() as an argument:
    LoadPlugin("myplugin", False, globals()), to load on client;
    LoadPlugin("myplugin", True, globals()), to load on server; 
    LoadPlugin("myplugin", ns=globals()), to load on server.
    Otherwise, the new functions will not appear in the global namespace."""
    
    if not ns:
        ns = globals()
    servermanager.LoadPlugin(filename, remote)
    _add_functions(ns)

class ActiveObjects(object):
    """This class manages the active objects (source and view). The active
    objects are shared between Python and the user interface. This class
    is for internal use. Use the Set/Get methods for setting and getting
    active objects."""
    def __get_selection_model(self, name):
        "Internal method."
        pxm = servermanager.ProxyManager()
        model = pxm.GetSelectionModel(name)
        if not model:
            model = servermanager.vtkSMProxySelectionModel()
            pxm.RegisterSelectionModel(name, model)
        return model

    def set_view(self, view):
        "Sets the active view."
        active_view_model = self.__get_selection_model("ActiveView") 
        if view:
            active_view_model.SetCurrentProxy(view.SMProxy, 0)
        else:
            active_view_model.SetCurrentProxy(None, 0)

    def get_view(self):
        "Returns the active view."
        return servermanager._getPyProxy(
            self.__get_selection_model("ActiveView").GetCurrentProxy())

    def set_source(self, source):
        "Sets the active source."
        active_sources_model = self.__get_selection_model("ActiveSources") 
        if source:
            # 3 == CLEAR_AND_SELECT
            active_sources_model.SetCurrentProxy(source.SMProxy, 3)
        else:
            active_sources_model.SetCurrentProxy(None, 3)

    def __convert_proxy(self, px):
        "Internal method."
        if not px:
            return None
        if px.IsA("vtkSMSourceProxy"):
            return servermanager._getPyProxy(px)
        else:
            return servermanager.OutputPort(
              servermanager._getPyProxy(px.GetSourceProxy()),
              px.GetPortIndex())
        
    def get_source(self):
        "Returns the active source."
        return self.__convert_proxy(
          self.__get_selection_model("ActiveSources").GetCurrentProxy())

    def get_selected_sources(self):
        "Returns the set of sources selected in the pipeline browser."
        model = self.__get_selection_model("ActiveSources")
        proxies = []
        for i in xrange(model.GetNumberOfSelectedProxies()):
            proxies.append(self.__convert_proxy(model.GetSelectedProxy(i)))
        return proxies

    view = property(get_view, set_view)
    source = property(get_source, set_source)

class _funcs_internals:
    "Internal class."
    first_render = True
    view_counter = 0
    rep_counter = 0

def demo1():
    """Simple demo that create the following pipeline
    sphere - shrink - \
                       - append
    cone            - /
    """
    # Create a sphere of radius = 2, theta res. = 32
    # This object becomes the active source.
    ss = Sphere(Radius=2, ThetaResolution=32)
    # Apply the shrink filter. The Input property is optional. If Input
    # is not specified, the filter is applied to the active source.
    shr = Shrink(Input=ss)
    # Create a cone source.
    cs = Cone()
    # Append cone and shrink
    app = AppendDatasets()
    app.Input = [shr, cs]
    # Show the output of the append filter. The argument is optional
    # as the app filter is now the active object.
    Show(app)
    # Render the default view.
    Render()

def demo2(fname="/Users/berk/Work/ParaView/ParaViewData/Data/disk_out_ref.ex2"):
    """This demo shows the use of readers, data information and display
    properties."""
    
    # Create the exodus reader and specify a file name
    reader = ExodusIIReader(FileName=fname)
    # Get the list of point arrays.
    avail = reader.PointVariables.Available
    print avail
    # Select all arrays
    reader.PointVariables = avail

    # Turn on the visibility of the reader
    Show(reader)
    # Set representation to wireframe
    SetDisplayProperties(Representation = "Wireframe")
    # Black background is not pretty
    SetViewProperties(Background = [0.4, 0.4, 0.6])
    Render()
    # Change the elevation of the camera. See VTK documentation of vtkCamera
    # for camera parameters.
    # NOTE: THIS WILL BE SIMPLER
    GetActiveCamera().Elevation(45)
    Render()
    # Now that the reader executed, let's get some information about it's
    # output.
    pdi = reader[0].PointData
    # This prints a list of all read point data arrays as well as their
    # value ranges.
    print 'Number of point arrays:', len(pdi)
    for i in range(len(pdi)):
        ai = pdi[i]
        print "----------------"
        print "Array:", i, " ", ai.Name, ":"
        numComps = ai.GetNumberOfComponents()
        print "Number of components:", numComps
        for j in range(numComps):
            print "Range:", ai.GetRange(j)
    # White is boring. Let's color the geometry using a variable.
    # First create a lookup table. This object controls how scalar
    # values are mapped to colors. See VTK documentation for
    # details.
    # Map min (0.00678) to blue, max (0.0288) to red
    SetDisplayProperties(LookupTable = MakeBlueToRedLT(0.00678, 0.0288))
    # Color by point array called Pres
    SetDisplayProperties(ColorAttributeType = "POINT_DATA")
    SetDisplayProperties(ColorArrayName = "Pres")
    Render()

def PrintTrace():
    print paravisSM.myParavis.GetTrace()

def SaveTrace(fileName):
    paravisSM.myParavis.SaveTrace(fileName)


_add_functions(globals())
active_objects = ActiveObjects()
## Initialisation for SALOME GUI
active_objects.view = servermanager.GetRenderView()

if not servermanager.ActiveConnection:
    Connect()

def ImportFile(theFileName):
    paravisSM.ImportFile(theFileName)
