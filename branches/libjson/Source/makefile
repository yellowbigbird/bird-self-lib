# PROJROOT must be provided by dirmake from source project
# or from environment, or from nmake command line

MODULE=libjson
MODDESC=libjson
MODTYPE=KEXLIBDLINK

!if "$(_DEBUG)" == "1"
CPPFLAGS=$(CPPFLAGS) -DJSON_DEBUG -D_MBCS
!else
CPPFLAGS=$(CPPFLAGS) -DNDEBUG -D_MBCS
!endif


!include $(ROOT)\devpriv\make.inc

OBJS=\
	$(OBJDIR)\internalJSONNode.obj\
	$(OBJDIR)\JSON_Base64.obj\
	$(OBJDIR)\JSONAllocator.obj\
	$(OBJDIR)\JSONChildren.obj\
	$(OBJDIR)\JSONDebug.obj\
	$(OBJDIR)\JSONIterators.obj\
	$(OBJDIR)\JSONMemory.obj\
	$(OBJDIR)\JSONNode.obj\
	$(OBJDIR)\JSONNode_Mutex.obj\
	$(OBJDIR)\JSONStream.obj\
	$(OBJDIR)\JSONValidator.obj\
	$(OBJDIR)\JSONWorker.obj\
	$(OBJDIR)\JSONWriter.obj\
	$(OBJDIR)\libjson.obj\

MODLIBS=\



!include $(ROOT)\devpriv\make2.inc
