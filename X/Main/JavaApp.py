import abc
import wx
import os
import javabridge

class JavaApp():
    def start(self, script, params=dict()):
        pass

class CmdJavaApp(JavaApp):
    def __init__(self):
        javabridge.start_vm(run_headless=True)

    def start(self, script, params=dict()):
        print(javabridge.run_script(script, params))
        javabridge.kill_vm()

class AwtJavaApp(wx.App, JavaApp):
    def __init__(self):
        super(AwtJavaApp, self).__init__(False)

    def OnInit(self):
        javabridge.start_vm()
        return True

    def start(self, script, params=dict()):
        javabridge.activate_awt()
        # Must exist (perhaps the app needs to have a top-level window?), but
        # does not have to be shown.
        frame = wx.Frame(None)

        javabridge.execute_runnable_in_main_thread(javabridge.run_script(script, params))
    
        self.MainLoop()

        javabridge.kill_vm()

def test_cmd_java_app():
    cmdApp = CmdJavaApp()

    script = 'java.lang.String.format("Hello, %s!", greetee);'
    params =  dict(greetee='world')

    cmdApp.start(script, params)

def test_awt_java_app():
    # GUI app
    awtApp = AwtJavaApp()

    script = """
        new java.lang.Runnable() {
            run: function() {
                with(JavaImporter(java.awt.Frame)) Frame().setVisible(true);
            }
        };"""

    awtApp.start(script)

if __name__ == "__main__":

    # command line app
    #test_cmd_java_app()

    # GUI app
    test_awt_java_app()

