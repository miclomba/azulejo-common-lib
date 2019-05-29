import unittest

from JavaApp import AwtJavaApp, CmdJavaApp

class Test_JavaApp(unittest.TestCase):

    def test_cmd_java_app(self):
        cmdApp = CmdJavaApp()

        script = 'java.lang.String.format("Hello, %s!", greetee);'
        params =  dict(greetee='world')

        cmdApp.start(script, params)

    def test_awt_java_app(self):
        # GUI app
        awtApp = AwtJavaApp()

        script = """
            new java.lang.Runnable() {
                run: function() {
                    with(JavaImporter(java.awt.Frame)) Frame().setVisible(true);
                }
            };"""

        #awtApp.start(script)

if __name__ == '__main__':
    unittest.main()
