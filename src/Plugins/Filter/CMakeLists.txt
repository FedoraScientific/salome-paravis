
# create a paraview plugin containing server manager xml and the server
# manager classes to build
# this plugin can be loaded on the server side

ADD_PARAVIEW_PLUGIN(SMSampleFilter "1.0"
  SERVER_MANAGER_XML SampleFilter.xml 
  SERVER_MANAGER_SOURCES vtkMySampleFilter.cxx)

INSTALL(
	TARGETS SMSampleFilter 
	DESTINATION .
)
