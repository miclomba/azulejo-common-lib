#requires jre/jdk-8u211-windows-x64
#requires pip install (numpy, javabridge, wxPython)
import javabridge
import os
import wx

class CmdJavaApp():
    def __init__(self):
        pass

    def start(self, script, paramsIn=dict(), paramsOut=dict()):
        print(javabridge.run_script(script, bindings_in=paramsIn, bindings_out=paramsOut))

class AwtJavaApp(wx.App):
    def __init__(self):
        super(AwtJavaApp, self).__init__(False)

    def OnInit(self):
        return True

    def start(self, script, paramsIn=dict(), paramsOut=dict()):
        javabridge.activate_awt()
        # Must exist (perhaps the app needs to have a top-level window?), but
        # does not have to be shown.
        frame = wx.Frame(None)

        javabridge.execute_runnable_in_main_thread(javabridge.run_script(script, bindings_in=paramsIn, bindings_out=paramsOut))
    
        self.MainLoop()
